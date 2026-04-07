/**
 * @file scheme.hpp
 * @brief Color scheme generation utilities.
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/flow/pipeline.hpp>
#include <vector>

namespace colorcpp::operations::flow {

enum class scheme_type {
  monochromatic,
  analogous,
  complementary,
  triadic,
  tetradic,
  square,
  split_complementary,
};

inline std::vector<core::rgba8_t> generate_scheme(const core::rgba8_t& base, scheme_type type, int count = 5) {
  std::vector<core::rgba8_t> result;

  switch (type) {
    case scheme_type::monochromatic: {
      result.reserve(static_cast<size_t>(count));
      for (int i = 0; i < count; ++i) {
        float l = 0.1f + static_cast<float>(i) / static_cast<float>(count - 1) * 0.85f;
        result.push_back(pipeline(base).saturate(10).darken((0.5f - l) * 100).color());
      }
      break;
    }
    case scheme_type::analogous: {
      if (count < 3) count = 3;
      if (count > 7) count = 7;
      result.reserve(static_cast<size_t>(count));
      int spread = 20;
      for (int i = 0; i < count; ++i) {
        float offset = static_cast<float>(i - (count - 1) / 2.f) * spread;
        result.push_back(pipeline(base).rotate_hue(offset).color());
      }
      break;
    }
    case scheme_type::complementary: {
      result.reserve(2);
      result.push_back(base);
      result.push_back(pipeline(base).rotate_hue(180).color());
      break;
    }
    case scheme_type::triadic: {
      result.reserve(3);
      result.push_back(base);
      result.push_back(pipeline(base).rotate_hue(120).color());
      result.push_back(pipeline(base).rotate_hue(240).color());
      break;
    }
    case scheme_type::tetradic: {
      result.reserve(4);
      result.push_back(base);
      result.push_back(pipeline(base).rotate_hue(60).color());
      result.push_back(pipeline(base).rotate_hue(180).color());
      result.push_back(pipeline(base).rotate_hue(240).color());
      break;
    }
    case scheme_type::square: {
      result.reserve(4);
      result.push_back(base);
      result.push_back(pipeline(base).rotate_hue(90).color());
      result.push_back(pipeline(base).rotate_hue(180).color());
      result.push_back(pipeline(base).rotate_hue(270).color());
      break;
    }
    case scheme_type::split_complementary: {
      result.reserve(3);
      result.push_back(base);
      result.push_back(pipeline(base).rotate_hue(150).color());
      result.push_back(pipeline(base).rotate_hue(210).color());
      break;
    }
  }
  return result;
}

inline std::vector<core::rgba8_t> warm_palette(const core::rgba8_t& base) {
  return generate_scheme(base, scheme_type::analogous, 5);
}

inline std::vector<core::rgba8_t> cool_palette(const core::rgba8_t& base) {
  return generate_scheme(base, scheme_type::analogous, 5);
}

inline std::vector<core::rgba8_t> pastel_palette(const core::rgba8_t& base) {
  std::vector<core::rgba8_t> result;
  auto hsla = operations::conversion::color_cast<core::hsla_float_t>(base);
  float hue = hsla.h();
  result.push_back(make_rgb(255, 179, 186).set_hue(hue).color());
  result.push_back(make_rgb(255, 223, 186).set_hue(hue).color());
  result.push_back(make_rgb(255, 255, 186).set_hue(hue).color());
  result.push_back(make_rgb(186, 255, 201).set_hue(hue).color());
  result.push_back(make_rgb(186, 225, 255).set_hue(hue).color());
  return result;
}

}  // namespace colorcpp::operations::flow
