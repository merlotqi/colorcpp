/**
 * @file test_ictcp.cpp
 * @brief Tests for ICtCp as a first-class core colour model.
 *
 * ICtCp (ITU-R BT.2100 PQ / Dolby 2016) previously lived only inside
 * algorithms/delta_e; as a core model it joins the conversion graph so
 * color_cast routes to and from it like any other space. Scale convention:
 * relative Y = 1 maps to the 100 cd/m^2 reference white before the absolute
 * PQ step (white encodes to PQ(100) = 0.5080784).
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/delta_e/itp.hpp>
#include <colorcpp/colorcpp.hpp>
#include <type_traits>

namespace colorcpp::core::test {

using colorcpp::operations::conversion::color_cast;

TEST(IctcpCore, ConstructionAndAccessors) {
  ictcp_t c(0.5f, -0.1f, 0.2f);
  EXPECT_FLOAT_EQ(c.i(), 0.5f);
  EXPECT_FLOAT_EQ(c.ct(), -0.1f);
  EXPECT_FLOAT_EQ(c.cp(), 0.2f);
  ictcp_t d;
  EXPECT_FLOAT_EQ(d.i(), 0.0f);
}

// The core model reproduces the delta_e reference anchors exactly: the
// scale fix (Y = 1 -> 100 cd/m^2) and matrices now live in the conversion
// layer both paths share.
TEST(IctcpCore, ColorCastMatchesDeltaEReferenceAnchors) {
  const xyz_t xyz(0.20654008f, 0.12197225f, 0.05136952f);
  const auto c = color_cast<ictcp_t>(xyz);
  EXPECT_NEAR(c.i(), 0.3175477f, 1e-5f);
  EXPECT_NEAR(c.ct(), -0.0157026f, 1e-5f);
  EXPECT_NEAR(c.cp(), 0.1732075f, 1e-5f);

  const xyz_t white(0.95047f, 1.0f, 1.08883f);
  const auto w = color_cast<ictcp_t>(white);
  EXPECT_NEAR(w.i(), 0.5080784f, 1e-5f);
  EXPECT_NEAR(w.ct(), 0.0f, 1e-5f);
  EXPECT_NEAR(w.cp(), 0.0f, 1e-5f);
}

// The delta_e wrappers are thin aliases over the graph edge — one maths, two
// entry points.
TEST(IctcpCore, DeltaEWrappersMatchColorCast) {
  const xyz_t xyz(0.20654008f, 0.12197225f, 0.05136952f);
  const auto via_cast = color_cast<ictcp_t>(xyz);
  const auto via_delta = colorcpp::algorithms::delta_e::xyz_to_ictcp(xyz);
  EXPECT_FLOAT_EQ(via_cast.i(), via_delta.i());
  EXPECT_FLOAT_EQ(via_cast.ct(), via_delta.ct());
  EXPECT_FLOAT_EQ(via_cast.cp(), via_delta.cp());

  const auto back = color_cast<xyz_t>(via_cast);
  const auto back_delta = colorcpp::algorithms::delta_e::ictcp_to_xyz(via_delta);
  EXPECT_NEAR(back.x(), back_delta.x(), 1e-6f);
  EXPECT_NEAR(back.y(), back_delta.y(), 1e-6f);
  EXPECT_NEAR(back.z(), back_delta.z(), 1e-6f);
}

TEST(IctcpCore, RoundTripThroughGraph) {
  const xyz_t xyz(0.3f, 0.4f, 0.5f);
  const auto back = color_cast<xyz_t>(color_cast<ictcp_t>(xyz));
  EXPECT_NEAR(back.x(), xyz.x(), 1e-4f);
  EXPECT_NEAR(back.y(), xyz.y(), 1e-4f);
  EXPECT_NEAR(back.z(), xyz.z(), 1e-4f);
}

// Graph integration: ICtCp hangs off XYZ like Jzazbz (cost 3 from rgbf,
// next hop through linear RGB and XYZ).
TEST(IctcpCore, GraphRoutesThroughXYZ) {
  static_assert(operations::conversion::graph::minimal_conversion_cost<rgbf_t, ictcp_t>() == 3);
  static_assert(std::is_same_v<operations::conversion::graph::next_hop_t<rgbf_t, ictcp_t>, linear_rgbf_t>);
  // ICtCp joins Jzazbz in the device-independent subset (XYZ-hub routing).
  static_assert(operations::conversion::graph::device_independent<ictcp_t>::value);
  SUCCEED();
}

}  // namespace colorcpp::core::test
