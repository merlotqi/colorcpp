/**
 * @file lut.hpp
 * @brief Color lookup table (LUT) data structures for 1D and 3D LUTs.
 *
 * Provides LUT types used by industry-standard formats (e.g. .cube, .csp).
 */

#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace colorcpp::io::binary_io {

/**
 * @brief 1D color lookup table — independent per-channel mapping.
 *
 * Each channel (R, G, B) has its own lookup table of @c size entries,
 * mapping input values within [domain_min, domain_max] to output values.
 */
struct lut1d {
  /** @brief Number of entries per channel. */
  std::size_t size = 0;

  /** @brief Domain minimum for R, G, B channels. */
  std::array<float, 3> domain_min = {0.0f, 0.0f, 0.0f};

  /** @brief Domain maximum for R, G, B channels. */
  std::array<float, 3> domain_max = {1.0f, 1.0f, 1.0f};

  /** @brief Title or description (optional). */
  std::string title;

  /** @brief Red channel lookup values. */
  std::vector<float> r;

  /** @brief Green channel lookup values. */
  std::vector<float> g;

  /** @brief Blue channel lookup values. */
  std::vector<float> b;

  /** @brief Check if the LUT is valid (size > 0 and data populated). */
  bool valid() const { return size > 0 && r.size() == size && g.size() == size && b.size() == size; }
};

/**
 * @brief 3D color lookup table — grid-based color transform.
 *
 * A 3D LUT is an NxNxN grid of RGB triplets. Input RGB values are
 * interpolated (typically trilinear) to produce output RGB values.
 *
 * Storage order: index = r * size*size + g * size + b (red varies slowest).
 */
struct lut3d {
  /** @brief Grid dimension (N for NxNxN). */
  std::size_t size = 0;

  /** @brief Domain minimum for R, G, B channels. */
  std::array<float, 3> domain_min = {0.0f, 0.0f, 0.0f};

  /** @brief Domain maximum for R, G, B channels. */
  std::array<float, 3> domain_max = {1.0f, 1.0f, 1.0f};

  /** @brief Title or description (optional). */
  std::string title;

  /** @brief Flat array of NxNxN RGB triplets. */
  std::vector<std::array<float, 3>> data;

  /** @brief Check if the LUT is valid. */
  bool valid() const { return size > 0 && data.size() == size * size * size; }

  /** @brief Access a grid point by (r, g, b) indices. */
  const std::array<float, 3>& at(std::size_t ri, std::size_t gi, std::size_t bi) const {
    return data[ri * size * size + gi * size + bi];
  }

  /** @brief Access a grid point by (r, g, b) indices. */
  std::array<float, 3>& at(std::size_t ri, std::size_t gi, std::size_t bi) {
    return data[ri * size * size + gi * size + bi];
  }
};

namespace details {

/**
 * @brief Clamp a value to [0, 1].
 */
inline float clamp01(float v) {
  if (v < 0.0f) return 0.0f;
  if (v > 1.0f) return 1.0f;
  return v;
}

/**
 * @brief Normalize an input value from [domain_min, domain_max] to [0, 1].
 */
inline float normalize(float val, float dmin, float dmax) {
  float range = dmax - dmin;
  if (range <= 0.0f) return 0.0f;
  return clamp01((val - dmin) / range);
}

/**
 * @brief Linear interpolation between two values.
 */
inline float lerp(float a, float b, float t) { return a + t * (b - a); }

}  // namespace details

/**
 * @brief Apply a 1D LUT to an RGB triplet (linear interpolation).
 * @param lut The 1D LUT.
 * @param r Input red [0, 1] (or within domain).
 * @param g Input green [0, 1].
 * @param b Input blue [0, 1].
 * @return Output RGB triple.
 */
inline std::array<float, 3> apply(const lut1d& lut, float r, float g, float b) {
  if (!lut.valid()) return {r, g, b};

  auto interp_channel = [](const std::vector<float>& table, float val, float dmin, float dmax,
                           std::size_t size) -> float {
    float t = details::normalize(val, dmin, dmax);
    float idx = t * static_cast<float>(size - 1);
    auto i0 = static_cast<std::size_t>(idx);
    if (i0 >= size - 1) return table.back();
    float frac = idx - static_cast<float>(i0);
    return details::lerp(table[i0], table[i0 + 1], frac);
  };

  return {interp_channel(lut.r, r, lut.domain_min[0], lut.domain_max[0], lut.size),
          interp_channel(lut.g, g, lut.domain_min[1], lut.domain_max[1], lut.size),
          interp_channel(lut.b, b, lut.domain_min[2], lut.domain_max[2], lut.size)};
}

/**
 * @brief Apply a 3D LUT to an RGB triplet (trilinear interpolation).
 * @param lut The 3D LUT.
 * @param r Input red [0, 1] (or within domain).
 * @param g Input green [0, 1].
 * @param b Input blue [0, 1].
 * @return Output RGB triple.
 */
inline std::array<float, 3> apply(const lut3d& lut, float r, float g, float b) {
  if (!lut.valid()) return {r, g, b};

  float tr = details::normalize(r, lut.domain_min[0], lut.domain_max[0]);
  float tg = details::normalize(g, lut.domain_min[1], lut.domain_max[1]);
  float tb = details::normalize(b, lut.domain_min[2], lut.domain_max[2]);

  float idx_r = tr * static_cast<float>(lut.size - 1);
  float idx_g = tg * static_cast<float>(lut.size - 1);
  float idx_b = tb * static_cast<float>(lut.size - 1);

  auto clamp_idx = [sz = lut.size](float idx) -> std::pair<std::size_t, float> {
    auto i = static_cast<std::size_t>(idx);
    if (i >= sz - 1) return {sz - 2, 1.0f};
    return {i, idx - static_cast<float>(i)};
  };

  auto [r0, fr] = clamp_idx(idx_r);
  auto [g0, fg] = clamp_idx(idx_g);
  auto [b0, fb] = clamp_idx(idx_b);

  std::size_t r1 = r0 + 1;
  std::size_t g1 = g0 + 1;
  std::size_t b1 = b0 + 1;

  // Trilinear interpolation
  const auto& c000 = lut.at(r0, g0, b0);
  const auto& c001 = lut.at(r0, g0, b1);
  const auto& c010 = lut.at(r0, g1, b0);
  const auto& c011 = lut.at(r0, g1, b1);
  const auto& c100 = lut.at(r1, g0, b0);
  const auto& c101 = lut.at(r1, g0, b1);
  const auto& c110 = lut.at(r1, g1, b0);
  const auto& c111 = lut.at(r1, g1, b1);

  std::array<float, 3> result{};
  for (int ch = 0; ch < 3; ++ch) {
    float c00 = details::lerp(c000[ch], c100[ch], fr);
    float c01 = details::lerp(c001[ch], c101[ch], fr);
    float c10 = details::lerp(c010[ch], c110[ch], fr);
    float c11 = details::lerp(c011[ch], c111[ch], fr);
    float c0 = details::lerp(c00, c10, fg);
    float c1 = details::lerp(c01, c11, fg);
    result[ch] = details::lerp(c0, c1, fb);
  }

  return result;
}

/**
 * @brief Apply a 3D LUT to any color type with r(), g(), b() accessors.
 * @tparam Color Color type with float r(), g(), b() methods.
 * @return New color with LUT-applied values.
 */
template <typename Color>
Color apply_color(const lut3d& lut, const Color& c) {
  auto [r, g, b] = apply(lut, c.r(), c.g(), c.b());
  Color result = c;
  result.r() = r;
  result.g() = g;
  result.b() = b;
  return result;
}

/**
 * @brief Apply a 1D LUT to any color type with r(), g(), b() accessors.
 */
template <typename Color>
Color apply_color(const lut1d& lut, const Color& c) {
  auto [r, g, b] = apply(lut, c.r(), c.g(), c.b());
  Color result = c;
  result.r() = r;
  result.g() = g;
  result.b() = b;
  return result;
}

}  // namespace colorcpp::io::binary_io
