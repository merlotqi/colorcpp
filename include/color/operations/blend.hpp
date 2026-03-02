/**
 * @file blend.hpp
 * @brief Color blending operations
 *
 * Provides compile-time color blending operations for mixing two colors
 * with specified ratios. Includes common blending modes like overlay,
 * screen, and multiply.
 *
 * @author Merlot.Qi
 * 
 */

#pragma once

#include "../core/rgb.hpp"

namespace color::operations {

/**
 * @brief Color blending operation
 *
 * Template struct for blending two colors with a specified ratio.
 * The ratio determines how much of each color contributes to the result.
 *
 * @tparam Color1 First color type
 * @tparam Color2 Second color type
 * @tparam Ratio Blending ratio (0-100, where 0 is Color1, 100 is Color2)
 */
template <typename Color1, typename Color2, int Ratio>
struct blend {
  using color1_type = Color1;
  using color2_type = Color2;
  static_assert(Ratio >= 0 && Ratio <= 100, "Ratio must be 0-100");

  /**
   * @brief Blend a single color channel
   *
   * @tparam T Channel value type
   * @param c1 First color channel value
   * @param c2 Second color channel value
   * @return Blended channel value
   */
  template <typename T>
  static constexpr T blend_channel(T c1, T c2) {
    return static_cast<T>((c1 * (100 - Ratio) + c2 * Ratio) / 100);
  }

  /// @brief Blended red channel value
  static constexpr auto r = blend_channel(Color1::r, Color2::r);
  /// @brief Blended green channel value
  static constexpr auto g = blend_channel(Color1::g, Color2::g);
  /// @brief Blended blue channel value
  static constexpr auto b = blend_channel(Color1::b, Color2::b);

  /// @brief Resulting RGB type after blending
  using type = std::conditional_t<std::is_same_v<typename Color1::value_type, uint8_t>, core::rgb8<r, g, b>,
                                  core::rgb8<r, g, b> >;
};

/**
 * @brief Type alias for color blending operation result
 *
 * @tparam Color1 First color type
 * @tparam Color2 Second color type
 * @tparam Ratio Blending ratio (0-100)
 * @return RGB color type representing the blended color
 */
template <typename Color1, typename Color2, int Ratio>
using blend_t = typename blend<Color1, Color2, Ratio>::type;

/**
 * @brief Common color blending operations namespace
 *
 * Provides predefined blending operations with commonly used ratios.
 */
namespace blend_ops {
/**
 * @brief Overlay blending operation (50% ratio)
 *
 * @tparam Color1 First color type
 * @tparam Color2 Second color type
 * @return RGB color type with equal contribution from both colors
 */
template <typename Color1, typename Color2>
using overlay = blend_t<Color1, Color2, 50>;

/**
 * @brief Screen blending operation (75% ratio)
 *
 * @tparam Color1 First color type
 * @tparam Color2 Second color type
 * @return RGB color type with 75% contribution from second color
 */
template <typename Color1, typename Color2>
using screen = blend_t<Color1, Color2, 75>;

/**
 * @brief Multiply blending operation (25% ratio)
 *
 * @tparam Color1 First color type
 * @tparam Color2 Second color type
 * @return RGB color type with 25% contribution from second color
 */
template <typename Color1, typename Color2>
using multiply = blend_t<Color1, Color2, 25>;
}  // namespace blend_ops

}  // namespace color::operations
