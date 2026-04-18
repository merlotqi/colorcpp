/**
 * @file schemes.hpp
 * @brief Palette generation from formal harmony schemes.
 */

#pragma once

#include <algorithm>
#include <colorcpp/algorithms/harmony/rules.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/details.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/scales.hpp>

namespace colorcpp::operations::palette::schemes {

namespace detail {

template <typename Color>
palette_set<Color> from_offsets(const Color& base, const std::vector<float>& offsets) {
  using namespace conversion;

  if (offsets.empty()) {
    return {};
  }

  auto hsl = color_cast<core::hsla_float_t>(base);
  palette_set<Color> result;
  for (float offset : offsets) {
    float target_hue = details::rotate_hue(hsl.template get_index<0>(), offset);
    result.add(color_cast<Color>(details::modify_hue(hsl, target_hue)));
  }
  return result;
}

}  // namespace detail

/**
 * @brief Generate a palette from a formal harmony scheme.
 */
template <typename Color>
palette_set<Color> make(const Color& base, algorithms::harmony::harmony_scheme scheme, size_t count = 0) {
  const auto rule = algorithms::harmony::rule_for(scheme, count);
  return detail::from_offsets(base, rule.generation_offsets);
}

/**
 * @brief Generate a complementary palette.
 */
template <typename Color>
palette_set<Color> complementary(const Color& base) {
  return make(base, algorithms::harmony::harmony_scheme::complementary);
}

/**
 * @brief Generate an analogous palette around the base hue.
 */
template <typename Color>
palette_set<Color> analogous(const Color& base, float angle = 30.0f) {
  if (std::abs(angle) < 0.01f) {
    return palette_set<Color>{base};
  }
  return detail::from_offsets(base, {-angle, 0.0f, angle});
}

/**
 * @brief Generate a triadic palette.
 */
template <typename Color>
palette_set<Color> triadic(const Color& base) {
  return make(base, algorithms::harmony::harmony_scheme::triadic);
}

/**
 * @brief Generate a split-complementary palette.
 */
template <typename Color>
palette_set<Color> split_complementary(const Color& base) {
  return make(base, algorithms::harmony::harmony_scheme::split_complementary);
}

/**
 * @brief Generate a tetradic palette.
 */
template <typename Color>
palette_set<Color> tetradic(const Color& base) {
  return make(base, algorithms::harmony::harmony_scheme::tetradic);
}

/**
 * @brief Generate a square palette.
 */
template <typename Color>
palette_set<Color> square(const Color& base) {
  return make(base, algorithms::harmony::harmony_scheme::square);
}

/**
 * @brief Generate a monochromatic palette by varying lightness.
 */
template <typename Color>
palette_set<Color> monochromatic(const Color& base, size_t count = 5, float min_lightness = 0.2f,
                                 float max_lightness = 0.9f) {
  using namespace conversion;
  auto h = color_cast<core::hsla_float_t>(base);

  min_lightness = std::max(0.0f, std::min(1.0f, min_lightness));
  max_lightness = std::max(0.0f, std::min(1.0f, max_lightness));

  if (min_lightness > max_lightness) {
    std::swap(min_lightness, max_lightness);
  }

  core::hsla_float_t low{h.template get_index<0>(), h.template get_index<1>(), min_lightness,
                         h.template get_index<3>()};
  core::hsla_float_t high{h.template get_index<0>(), h.template get_index<1>(), max_lightness,
                          h.template get_index<3>()};

  return visual_scale(color_cast<Color>(low), color_cast<Color>(high), count);
}

}  // namespace colorcpp::operations::palette::schemes