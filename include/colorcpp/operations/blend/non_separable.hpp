/**
 * @file non_separable.hpp
 * @brief Non-separable blend modes (hue, saturation, color, luminosity).
 */

#pragma once

#include <algorithm>

namespace colorcpp::operations::blend::details {

struct rgb3 {
  float r, g, b;
};

inline float lum(rgb3 c) { return 0.299f * c.r + 0.587f * c.g + 0.114f * c.b; }

inline float sat(rgb3 c) { return std::max({c.r, c.g, c.b}) - std::min({c.r, c.g, c.b}); }

inline rgb3 clip_color(rgb3 c) {
  float l = lum(c);
  float n = std::min({c.r, c.g, c.b});
  float x = std::max({c.r, c.g, c.b});
  if (n < 0.0f) {
    // d == 0 when all channels are equal (achromatic): scaling is a no-op, skip.
    float d = l - n;
    if (d > 0.0f) {
      c.r = l + (c.r - l) * l / d;
      c.g = l + (c.g - l) * l / d;
      c.b = l + (c.b - l) * l / d;
    }
  }
  if (x > 1.0f) {
    // d == 0 when all channels are equal (achromatic): scaling is a no-op, skip.
    float d = x - l;
    if (d > 0.0f) {
      c.r = l + (c.r - l) * (1.0f - l) / d;
      c.g = l + (c.g - l) * (1.0f - l) / d;
      c.b = l + (c.b - l) * (1.0f - l) / d;
    }
  }
  return c;
}

inline rgb3 set_lum(rgb3 c, float l) {
  float d = l - lum(c);
  return clip_color({c.r + d, c.g + d, c.b + d});
}

// Adjusts saturation while preserving per-component hue ordering (W3C SetSat)
inline rgb3 set_sat(rgb3 c, float s) {
  float* p[3] = {&c.r, &c.g, &c.b};
  // 3-element sort so p[0]=min, p[1]=mid, p[2]=max
  if (*p[0] > *p[1]) std::swap(p[0], p[1]);
  if (*p[1] > *p[2]) std::swap(p[1], p[2]);
  if (*p[0] > *p[1]) std::swap(p[0], p[1]);

  float cmin = *p[0], cmid = *p[1], cmax = *p[2];
  if (cmax > cmin) {
    *p[1] = (cmid - cmin) * s / (cmax - cmin);
    *p[2] = s;
  } else {
    *p[1] = 0.0f;
    *p[2] = 0.0f;
  }
  *p[0] = 0.0f;
  return c;
}

inline rgb3 ns_hue(rgb3 dst, rgb3 src) { return set_lum(set_sat(src, sat(dst)), lum(dst)); }
inline rgb3 ns_saturation(rgb3 dst, rgb3 src) { return set_lum(set_sat(dst, sat(src)), lum(dst)); }
inline rgb3 ns_color(rgb3 dst, rgb3 src) { return set_lum(src, lum(dst)); }
inline rgb3 ns_luminosity(rgb3 dst, rgb3 src) { return set_lum(dst, lum(src)); }

}  // namespace colorcpp::operations::blend::details
