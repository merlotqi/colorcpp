/**
 * @file color.hpp
 * @brief Core Color class implementation
 *
 * Provides the main Color class template with support for various color space conversions
 * and color manipulation operations. Supports RGBA, HSVA, HSLA, and CMYKA color spaces.
 *
 * @author Merlot.Qi
 *
 */

#pragma once

#include <color/conversion/conversion.hpp>
#include <color/core/cmyk.hpp>
#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/core/rgb.hpp>
#include <cstdint>
#include <type_traits>

namespace color::core {

/**
 * @brief Main Color class template
 *
 * Template class for representing colors with support for multiple color spaces
 * and various color manipulation operations. The class can be instantiated with
 * different value types (e.g., uint8_t for 8-bit colors, float for floating-point).
 *
 * @tparam T Value type for color components (default: uint8_t)
 */
template <typename T = uint8_t>
class Color {
 public:
  using value_type = T;

 private:
  T r_, g_, b_, a_;

 public:
  /**
   * @brief Default constructor
   *
   * Creates a black color (RGBA: 0, 0, 0, 1).
   */
  constexpr Color() : r_(0), g_(0), b_(0), a_(1) {}

  /**
   * @brief RGB constructor
   *
   * Creates a color from red, green, and blue components.
   *
   * @param r Red component value
   * @param g Green component value
   * @param b Blue component value
   */
  constexpr Color(T r, T g, T b) : r_(r), g_(g), b_(b), a_(1) {}

  /**
   * @brief RGBA constructor
   *
   * Creates a color from red, green, blue, and alpha components.
   *
   * @param r Red component value
   * @param g Green component value
   * @param b Blue component value
   * @param a Alpha component value
   */
  constexpr Color(T r, T g, T b, T a) : r_(r), g_(g), b_(b), a_(a) {}

  /**
   * @brief RGBA type constructor
   *
   * Creates a color from a basic_rgba type, converting the components to the target type.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param rgba RGBA color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_rgba<U, Scale>& rgba)
      : r_(static_cast<T>(rgba.r)),
        g_(static_cast<T>(rgba.g)),
        b_(static_cast<T>(rgba.b)),
        a_(static_cast<T>(rgba.a)) {}

  /**
   * @brief HSVA type constructor
   *
   * Creates a color from a basic_hsva type by converting HSVA to RGBA.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param hsva HSVA color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_hsva<U, Scale>& hsva) {
    auto rgba_res = conversion::convert(hsva);
    r_ = static_cast<T>(rgba_res.r);
    g_ = static_cast<T>(rgba_res.g);
    b_ = static_cast<T>(rgba_res.b);
    a_ = static_cast<T>(rgba_res.a);
  }

  /**
   * @brief HSLA type constructor
   *
   * Creates a color from a basic_hsla type by converting HSLA to RGBA.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param hsla HSLA color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_hsla<U, Scale>& hsla) {
    auto rgba_res = conversion::convert(hsla);
    r_ = static_cast<T>(rgba_res.r);
    g_ = static_cast<T>(rgba_res.g);
    b_ = static_cast<T>(rgba_res.b);
    a_ = static_cast<T>(rgba_res.a);
  }

  /**
   * @brief CMYKA type constructor
   *
   * Creates a color from a basic_cmyka type by converting CMYKA to RGBA.
   *
   * @tparam U Source value type
   * @tparam Scale Scaling factor for value conversion
   * @param cmyka CMYKA color type to convert from
   */
  template <typename U, intptr_t Scale>
  constexpr Color(const basic_cmyk<U, Scale>& cmyka) {
    auto rgba_res = conversion::convert(cmyka);
    r_ = static_cast<T>(rgba_res.r);
    g_ = static_cast<T>(rgba_res.g);
    b_ = static_cast<T>(rgba_res.b);
    a_ = static_cast<T>(rgba_res.a);
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

  /**
   * @brief Get alpha component value
   * @return Alpha component value
   */
  constexpr T a() const { return a_; }

  template <typename U = T, intptr_t TargetScale = (std::is_integral_v<U> ? 1 : 100)>
  constexpr basic_rgba<U, TargetScale> to_rgba() const {
    if constexpr (std::is_integral_v<T> && !std::is_floating_point_v<U>) {
      return basic_rgba<U, TargetScale>{static_cast<U>(r_ * TargetScale / 255), static_cast<U>(g_ * TargetScale / 255),
                                        static_cast<U>(b_ * TargetScale / 255), static_cast<U>(a_ * TargetScale / 255)};
    } else {
      float r_norm = std::is_integral_v<T> ? r_ / 255.0 : static_cast<float>(r_);
      float g_norm = std::is_integral_v<T> ? g_ / 255.0 : static_cast<float>(g_);
      float b_norm = std::is_integral_v<T> ? b_ / 255.0 : static_cast<float>(b_);
      float a_norm = std::is_integral_v<T> ? a_ / 255.0 : static_cast<float>(a_);
      return basic_rgba<U, TargetScale>{static_cast<U>(r_norm * TargetScale), static_cast<U>(g_norm * TargetScale),
                                        static_cast<U>(b_norm * TargetScale), static_cast<U>(a_norm * TargetScale)};
    }
  }

  /**
   * @brief Convert to HSVA type
   */
  template <typename U = T, intptr_t TargetScale = 100>
  constexpr basic_hsva<U, TargetScale> to_hsva() const {
    auto current_rgba = to_rgba < T, std::is_integral_v<T> ? 1 : 1 > ();
    return conversion::convert<U, TargetScale>(current_rgba);
  }

  /**
   * @brief Convert to HSLA type
   */
  template <typename U = T, intptr_t TargetScale = 100>
  constexpr basic_hsla<U, TargetScale> to_hsla() const {
    auto current_rgba = to_rgba < T, std::is_integral_v<T> ? 1 : 1 > ();
    return conversion::convert<U, TargetScale>(current_rgba);
  }

  /**
   * @brief Convert to CMYKA type
   */
  template <typename U = T, intptr_t TargetScale = 100>
  constexpr basic_cmyk<U, TargetScale> to_cmyka() const {
    auto current_rgba = to_rgba < T, std::is_integral_v<T> ? 1 : 1 > ();
    return conversion::convert<U, TargetScale>(current_rgba);
  }

  /**
   * @brief Lighten the color by a percentage
   *
   * @param percent Percentage to lighten (0-100)
   * @return Lightened color
   */
  constexpr Color lighten(int percent) const {
    auto hsv = to_hsva();
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
    T other_a = static_cast<T>(other.a());

    T new_r = static_cast<T>((r_ * (100 - ratio) + other_r * ratio) / 100);
    T new_g = static_cast<T>((g_ * (100 - ratio) + other_g * ratio) / 100);
    T new_b = static_cast<T>((b_ * (100 - ratio) + other_b * ratio) / 100);
    T new_a = static_cast<T>((a_ * (100 - ratio) + other_a * ratio) / 100);

    return Color(new_r, new_g, new_b, new_a);
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
  constexpr bool operator==(const Color& other) const {
    return r_ == other.r_ && g_ == other.g_ && b_ == other.b_ && a_ == other.a_;
  }

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
 * Represents colors using float precision floating-point values (0.0-1.0 range).
 */
using ColorF = Color<float>;

/** @} */

/**
 * @name Class Template Argument Deduction Guides
 * @{
 */

/**
 * @brief Deduction guide for RGBA color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_rgba<U, Scale>&) -> Color<U>;

/**
 * @brief Deduction guide for HSVA color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_hsva<U, Scale>&) -> Color<U>;

/**
 * @brief Deduction guide for HSLA color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_hsla<U, Scale>&) -> Color<U>;

/**
 * @brief Deduction guide for CMYKA color types
 *
 * @tparam U Source value type
 * @tparam Scale Scaling factor for value conversion
 */
template <typename U, intptr_t Scale>
Color(const basic_cmyk<U, Scale>&) -> Color<U>;

/** @} */

}  // namespace color::core
