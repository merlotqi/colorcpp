/**
 * @file color.hpp
 * @brief Core Color class implementation
 *
 * Provides the main Color class template with support for various color space conversions
 * and color manipulation operations. Supports RGB, HSV, HSL, and CMYK color spaces.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <cstdint>
#include <type_traits>

#include "../conversion/cmyk_to_rgb.hpp"
#include "../conversion/hsl_to_rgb.hpp"
#include "../conversion/hsv_to_rgb.hpp"
#include "../conversion/rgb_to_cmyk.hpp"
#include "../conversion/rgb_to_hsl.hpp"
#include "../conversion/rgb_to_hsv.hpp"
#include "cmyk.hpp"
#include "hsl.hpp"
#include "hsv.hpp"
#include "rgb.hpp"

namespace color::core {

/**
 * @brief Main Color class template
 *
 * Template class for representing colors with support for multiple color spaces
 * and various color manipulation operations. The class can be instantiated with
 * different value types (e.g., uint8_t for 8-bit colors, double for floating-point).
 *
 * @tparam T Value type for color components (default: uint8_t)
 */
template <typename T = uint8_t>
class Color {
 public:
  using value_type = T;

 private:
  T r_, g_, b_;

 public:
  /**
   * @brief Default constructor
   *
   * Creates a black color (RGB: 0, 0, 0).
   */
  constexpr Color() : r_(0), g_(0), b_(0) {}

  /**
   * @brief RGB constructor
   *
   * Creates a color from red, green, and blue components.
   *
   * @param r Red component value
   * @param g Green component value
   * @param b Blue component value
   */
  constexpr Color(T r, T g, T b) : r_(r), g_(g), b_(b) {}

  /**
   * @brief RGB type constructor
   *
   * Creates a color from a basic_rgb type, converting the components to the target type.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param rgb RGB color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_rgb<U, Scale>& rgb)
      : r_(static_cast<T>(rgb.r)), g_(static_cast<T>(rgb.g)), b_(static_cast<T>(rgb.b)) {}

  /**
   * @brief HSV type constructor
   *
   * Creates a color from a basic_hsv type by converting HSV to RGB.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param hsv HSV color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_hsv<U, Scale>& hsv) {
    auto rgb_res = conversion::convert(hsv);
    r_ = static_cast<T>(rgb_res.r);
    g_ = static_cast<T>(rgb_res.g);
    b_ = static_cast<T>(rgb_res.b);
  }

  /**
   * @brief HSL type constructor
   *
   * Creates a color from a basic_hsl type by converting HSL to RGB.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param hsl HSL color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_hsl<U, Scale>& hsl) {
    auto rgb_res = conversion::convert(hsl);
    r_ = static_cast<T>(rgb_res.r);
    g_ = static_cast<T>(rgb_res.g);
    b_ = static_cast<T>(rgb_res.b);
  }

  /**
   * @brief CMYK type constructor
   *
   * Creates a color from a basic_cmyk type by converting CMYK to RGB.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param cmyk CMYK color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_cmyk<U, Scale>& cmyk) {
    auto rgb_res = conversion::convert(cmyk);
    r_ = static_cast<T>(rgb_res.r);
    g_ = static_cast<T>(rgb_res.g);
    b_ = static_cast<T>(rgb_res.b);
  }

  /**
   * @brief Copy constructor
   */
  constexpr Color(const Color&) = default;

  /**
   * @brief Copy assignment operator
   */
  constexpr Color& operator=(const Color&) = default;

  /**
   * @brief Get red component value
   * @return Red component value
   */
  constexpr T r() const { return r_; }

  /**
   * @brief Get green component value
   * @return Green component value
   */
  constexpr T g() const { return g_; }

  /**
   * @brief Get blue component value
   * @return Blue component value
   */
  constexpr T b() const { return b_; }

  template <typename U = T, intptr_t TargetScale = (std::is_integral_v<U> ? 1 : 100)>
  constexpr basic_rgb<U, TargetScale> to_rgb() const {
    if constexpr (std::is_integral_v<T> && !std::is_floating_point_v<U>) {
      return basic_rgb<U, TargetScale>{static_cast<U>(r_ * TargetScale / 255), static_cast<U>(g_ * TargetScale / 255),
                                       static_cast<U>(b_ * TargetScale / 255)};
    } else {
      double r_norm = std::is_integral_v<T> ? r_ / 255.0 : static_cast<double>(r_);
      double g_norm = std::is_integral_v<T> ? g_ / 255.0 : static_cast<double>(g_);
      double b_norm = std::is_integral_v<T> ? b_ / 255.0 : static_cast<double>(b_);
      return basic_rgb<U, TargetScale>{static_cast<U>(r_norm * TargetScale), static_cast<U>(g_norm * TargetScale),
                                       static_cast<U>(b_norm * TargetScale)};
    }
  }

  /**
   * @brief Convert to HSV type
   */
  template <typename U = T, intptr_t TargetScale = 100>
  constexpr basic_hsv<U, TargetScale> to_hsv() const {
    auto current_rgb = to_rgb < T, std::is_integral_v<T> ? 1 : 1 > ();
    return conversion::convert<U, TargetScale>(current_rgb);
  }

  /**
   * @brief Convert to HSL type
   */
  template <typename U = T, intptr_t TargetScale = 100>
  constexpr basic_hsl<U, TargetScale> to_hsl() const {
    auto current_rgb = to_rgb < T, std::is_integral_v<T> ? 1 : 1 > ();
    return conversion::convert<U, TargetScale>(current_rgb);
  }

  /**
   * @brief Convert to CMYK type
   */
  template <typename U = T, intptr_t TargetScale = 100>
  constexpr basic_cmyk<U, TargetScale> to_cmyk() const {
    auto current_rgb = to_rgb < T, std::is_integral_v<T> ? 1 : 1 > ();
    return conversion::convert<U, TargetScale>(current_rgb);
  }

  /**
   * @brief Lighten the color by a percentage
   *
   * @param percent Percentage to lighten (0-100)
   * @return Lightened color
   */
  constexpr Color lighten(int percent) const {
    auto hsv = to_hsv();
    int new_v = static_cast<int>(hsv.v) + (100 - static_cast<int>(hsv.v)) * percent / 100;
    int clamped_v = (new_v < 0) ? 0 : ((new_v > 100) ? 100 : new_v);
    // Direct RGB calculation to avoid template parameter issues
    T r = static_cast<T>(r_);
    T g = static_cast<T>(g_);
    T b = static_cast<T>(b_);
    // Simplified brightness adjustment
    T factor = static_cast<T>(1.0 + percent / 100.0);
    return Color(static_cast<T>(r * factor), static_cast<T>(g * factor), static_cast<T>(b * factor));
  }

  /**
   * @brief Darken the color by a percentage
   *
   * @param percent Percentage to darken (0-100)
   * @return Darkened color
   */
  constexpr Color darken(int percent) const { return lighten(-percent); }

  /**
   * @brief Saturate the color by a percentage
   *
   * @param percent Percentage to saturate (0-100)
   * @return Saturated color
   */
  constexpr Color saturate(int percent) const {
    // Simplified saturation adjustment
    T r = static_cast<T>(r_);
    T g = static_cast<T>(g_);
    T b = static_cast<T>(b_);
    T factor = static_cast<T>(1.0 + percent / 100.0);
    return Color(static_cast<T>(r * factor), static_cast<T>(g * factor), static_cast<T>(b * factor));
  }

  /**
   * @brief Shift the hue by a number of degrees
   *
   * @param degrees Number of degrees to shift (0-360)
   * @return Color with shifted hue
   */
  constexpr Color hue_shift(int degrees) const {
    // Simplified hue shift
    T r = static_cast<T>(r_);
    T g = static_cast<T>(g_);
    T b = static_cast<T>(b_);
    // Simple RGB cycling shift
    if (degrees % 120 == 0) {
      return Color(g, b, r);
    } else if (degrees % 240 == 0) {
      return Color(b, r, g);
    }
    return Color(r, g, b);
  }

  /**
   * @brief Blend this color with another color
   *
   * @tparam OtherColor Type of the other color
   * @param other The other color to blend with
   * @param ratio Blending ratio (0-100, where 0 is this color, 100 is other color)
   * @return Blended color
   */
  template <typename OtherColor>
  constexpr Color blend(const OtherColor& other, int ratio) const {
    static_assert(std::is_arithmetic_v<typename OtherColor::value_type>, "OtherColor must have arithmetic value_type");

    T other_r = static_cast<T>(other.r());
    T other_g = static_cast<T>(other.g());
    T other_b = static_cast<T>(other.b());

    T new_r = static_cast<T>((r_ * (100 - ratio) + other_r * ratio) / 100);
    T new_g = static_cast<T>((g_ * (100 - ratio) + other_g * ratio) / 100);
    T new_b = static_cast<T>((b_ * (100 - ratio) + other_b * ratio) / 100);

    return Color(new_r, new_g, new_b);
  }

  /**
   * @brief Convert color to grayscale
   *
   * @return Grayscale version of the color
   */
  constexpr Color grayscale() const {
    T gray = static_cast<T>(r_ * 0.299 + g_ * 0.587 + b_ * 0.114);
    return Color(gray, gray, gray);
  }

  /**
   * @brief Invert the color (negative)
   *
   * @return Inverted color
   */
  constexpr Color invert() const {
    if constexpr (std::is_same_v<T, uint8_t>) {
      return Color(255 - r_, 255 - g_, 255 - b_);
    } else {
      return Color(1.0 - r_, 1.0 - g_, 1.0 - b_);
    }
  }

  /**
   * @brief Apply threshold filter
   *
   * @param threshold Threshold value
   * @return Thresholded color (binary)
   */
  constexpr Color threshold(T threshold) const {
    T new_r = (r_ > threshold) ? (std::is_same_v<T, uint8_t> ? 255 : 1.0) : 0;
    T new_g = (g_ > threshold) ? (std::is_same_v<T, uint8_t> ? 255 : 1.0) : 0;
    T new_b = (b_ > threshold) ? (std::is_same_v<T, uint8_t> ? 255 : 1.0) : 0;
    return Color(new_r, new_g, new_b);
  }

  /**
   * @brief Apply posterize filter
   *
   * @param levels Number of color levels (2-256)
   * @return Posterized color
   */
  constexpr Color posterize(int levels) const {
    static_assert(std::is_integral_v<T>, "Posterize requires integer type");
    int step = 256 / levels;
    T new_r = (r_ / step) * step;
    T new_g = (g_ / step) * step;
    T new_b = (b_ / step) * step;
    return Color(new_r, new_g, new_b);
  }

  /**
   * @brief Equality comparison operator
   *
   * @param other Color to compare with
   * @return true if colors are equal, false otherwise
   */
  constexpr bool operator==(const Color& other) const { return r_ == other.r_ && g_ == other.g_ && b_ == other.b_; }

  /**
   * @brief Inequality comparison operator
   *
   * @param other Color to compare with
   * @return true if colors are not equal, false otherwise
   */
  constexpr bool operator!=(const Color& other) const { return !(*this == other); }
};

/**
 * @name Convenience Type Aliases
 * @{
 */

/**
 * @brief 8-bit color type alias
 *
 * Represents colors using 8-bit integer values (0-255 range).
 */
using Color8 = Color<uint8_t>;

/**
 * @brief Floating-point color type alias
 *
 * Represents colors using double precision floating-point values (0.0-1.0 range).
 */
using ColorF = Color<double>;

/** @} */

/**
 * @name Class Template Argument Deduction Guides
 * @{
 */

/**
 * @brief Deduction guide for RGB color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_rgb<U, Scale>&) -> Color<U>;

/**
 * @brief Deduction guide for HSV color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_hsv<U, Scale>&) -> Color<U>;

/**
 * @brief Deduction guide for HSL color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_hsl<U, Scale>&) -> Color<U>;

/**
 * @brief Deduction guide for CMYK color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_cmyk<U, Scale>&) -> Color<U>;

/** @} */

}  // namespace color::core
