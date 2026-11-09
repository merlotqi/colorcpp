/**
 * @file color_lerp.hpp
 * @brief High-level color interpolation: multi-stop gradients, easing, alpha control, and stop-based gradients.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace colorcpp::operations::interpolate {

/**
 * @brief A color stop with position for gradient interpolation.
 * @tparam Color The color type.
 */
template <typename Color>
struct stop {
  Color color;     ///< The color at this stop.
  float position;  ///< Position in [0, 1] along the gradient.
};

/**
 * @brief Easing curves mapping @f$t \in [0,1]@f$ → @f$[0,1]@f$ (back easing may overshoot slightly).
 *        Pass to @ref ease, @ref ease_hsl, @ref ease_oklab, or @ref ease_oklch.
 */
namespace easing {

static constexpr float kPi = 3.14159265358979323846f;

inline float linear(float t) noexcept { return t; }

// Quadratic
inline float in_quad(float t) noexcept { return t * t; }
inline float out_quad(float t) noexcept { return t * (2.0f - t); }
inline float in_out_quad(float t) noexcept { return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t; }

// Cubic
inline float in_cubic(float t) noexcept { return t * t * t; }
inline float out_cubic(float t) noexcept {
  float s = 1.0f - t;
  return 1.0f - s * s * s;
}
inline float in_out_cubic(float t) noexcept {
  return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;
}

// Smoothstep
inline float smoothstep(float t) noexcept { return t * t * (3.0f - 2.0f * t); }

// Smootherstep
inline float smootherstep(float t) noexcept { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

// Sine
inline float in_sine(float t) noexcept { return 1.0f - std::cos(t * kPi * 0.5f); }
inline float out_sine(float t) noexcept { return std::sin(t * kPi * 0.5f); }
inline float in_out_sine(float t) noexcept { return -(std::cos(kPi * t) - 1.0f) * 0.5f; }

// Exponential
inline float in_expo(float t) noexcept { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f); }
inline float out_expo(float t) noexcept { return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }
inline float in_out_expo(float t) noexcept {
  if (t == 0.0f) return 0.0f;
  if (t == 1.0f) return 1.0f;
  return t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) * 0.5f : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) * 0.5f;
}

// Back
inline float in_back(float t) noexcept {
  constexpr float c = 1.70158f;
  return (c + 1.0f) * t * t * t - c * t * t;
}
inline float out_back(float t) noexcept {
  constexpr float c = 1.70158f;
  float s = 1.0f - t;
  return 1.0f - ((c + 1.0f) * s * s * s - c * s * s);
}
inline float in_out_back(float t) noexcept {
  constexpr float c = 1.70158f * 1.525f;
  return t < 0.5f ? (std::pow(2.0f * t, 2.0f) * ((c + 1.0f) * 2.0f * t - c)) * 0.5f
                  : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c + 1.0f) * (2.0f * t - 2.0f) + c) + 2.0f) * 0.5f;
}

}  // namespace easing

enum class alpha_mode {
  lerp,     ///< Interpolate alpha channel linearly between start and end values
  start,    ///< Use the start color's alpha value throughout the interpolation
  end,      ///< Use the end color's alpha value throughout the interpolation
  multiply  ///< Geometric blend of the alphas: pow(start, 1-t) * pow(end, t) (issue #81)
};

/**
 * @brief Interpolation with independent alpha channel control.
 *
 * Color channels are interpolated in @p Color's own space (same as
 * @ref lerp — issue #50: the old @c rgbaf_t hub clipped wide gamuts);
 * only the alpha selection policy differs.
 * @param a Start color
 * @param b End color
 * @param t Interpolation factor in [0, 1] (clamped).
 * @param mode Alpha interpolation mode (default: lerp).
 */
template <typename Color>
Color lerp_alpha_mode(const Color& a, const Color& b, float t, alpha_mode mode = alpha_mode::lerp) {
  t = details::sanitize_t(t);

  Color mid = details::lerp_channels(a, b, t);

  if constexpr (conversion::details::has_alpha_v<Color>) {
    constexpr std::size_t ai = conversion::details::alpha_index_v<Color>;

    const float aa = details::src_alpha(a);
    const float ab = details::src_alpha(b);

    float alpha;
    switch (mode) {
      case alpha_mode::start:
        alpha = aa;
        break;
      case alpha_mode::end:
        alpha = ab;
        break;
      case alpha_mode::multiply:
        // Geometric blend (issue #81): pow(aa, 1-t) * pow(ab, t) is the only
        // product-flavored curve that stays t-dependent and still hits both
        // endpoints exactly; the old constant aa * ab ignored t and broke
        // them. Alphas are clamped first so a negative stored value cannot
        // produce NaN inside std::pow.
        alpha = std::pow(std::clamp(aa, 0.0f, 1.0f), 1.0f - t) * std::pow(std::clamp(ab, 0.0f, 1.0f), t);
        break;
      case alpha_mode::lerp:
      default:
        alpha = details::lerp_f(aa, ab, t);
        break;
    }

    mid.template get_index<ai>() = conversion::details::from_unit<Color, ai>(alpha);
  }
  return mid;
}

/**
 * @brief RGB @ref lerp with @p ef applied to @p t (e.g. @c easing::in_cubic).
 *
 * The caller's own @p t is clamped to [0, 1] (a unit-interval parameter), but
 * the eased factor is fed to the blend UNCLAMPED (issue #81): back easing
 * deliberately overshoots below 0 / above 1 and that overshoot must
 * extrapolate, not collapse onto the endpoint.
 */
template <typename Color, typename EasingFunc>
Color ease(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return details::lerp_channels(a, b, ef(details::sanitize_t(t)));
}

/** @brief HSL @ref lerp_hsl with easing on @p t (raw eased factor, see @ref ease). */
template <typename Color, typename EasingFunc>
Color ease_hsl(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return details::lerp_hsl_unclamped(a, b, ef(details::sanitize_t(t)));
}

/** @brief OkLab @ref lerp_oklab with easing on @p t (raw eased factor, see @ref ease). */
template <typename Color, typename EasingFunc>
Color ease_oklab(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return details::lerp_oklab_unclamped(a, b, ef(details::sanitize_t(t)));
}

/** @brief OkLCH @ref lerp_oklch with easing on @p t (raw eased factor, see @ref ease). */
template <typename Color, typename EasingFunc>
Color ease_oklch(const Color& a, const Color& b, float t, EasingFunc&& ef) {
  return details::lerp_oklch_unclamped(a, b, ef(details::sanitize_t(t)));
}

/** @brief Shorthand for @ref ease with @c easing::smoothstep. */
template <typename Color>
Color ease_in_out(const Color& a, const Color& b, float t) {
  return ease(a, b, t, easing::smoothstep);
}

namespace details {

// Shared blend step (issue #84): every multi-stop sampler below and every
// two-color caller routes through the same lerp / lerp_hsl / lerp_oklab /
// lerp_oklch entry points, so the working-space entry and the alpha-policy
// application have exactly one implementation per space. The samplers take
// that step as their Blend parameter instead of re-copying it.

// Generic multi-stop sampling core over evenly spaced stops (issue #84).
// Accepts any random-access range of colors (initializer_list, vector, ...)
// plus the per-segment blend and the caller-specific error message.
template <typename Range, typename Blend>
auto multi_lerp_even_impl(const Range& stops, float t, Blend&& blend,
                          const char* what) -> std::decay_t<decltype(*std::begin(stops))> {
  if (std::size(stops) < 2) throw std::invalid_argument(what);
  t = sanitize_t(t);

  const std::size_t n = std::size(stops) - 1;
  float scaled = t * static_cast<float>(n);
  std::size_t i = std::min(static_cast<std::size_t>(scaled), n - 1);
  float local_t = scaled - static_cast<float>(i);

  auto it = std::begin(stops);
  std::advance(it, i);
  const auto& lo = *it++;
  const auto& hi = *it;
  return blend(lo, hi, local_t);
}

// Generic multi-stop sampling core over explicit stop positions (issue #84).
// The CSS Gradient non-decreasing stop rule (issue #82) is kept verbatim:
// this family's scan is a different position model than the evenly spaced
// core above, so both formulas stay byte-for-byte identical.
template <typename Color, typename Blend>
Color multi_lerp_stops_impl(const std::vector<stop<Color>>& stops, float t, Blend&& blend, const char* what) {
  if (stops.size() < 2) throw std::invalid_argument(what);
  t = sanitize_t(t);

  // CSS Gradient stop rule (issue #82): positions are normalized to be
  // non-decreasing (pos_i = max(pos_i, pos_{i-1})) before sampling, so
  // out-of-order input can no longer sample from arbitrary segments.
  float prev_pos = stops.front().position;
  float back_pos = prev_pos;
  for (const auto& s : stops) back_pos = std::max(back_pos, s.position);

  if (t <= stops.front().position) return stops.front().color;
  if (t >= back_pos) return stops.back().color;

  for (std::size_t i = 0; i < stops.size() - 1; ++i) {
    const auto& lo = stops[i];
    const auto& hi = stops[i + 1];
    const float lo_pos = prev_pos;
    const float hi_pos = std::max(hi.position, prev_pos);
    prev_pos = hi_pos;
    if (t >= lo_pos && t <= hi_pos) {
      float range = hi_pos - lo_pos;
      if (range < 1e-10f) return lo.color;
      float local_t = (t - lo_pos) / range;
      return blend(lo.color, hi.color, local_t);
    }
  }

  return stops.back().color;
}

// Generic count-based sampling core (issue #84).
template <typename Color, typename Blend>
std::vector<Color> lerp_n_impl(const Color& a, const Color& b, std::size_t count, Blend&& blend) {
  std::vector<Color> out;
  if (count == 0) return out;
  out.reserve(count);
  if (count == 1) {
    out.push_back(a);
    return out;
  }
  for (std::size_t i = 0; i < count; ++i)
    out.push_back(blend(a, b, static_cast<float>(i) / static_cast<float>(count - 1)));
  return out;
}

}  // namespace details

/** @brief Piecewise @ref lerp across evenly spaced stops on @f$t \in [0,1]@f$; at least two stops required. */
template <typename Color>
Color multi_lerp(std::initializer_list<Color> stops, float t) {
  return details::multi_lerp_even_impl(stops, t, lerp<Color>, "colorcpp: multi_lerp requires at least 2 colour stops");
}

/** @brief Piecewise @ref lerp_hsl between adjacent stops. */
template <typename Color>
Color multi_lerp_hsl(std::initializer_list<Color> stops, float t) {
  return details::multi_lerp_even_impl(stops, t, lerp_hsl<Color>,
                                       "colorcpp: multi_lerp_hsl requires at least 2 colour stops");
}

/** @brief Piecewise @ref lerp_oklab between adjacent stops. */
template <typename Color>
Color multi_lerp_oklab(std::initializer_list<Color> stops, float t) {
  return details::multi_lerp_even_impl(stops, t, lerp_oklab<Color>,
                                       "colorcpp: multi_lerp_oklab requires at least 2 colour stops");
}

/** @brief Piecewise @ref lerp_oklch between adjacent stops. */
template <typename Color>
Color multi_lerp_oklch(std::initializer_list<Color> stops, float t) {
  return details::multi_lerp_even_impl(stops, t, lerp_oklch<Color>,
                                       "colorcpp: multi_lerp_oklch requires at least 2 colour stops");
}

/**
 * @brief Piecewise @ref lerp across stops with explicit positions on @f$t \in [0,1]@f$.
 * @param stops Vector of color stops with positions; at least two required.
 * @param t Interpolation factor in [0, 1] (clamped).
 */
template <typename Color>
Color multi_lerp(const std::vector<stop<Color>>& stops, float t) {
  return details::multi_lerp_stops_impl(stops, t, lerp<Color>, "colorcpp: multi_lerp requires at least 2 colour stops");
}

/**
 * @brief Piecewise @ref lerp_hsl across stops with explicit positions.
 */
template <typename Color>
Color multi_lerp_hsl(const std::vector<stop<Color>>& stops, float t) {
  return details::multi_lerp_stops_impl(stops, t, lerp_hsl<Color>,
                                        "colorcpp: multi_lerp_hsl requires at least 2 colour stops");
}

/**
 * @brief Piecewise @ref lerp_oklab across stops with explicit positions.
 */
template <typename Color>
Color multi_lerp_oklab(const std::vector<stop<Color>>& stops, float t) {
  return details::multi_lerp_stops_impl(stops, t, lerp_oklab<Color>,
                                        "colorcpp: multi_lerp_oklab requires at least 2 colour stops");
}

/**
 * @brief Piecewise @ref lerp_oklch across stops with explicit positions.
 */
template <typename Color>
Color multi_lerp_oklch(const std::vector<stop<Color>>& stops, float t) {
  return details::multi_lerp_stops_impl(stops, t, lerp_oklch<Color>,
                                        "colorcpp: multi_lerp_oklch requires at least 2 colour stops");
}

/** @brief @p count evenly spaced samples from @p a to @p b via @ref lerp. */
template <typename Color>
std::vector<Color> lerp_n(const Color& a, const Color& b, std::size_t count) {
  return details::lerp_n_impl(a, b, count, lerp<Color>);
}

/** @brief @p count samples via @ref lerp_hsl. */
template <typename Color>
std::vector<Color> lerp_hsl_n(const Color& a, const Color& b, std::size_t count) {
  return details::lerp_n_impl(a, b, count, lerp_hsl<Color>);
}

/** @brief @p count samples via @ref lerp_oklab. */
template <typename Color>
std::vector<Color> lerp_oklab_n(const Color& a, const Color& b, std::size_t count) {
  return details::lerp_n_impl(a, b, count, lerp_oklab<Color>);
}

/** @brief @p count samples via @ref lerp_oklch. */
template <typename Color>
std::vector<Color> lerp_oklch_n(const Color& a, const Color& b, std::size_t count) {
  return details::lerp_n_impl(a, b, count, lerp_oklch<Color>);
}

}  // namespace colorcpp::operations::interpolate
