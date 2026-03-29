/**
 * @file harmony.hpp
 * @brief Color harmony generators (complementary, analogous, triadic, etc.).
 */

#pragma once

#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/details.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <colorcpp/operations/palette/scales.hpp>

namespace colorcpp::operations::palette {

/** @brief Static helpers that return @ref palette_set for harmony schemes in HSL hue. */
struct generate {
  /**
   * @brief Generate complementary color palette (180° rotation).
   * @param base Base color.
   * @return Palette with base and complementary color.
   */
  template <typename Color>
  static auto complementary(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float next_h = details::rotate_hue(h.template get_index<0>(), 180.0f);
    auto comp_h = details::modify_hue(h, next_h);

    return palette_set<Color>{base, color_cast<Color>(comp_h)};
  }

  /**
   * @brief Generate analogous color palette (±angle rotation).
   * @param base Base color.
   * @param angle Rotation angle (default: 30°).
   * @return Palette with analogous colors.
   * @note If angle is 0 or very small (< 0.01), returns palette with only the base color.
   */
  template <typename Color>
  static auto analogous(const Color& base, float angle = 30.0f) {
    using namespace conversion;

    // Handle edge case: angle ~ 0 returns single color
    if (std::abs(angle) < 0.01f) {
      return palette_set<Color>{base};
    }

    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), -angle);
    float h2 = details::rotate_hue(h.template get_index<0>(), angle);

    return palette_set<Color>{color_cast<Color>(details::modify_hue(h, h1)), base,
                              color_cast<Color>(details::modify_hue(h, h2))};
  }

  /**
   * @brief Generate triadic color palette (120° rotations).
   * @param base Base color.
   * @return Palette with triadic colors.
   */
  template <typename Color>
  static auto triadic(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 120.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 240.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2))};
  }

  /**
   * @brief Generate split-complementary color palette (150° and 210° rotations).
   * @param base Base color.
   * @return Palette with split-complementary colors.
   */
  template <typename Color>
  static auto split_complementary(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 150.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 210.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2))};
  }

  /**
   * @brief Generate tetradic (rectangular) color palette.
   * @param base Base color.
   * @return Palette with tetradic colors.
   */
  template <typename Color>
  static auto tetradic(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 60.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 180.0f);
    float h3 = details::rotate_hue(h.template get_index<0>(), 240.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2)),
                              color_cast<Color>(details::modify_hue(h, h3))};
  }

  /**
   * @brief Generate square color palette (90° rotations).
   * @param base Base color.
   * @return Palette with square colors.
   */
  template <typename Color>
  static auto square(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 90.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 180.0f);
    float h3 = details::rotate_hue(h.template get_index<0>(), 270.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2)),
                              color_cast<Color>(details::modify_hue(h, h3))};
  }

  /**
   * @brief Generate monochromatic color palette (varying lightness).
   * @param base Base color.
   * @param count Number of colors (default: 5).
   * @param min_lightness Minimum lightness value (default: 0.2).
   * @param max_lightness Maximum lightness value (default: 0.9).
   * @return Palette with monochromatic colors.
   */
  template <typename Color>
  static auto monochromatic(const Color& base, size_t count = 5, float min_lightness = 0.2f,
                            float max_lightness = 0.9f) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    // Clamp lightness values to valid range [0, 1]
    min_lightness = std::max(0.0f, std::min(1.0f, min_lightness));
    max_lightness = std::max(0.0f, std::min(1.0f, max_lightness));

    // Ensure min < max
    if (min_lightness > max_lightness) {
      std::swap(min_lightness, max_lightness);
    }

    core::hsla_float_t low{h.template get_index<0>(), h.template get_index<1>(), min_lightness,
                           h.template get_index<3>()};
    core::hsla_float_t high{h.template get_index<0>(), h.template get_index<1>(), max_lightness,
                            h.template get_index<3>()};

    return visual_scale(color_cast<Color>(low), color_cast<Color>(high), count);
  }
};

}  // namespace colorcpp::operations::palette
