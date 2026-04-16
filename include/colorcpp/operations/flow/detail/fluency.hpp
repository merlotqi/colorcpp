/**
 * @file fluency.hpp
 * @brief Fluent interface helper utilities for the colorflow module.
 *
 * Provides base class and helpers for building chainable color operations.
 */

#pragma once

#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::operations::flow::detail {

/**
 * @brief Convert a core color to HSLA float representation for manipulation.
 * @tparam Color Input color type.
 * @param c Input color.
 * @return HSLA float color.
 */
template <typename Color>
inline core::hsla_float_t to_hsla(const Color& c) {
  return operations::conversion::color_cast<core::hsla_float_t>(c);
}

/**
 * @brief Convert HSLA float back to target color type.
 * @tparam Color Target color type.
 * @param hsla HSLA float color.
 * @return Target color type.
 */
template <typename Color>
inline Color from_hsla(const core::hsla_float_t& hsla) {
  return operations::conversion::color_cast<Color>(hsla);
}

/**
 * @brief Rotate hue angle with wrap-around at 360.
 * @param hue Current hue (0-360).
 * @param degrees Degrees to rotate (negative allowed).
 * @return Wrapped hue value.
 */
inline float rotate_hue_angle(float hue, float degrees) {
  float result = std::fmod(hue + degrees, 360.0f);
  if (result < 0.0f) result += 360.0f;
  return result;
}

/**
 * @brief Convert a hex color value to rgba8_t.
 * @param hex 32-bit RGBA value (0xRRGGBBAA).
 * @return rgba8_t color.
 */
inline core::rgba8_t from_hex(uint32_t hex) {
  uint8_t r = static_cast<uint8_t>((hex >> 24) & 0xFF);
  uint8_t g = static_cast<uint8_t>((hex >> 16) & 0xFF);
  uint8_t b = static_cast<uint8_t>((hex >> 8) & 0xFF);
  uint8_t a = static_cast<uint8_t>(hex & 0xFF);
  return core::rgba8_t{r, g, b, a};
}

/**
 * @brief Convert rgba8_t to a hex string (without alpha).
 * @param color Input color.
 * @return Hex string like "#ff6347".
 */
inline std::string to_hex_string(const core::rgba8_t& color) {
  char buf[8];
  std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", color.r(), color.g(), color.b());
  return std::string(buf);
}

/**
 * @brief Convert rgba8_t to a hex string with alpha.
 * @param color Input color.
 * @return Hex string like "#ff6347ff".
 */
inline std::string to_hex_string_rgba(const core::rgba8_t& color) {
  char buf[10];
  std::snprintf(buf, sizeof(buf), "#%02x%02x%02x%02x", color.r(), color.g(), color.b(), color.a());
  return std::string(buf);
}

}  // namespace colorcpp::operations::flow::detail
