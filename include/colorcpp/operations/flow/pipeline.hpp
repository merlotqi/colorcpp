/**
 * @file pipeline.hpp
 * @brief Fluent API pipeline for chainable color operations.
 *
 * Provides a builder-style interface for common color manipulations:
 *
 * @code
 * auto result = flow::make(0xff6347)
 *     .lighten(20)
 *     .saturate(10)
 *     .rotate_hue(30)
 *     .to_rgba8();
 * @endcode
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/flow/detail/fluency.hpp>
#include <colorcpp/operations/interpolate.hpp>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace colorcpp::operations::flow {

/**
 * @class pipeline
 * @brief Fluent API wrapper for chainable color operations.
 *
 * Encapsulates a color value and provides method chaining for common
 * transformations. All operations return a new pipeline instance,
 * preserving immutability.
 */
class pipeline {
  core::rgba8_t color_;

 public:
  explicit pipeline(core::rgba8_t c) : color_(c) {}
  core::rgba8_t color() const { return color_; }

  pipeline lighten(float percent) const {
    auto hsla = detail::to_hsla(color_);
    hsla.l() = std::min(hsla.l() + percent / 100.0f * (1.0f - hsla.l()), 1.0f);
    return pipeline(detail::from_hsla<core::rgba8_t>(hsla));
  }

  pipeline darken(float percent) const {
    auto hsla = detail::to_hsla(color_);
    hsla.l() = std::max(hsla.l() - percent / 100.0f * hsla.l(), 0.0f);
    return pipeline(detail::from_hsla<core::rgba8_t>(hsla));
  }

  pipeline saturate(float percent) const {
    auto hsla = detail::to_hsla(color_);
    hsla.s() = std::clamp(hsla.s() + percent / 100.0f * (1.0f - hsla.s()), 0.0f, 1.0f);
    return pipeline(detail::from_hsla<core::rgba8_t>(hsla));
  }

  pipeline desaturate(float percent) const {
    auto hsla = detail::to_hsla(color_);
    hsla.s() = std::max(hsla.s() - percent / 100.0f * hsla.s(), 0.0f);
    return pipeline(detail::from_hsla<core::rgba8_t>(hsla));
  }

  pipeline rotate_hue(float degrees) const {
    auto hsla = detail::to_hsla(color_);
    hsla.h() = detail::rotate_hue_angle(hsla.h(), degrees);
    return pipeline(detail::from_hsla<core::rgba8_t>(hsla));
  }

  pipeline set_hue(float degrees) const {
    auto hsla = detail::to_hsla(color_);
    hsla.h() = std::clamp(degrees, 0.0f, 360.0f);
    return pipeline(detail::from_hsla<core::rgba8_t>(hsla));
  }

  pipeline alpha(uint8_t a) const { return pipeline(core::rgba8_t{color_.r(), color_.g(), color_.b(), a}); }

  pipeline fade(float percent) const {
    uint8_t a = static_cast<uint8_t>(std::clamp(percent, 0.0f, 100.0f) / 100.0f * 255);
    return pipeline(core::rgba8_t{color_.r(), color_.g(), color_.b(), a});
  }

  core::rgba8_t to_rgba8() const { return color_; }
  core::rgbaf_t to_rgbaf() const { return operations::conversion::color_cast<core::rgbaf_t>(color_); }
  std::string to_hex() const { return detail::to_hex_string(color_); }
  std::string to_hex_rgba() const { return detail::to_hex_string_rgba(color_); }
  std::string to_css() const { return to_hex(); }
  uint32_t to_rgba32() const {
    return (static_cast<uint32_t>(color_.r()) << 24) | (static_cast<uint32_t>(color_.g()) << 16) |
           (static_cast<uint32_t>(color_.b()) << 8) | static_cast<uint32_t>(color_.a());
  }

  std::vector<core::rgba8_t> to_palette(int count) const {
    if (count <= 0) return {};
    if (count == 1) return {color_};
    std::vector<core::rgba8_t> result;
    result.reserve(static_cast<size_t>(count));
    result.push_back(color_);
    if (count >= 2) result.push_back(rotate_hue(180).color_);
    if (count >= 3) result.push_back(rotate_hue(30).color_);
    if (count >= 4) result.push_back(rotate_hue(120).color_);
    if (count >= 5) result.push_back(rotate_hue(210).color_);
    if (count >= 6) result.push_back(lighten(20).color_);
    return result;
  }

  pipeline blend(const pipeline& other, float ratio = 0.5f) const {
    return pipeline(interpolate::lerp(color_, other.color_, std::clamp(ratio, 0.0f, 1.0f)));
  }

  pipeline blend(const core::rgba8_t& other, float ratio = 0.5f) const {
    return pipeline(interpolate::lerp(color_, other, std::clamp(ratio, 0.0f, 1.0f)));
  }
};

inline pipeline make(const core::rgba8_t& c) { return pipeline(c); }

inline pipeline make(uint32_t hex) {
  uint8_t r = static_cast<uint8_t>((hex >> 24) & 0xFF);
  uint8_t g = static_cast<uint8_t>((hex >> 16) & 0xFF);
  uint8_t b = static_cast<uint8_t>((hex >> 8) & 0xFF);
  uint8_t a = static_cast<uint8_t>(hex & 0xFF);
  return pipeline(core::rgba8_t{r, g, b, a});
}

inline pipeline make_rgb_hex(uint32_t hex) {
  uint8_t r = static_cast<uint8_t>((hex >> 16) & 0xFF);
  uint8_t g = static_cast<uint8_t>((hex >> 8) & 0xFF);
  uint8_t b = static_cast<uint8_t>(hex & 0xFF);
  return pipeline(core::rgba8_t{r, g, b, 255});
}

inline pipeline make_rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
  return pipeline(core::rgba8_t{r, g, b, a});
}

}  // namespace colorcpp::operations::flow
