/**
 * @file test_conversion_extension.cpp
 * @brief Contract tests for the documented conversion-graph extension workflow.
 *
 * The documented pattern is:
 *   1. Include the graph infrastructure (conversion.hpp) — this does not
 *      instantiate the global node set.
 *   2. Specialize graph::additional_color_nodes<> and register conversion
 *      edges with the COLORCPP_REGISTER_CONVERSION* macros.
 *   3. Include the rest of the library (e.g. the colorcpp.hpp umbrella).
 *
 * Steps 2 must precede any header that converts concrete color types at
 * namespace scope (palette/theme, io/css, ...), otherwise the node set is
 * locked before the specialization.
 */

#include <gtest/gtest.h>

#include <colorcpp/operations/conversion.hpp>

// ---------------------------------------------------------------------------
// Custom graph node registered from user code, after the include.
// ---------------------------------------------------------------------------

struct probe_uv {
  float u{};
  float v{};
};

namespace {

probe_uv rgbf_to_probe_uv(const colorcpp::core::rgbf_t& c) { return {c.r(), c.g()}; }

colorcpp::core::rgbf_t probe_uv_to_rgbf(const probe_uv& c) { return colorcpp::core::rgbf_t(c.u, c.v, 0.5f); }

// Direct probe_uv -> xyz edge, deliberately more expensive (4) than the
// multi-hop route probe_uv -> rgbf -> linear_rgb -> xyz (1 + 1 + 1 = 3).
// Routing must pick the cheaper chain; if edge weights were ignored this
// direct hop would win instead.
colorcpp::core::xyz_t probe_uv_to_xyz(const probe_uv&) { return colorcpp::core::xyz_t(1.0f, 0.0f, 0.0f); }

}  // namespace

template <>
struct colorcpp::operations::conversion::graph::additional_color_nodes<> {
  using type = colorcpp::operations::conversion::graph::node_list<probe_uv>;
};

// Documented usage: registration macros at namespace scope of user code.
COLORCPP_REGISTER_CONVERSION(colorcpp::core::rgbf_t, probe_uv, rgbf_to_probe_uv)
COLORCPP_REGISTER_CONVERSION(probe_uv, colorcpp::core::rgbf_t, probe_uv_to_rgbf)
COLORCPP_REGISTER_CONVERSION_WEIGHTED(probe_uv, colorcpp::core::xyz_t, probe_uv_to_xyz, 4)

// Graph fully configured; pulling the umbrella (and everything with
// namespace-scope concrete conversions) after this point is safe.
#include <colorcpp/colorcpp.hpp>

namespace colorcpp::test {

using namespace core;
using namespace operations::conversion;

TEST(ConversionExtensionTest, UserNodeJoinsGlobalGraph) {
  static_assert(graph::endpoints_in_global_graph_v<probe_uv, xyz_t>,
                "specialized additional_color_nodes must add probe_uv to the global node set");
  static_assert(graph::contains_node_v<graph::global_color_nodes_t<>, probe_uv>);
}

TEST(ConversionExtensionTest, WeightedRoutePrefersCheapChainOverExpensiveDirect) {
  // Cost 3 via rgbf must beat the cost-4 direct edge.
  static_assert(graph::minimal_conversion_cost<probe_uv, xyz_t>() == 3);
  static_assert(std::is_same_v<graph::next_hop_t<probe_uv, xyz_t>, rgbf_t>);

  // Functional anchor: the result must follow the rgbf chain, not the direct
  // edge's fixed sentinel (1, 0, 0).
  const auto xyz = color_cast<xyz_t>(probe_uv{1.0f, 0.5f});
  const auto expected = color_cast<xyz_t>(color_cast<linear_rgbf_t>(rgbf_t(1.0f, 0.5f, 0.5f)));
  EXPECT_NEAR(xyz.x(), expected.x(), 1e-6);
  EXPECT_NEAR(xyz.y(), expected.y(), 1e-6);
  EXPECT_NEAR(xyz.z(), expected.z(), 1e-6);
}

TEST(ConversionExtensionTest, UserNodeReturnRouteUsesRgbChain) {
  // xyz -> linear_rgb -> rgbf -> probe_uv (cost 3); no reverse direct edge.
  static_assert(graph::minimal_conversion_cost<xyz_t, probe_uv>() == 3);
  static_assert(std::is_same_v<graph::next_hop_t<xyz_t, probe_uv>, linear_rgbf_t>);

  const xyz_t gray(0.5f, 0.5f, 0.5f);
  const auto uv = color_cast<probe_uv>(gray);
  EXPECT_GT(uv.u, 0.0f);
  EXPECT_GT(uv.v, 0.0f);
}

TEST(ConversionExtensionTest, UserNodeRoutesIntoBuiltinSpaces) {
  // probe_uv -> rgbf (cost 1) -> oklab (direct edge, cost 1).
  static_assert(std::is_same_v<graph::next_hop_t<probe_uv, oklab_t>, rgbf_t>);
  static_assert(graph::minimal_conversion_cost<probe_uv, oklab_t>() == 2);

  const auto ok = color_cast<oklab_t>(probe_uv{0.25f, 0.75f});
  const auto expected = color_cast<oklab_t>(rgbf_t(0.25f, 0.75f, 0.5f));
  EXPECT_NEAR(ok.l(), expected.l(), 1e-6);
  EXPECT_NEAR(ok.a(), expected.a(), 1e-6);
  EXPECT_NEAR(ok.b(), expected.b(), 1e-6);
}

// ---------------------------------------------------------------------------
// Routing invariant over builtin pairs: the next-hop walk's summed edge costs
// must equal the declared minimal cost. Guards against dist/prev bookkeeping
// bugs in the compile-time Dijkstra.
// ---------------------------------------------------------------------------

namespace detail_ext {

template <typename Cur, typename To>
constexpr std::size_t remaining_cost() {
  if constexpr (std::is_same_v<Cur, To>) {
    return 0;
  } else {
    using Next = graph::next_hop_t<Cur, To>;
    static_assert(!std::is_same_v<Next, void>, "hop walk reached void");
    static_assert(!std::is_same_v<Next, Cur>, "hop walk cycles");
    return graph::minimal_conversion_cost<Cur, Next>() + remaining_cost<Next, To>();
  }
}

template <typename A, typename B>
constexpr bool walk_matches_declared() {
  if constexpr (graph::has_graph_path<A, B>() && graph::endpoints_in_global_graph_v<A, B>) {
    return graph::minimal_conversion_cost<A, B>() == remaining_cost<A, B>();
  } else {
    return true;
  }
}

}  // namespace detail_ext

TEST(ConversionExtensionTest, BuiltinWalkCostMatchesDeclaredCost) {
  static_assert(detail_ext::walk_matches_declared<rgb8_t, oklch_t>());
  static_assert(detail_ext::walk_matches_declared<rgba8_t, cielch_t>());
  static_assert(detail_ext::walk_matches_declared<cmyk8_t, oklab_t>());
  static_assert(detail_ext::walk_matches_declared<hsla_float_t, display_p3af_t>());
  static_assert(detail_ext::walk_matches_declared<rgbf_t, display_p3f_t>());
  static_assert(detail_ext::walk_matches_declared<rgbaf_t, display_p3af_t>());
  static_assert(detail_ext::walk_matches_declared<prophoto_rgbf_t, rec2020_rgbf_t>());
  static_assert(detail_ext::walk_matches_declared<hwb_float_t, cielab_t>());
  static_assert(detail_ext::walk_matches_declared<hsv_float_t, cmyk_float_t>());
  static_assert(detail_ext::walk_matches_declared<adobe_rgbf_t, oklab_t>());
  static_assert(detail_ext::walk_matches_declared<rgb8_t, xyz_t>());
  static_assert(detail_ext::walk_matches_declared<rgbaf_t, oklab_t>());
  SUCCEED();
}

}  // namespace colorcpp::test
