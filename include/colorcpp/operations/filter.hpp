/**
 * @file adjust.hpp
 * @brief Complete Color adjustment and filter operations
 *
 * Provides both compile-time (template-based) and run-time (constexpr)
 * color manipulation including HSL/HSV adjustments and RGB filters.
 *
 * @author Merlot.Qi
 */

#pragma once

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <colorcpp/operations/color_cast.hpp>
#include <colorcpp/traits/concepts.hpp>
#include <vector>

namespace color::operations::filter {

namespace details {

template <typename Color>
auto to_rgb_float(const Color& c) {
  return conversion::color_cast<core::basic_rgba<float, std::ratio<1>>, Color>(c);
}

template <typename Color>
Color from_rgb_float(const core::basic_rgba<float, std::ratio<1>>& c) {
  return conversion::color_cast<Color>(c);
}

}  // namespace details

template <typename Color, typename Func>
Color point(const Color& c, Func&& fn) {
  auto rgb = details::to_rgb_float(c);

  rgb.r = std::clamp(fn(rgb.r), 0.0, 1.0);
  rgb.g = std::clamp(fn(rgb.g), 0.0, 1.0);
  rgb.b = std::clamp(fn(rgb.b), 0.0, 1.0);

  return details::from_rgb_float<Color>(rgb);
}

template <typename Color>
Color invert(const Color& c) {
  return point(c, [](float x) { return 1.0 - x; });
}

template <typename Color>
Color gamma(const Color& c, float g) {
  return point(c, [g](float x) { return std::pow(x, 1.0 / g); });
}

template <typename Color, size_t N>
Color convolve(const std::vector<std::vector<Color>>& image, size_t x, size_t y,
               const std::array<std::array<float, N>, N>& kernel) {
  int half = static_cast<int>(N / 2);

  float r = 0, g = 0, b = 0, a = 0;
  float weight_sum = 0;

  for (int ky = 0; ky < static_cast<int>(N); ++ky) {
    for (int kx = 0; kx < static_cast<int>(N); ++kx) {
      int ix = static_cast<int>(x) + kx - half;
      int iy = static_cast<int>(y) + ky - half;

      if (iy < 0 || iy >= static_cast<int>(image.size()) || ix < 0 || ix >= static_cast<int>(image[0].size())) continue;

      auto rgb = details::to_rgb_float(image[iy][ix]);

      float weight = kernel[ky][kx];

      r += rgb.r * weight;
      g += rgb.g * weight;
      b += rgb.b * weight;
      a += rgb.a * weight;

      weight_sum += weight;
    }
  }

  if (weight_sum != 0) {
    r /= weight_sum;
    g /= weight_sum;
    b /= weight_sum;
    a /= weight_sum;
  }

  core::basic_rgba<float, std::ratio<1>> out{r, g, b, a};

  return details::from_rgb_float<Color>(out);
}

template <typename Color, size_t N>
std::vector<std::vector<Color>> apply_kernel(const std::vector<std::vector<Color>>& image,
                                             const std::array<std::array<float, N>, N>& kernel) {
  size_t h = image.size();
  size_t w = image[0].size();

  std::vector<std::vector<Color>> output(h, std::vector<Color>(w));

  for (size_t y = 0; y < h; ++y) {
    for (size_t x = 0; x < w; ++x) {
      output[y][x] = convolve<Color, N>(image, x, y, kernel);
    }
  }

  return output;
}

}  // namespace color::operations::filter
