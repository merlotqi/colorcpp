/**
 * @file test_reference_vectors.cpp
 * @brief Data-driven reference-vector tests against W3C CSS Color 4 worked
 *        examples. Every ordered pair of color spaces in a fixture case is
 *        converted via color_cast and compared with the spec's numbers.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <colorcpp/colorcpp.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <variant>

namespace colorcpp::reference_test {

using namespace core;
namespace conversion = operations::conversion;
namespace gamut = colorcpp::algorithms::gamut;

namespace {

using AnyColor = std::variant<rgbf_t, linear_rgbf_t, display_p3f_t, linear_display_p3f_t, adobe_rgbf_t, prophoto_rgbf_t,
                              rec2020_rgbf_t, xyz_t, oklab_t, oklch_t>;

using Vec3 = std::array<float, 3>;

template <typename T>
AnyColor make_color(const Vec3& v) {
  return T{v[0], v[1], v[2]};
}

AnyColor make_color(const std::string& space, const Vec3& v) {
  if (space == "srgb") return make_color<rgbf_t>(v);
  if (space == "srgb_linear") return make_color<linear_rgbf_t>(v);
  if (space == "display_p3") return make_color<display_p3f_t>(v);
  if (space == "display_p3_linear") return make_color<linear_display_p3f_t>(v);
  if (space == "adobe_rgb") return make_color<adobe_rgbf_t>(v);
  if (space == "prophoto_rgb") return make_color<prophoto_rgbf_t>(v);
  if (space == "rec2020") return make_color<rec2020_rgbf_t>(v);
  if (space == "xyz_d65") return make_color<xyz_t>(v);
  if (space == "oklab") return make_color<oklab_t>(v);
  if (space == "oklch") return make_color<oklch_t>(v);
  throw std::runtime_error("unknown space: " + space);
}

template <typename To>
Vec3 cast_extract(const AnyColor& src_any) {
  return std::visit(
      [](const auto& src) -> Vec3 {
        const auto dst = conversion::color_cast<To>(src);
        return {static_cast<float>(dst.template get_index<0>()), static_cast<float>(dst.template get_index<1>()),
                static_cast<float>(dst.template get_index<2>())};
      },
      src_any);
}

Vec3 cast_to(const std::string& to_space, const AnyColor& src) {
  if (to_space == "srgb") return cast_extract<rgbf_t>(src);
  if (to_space == "srgb_linear") return cast_extract<linear_rgbf_t>(src);
  if (to_space == "display_p3") return cast_extract<display_p3f_t>(src);
  if (to_space == "display_p3_linear") return cast_extract<linear_display_p3f_t>(src);
  if (to_space == "adobe_rgb") return cast_extract<adobe_rgbf_t>(src);
  if (to_space == "prophoto_rgb") return cast_extract<prophoto_rgbf_t>(src);
  if (to_space == "rec2020") return cast_extract<rec2020_rgbf_t>(src);
  if (to_space == "xyz_d65") return cast_extract<xyz_t>(src);
  if (to_space == "oklab") return cast_extract<oklab_t>(src);
  if (to_space == "oklch") return cast_extract<oklch_t>(src);
  throw std::runtime_error("unknown space: " + to_space);
}

// Cylindrical spaces carry hue in degrees: compare with wraparound.
bool is_hue_space(const std::string& space) { return space == "oklch"; }

float channel_diff(const std::string& to_space, int i, float actual, float expected) {
  if (is_hue_space(to_space) && i == 2) {
    const float d = std::abs(actual - expected);
    return std::min(d, 360.0f - d);
  }
  return std::abs(actual - expected);
}

nlohmann::json load_vectors() {
  const std::string path = std::string(COLORCPP_REFERENCE_DATA_DIR) + "/css_color4_vectors.json";
  std::ifstream in(path);
  if (!in) throw std::runtime_error("missing fixture: " + path);
  nlohmann::json j;
  in >> j;
  return j;
}

}  // namespace

TEST(CssColor4Reference, ConversionVectors) {
  const auto j = load_vectors();
  const float default_tol = j.value("default_tolerance", 0.0015f);

  for (const auto& case_j : j.at("cases")) {
    const std::string name = case_j.at("name").get<std::string>();
    const float tol = case_j.value("tolerance", default_tol);
    const auto& spaces = case_j.at("spaces");

    for (auto it_from = spaces.begin(); it_from != spaces.end(); ++it_from) {
      for (auto it_to = spaces.begin(); it_to != spaces.end(); ++it_to) {
        if (it_from.key() == it_to.key()) continue;
        const auto src = make_color(it_from.key(), it_from.value().get<Vec3>());
        const auto expected = it_to.value().get<Vec3>();
        const auto actual = cast_to(it_to.key(), src);

        for (int i = 0; i < 3; ++i) {
          // Spec prints hue to 0.1 deg (sometimes integers); allow 0.25 deg for
          // the hue channel regardless of the case's value tolerance.
          float channel_tol = tol;
          if (is_hue_space(it_to.key()) && i == 2) channel_tol = std::max(tol, 0.25f);
          const float diff = channel_diff(it_to.key(), i, actual[i], expected[i]);
          EXPECT_LE(diff, channel_tol) << "case=" << name << " route=" << it_from.key() << "->" << it_to.key()
                                       << " channel=" << i << " actual=" << actual[i] << " expected=" << expected[i];
        }
      }
    }
  }
}

TEST(CssColor4Reference, GamutMapP3YellowToSrgb) {
  // CSS Color 4 §14.2 worked example: chroma reduction of display-p3 yellow.
  // Input: oklch(0.96476 0.24503 110.23). After chroma reduction:
  // oklch(0.96476 0.21094 110.23) = srgb(0.99116 0.99733 0.00001).
  const auto mapped = gamut::gamut_clip(display_p3f_t{1.0f, 1.0f, 0.0f}, gamut::clip_method::chroma_reduce);
  const auto oklch = conversion::color_cast<oklch_t>(mapped);
  EXPECT_NEAR(static_cast<float>(oklch.get_index<0>()), 0.96476f, 0.002f);  // L
  EXPECT_NEAR(static_cast<float>(oklch.get_index<1>()), 0.21094f, 0.002f);  // C
  EXPECT_NEAR(static_cast<float>(oklch.get_index<2>()), 110.23f, 0.5f);     // H (deg)

  const auto srgb = conversion::color_cast<rgbf_t>(mapped);
  EXPECT_NEAR(static_cast<float>(srgb.r()), 0.99116f, 0.002f);
  EXPECT_NEAR(static_cast<float>(srgb.g()), 0.99733f, 0.002f);
  EXPECT_NEAR(static_cast<float>(srgb.b()), 0.00001f, 0.002f);
}

}  // namespace colorcpp::reference_test
