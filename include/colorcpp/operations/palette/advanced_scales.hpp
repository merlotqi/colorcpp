/**
 * @file advanced_scales.hpp
 * @brief Advanced gradient generators with easing functions, spline curves, and multi-color transitions.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/interpolate.hpp>
#include <colorcpp/operations/palette/palette_set.hpp>
#include <vector>

namespace colorcpp::operations::palette {

/**
 * @brief Interpolation methods enum for convenience.
 */
enum class lerp_method {
  rgb,   ///< RGB linear interpolation
  hsl,   ///< HSL interpolation
  hsv,   ///< HSV interpolation
  oklab  ///< OkLab perceptual interpolation (default)
};

/**
 * @brief Easing function types for gradient generation.
 */
enum class easing_type {
  linear,          ///< Linear interpolation (no easing)
  ease_in,         ///< Quadratic ease-in (slow start)
  ease_out,        ///< Quadratic ease-out (slow end)
  ease_in_out,     ///< Quadratic ease-in-out (slow start and end)
  cubic_in,        ///< Cubic ease-in
  cubic_out,       ///< Cubic ease-out
  cubic_in_out,    ///< Cubic ease-in-out
  quartic_in,      ///< Quartic ease-in
  quartic_out,     ///< Quartic ease-out
  quartic_in_out,  ///< Quartic ease-in-out
  elastic,         ///< Elastic bounce effect
  bounce           ///< Bouncing effect
};

namespace details {

/**
 * @brief Apply easing function to a linear parameter t.
 * @param t Linear parameter in [0, 1].
 * @param easing Easing function type.
 * @return Eased parameter in [0, 1].
 */
inline float apply_easing(float t, easing_type easing) {
  t = std::clamp(t, 0.0f, 1.0f);

  switch (easing) {
    case easing_type::linear:
      return t;

    case easing_type::ease_in:
      return t * t;

    case easing_type::ease_out:
      return 1.0f - (1.0f - t) * (1.0f - t);

    case easing_type::ease_in_out:
      return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);

    case easing_type::cubic_in:
      return t * t * t;

    case easing_type::cubic_out:
      return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

    case easing_type::cubic_in_out:
      return t < 0.5f ? 4.0f * t * t * t : 1.0f - 4.0f * (1.0f - t) * (1.0f - t) * (1.0f - t);

    case easing_type::quartic_in:
      return t * t * t * t;

    case easing_type::quartic_out:
      return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);

    case easing_type::quartic_in_out:
      return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - 8.0f * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);

    case easing_type::elastic: {
      if (t == 0.0f || t == 1.0f) return t;
      constexpr float two_pi = 6.283185307179586476925286766559f;
      float p = 0.3f;
      float s = p / 4.0f;
      return std::pow(2.0f, -10.0f * t) * std::sin((t - s) * two_pi / p) + 1.0f;
    }

    case easing_type::bounce: {
      if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
      } else if (t < 2.0f / 2.75f) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
      } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
      } else {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
      }
    }

    default:
      return t;
  }
}

}  // namespace details

/**
 * @brief Generate gradient with easing function.
 * @param start Start color.
 * @param end End color.
 * @param count Number of colors to generate.
 * @param easing Easing function type.
 * @param method Interpolation method (default: OkLab).
 * @return Palette with eased gradient colors.
 */
template <typename Color>
auto easing_scale(const Color& start, const Color& end, size_t count, easing_type easing,
                  lerp_method method = lerp_method::oklab) {
  palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t_linear = static_cast<float>(i) / (count - 1);
    float t_eased = details::apply_easing(t_linear, easing);

    switch (method) {
      case lerp_method::rgb:
        p.add(interpolate::lerp(start, end, t_eased));
        break;
      case lerp_method::hsl:
        p.add(interpolate::lerp_hsl(start, end, t_eased));
        break;
      case lerp_method::hsv:
        p.add(interpolate::lerp_hsv(start, end, t_eased));
        break;
      case lerp_method::oklab:
      default:
        p.add(interpolate::lerp_oklab(start, end, t_eased));
        break;
    }
  }
  return p;
}

/**
 * @brief Generate gradient through multiple control points using Catmull-Rom spline.
 * @param control_points Vector of control colors (minimum 4 for proper spline).
 * @param count Total number of colors to generate.
 * @param mode Catmull-Rom parameterization mode.
 * @return Palette with spline-interpolated colors.
 */
template <typename Color>
auto spline_scale(const std::vector<Color>& control_points, size_t count,
                  interpolate::cr_mode mode = interpolate::cr_mode::centripetal) {
  palette_set<Color> p;

  if (control_points.empty() || count == 0) return p;
  if (control_points.size() == 1) {
    for (size_t i = 0; i < count; ++i) {
      p.add(control_points[0]);
    }
    return p;
  }

  // For 2 points, fall back to linear interpolation
  if (control_points.size() == 2) {
    for (size_t i = 0; i < count; ++i) {
      float t = static_cast<float>(i) / (count - 1);
      p.add(interpolate::lerp_oklab(control_points[0], control_points[1], t));
    }
    return p;
  }

  // Distribute samples across segments
  size_t num_segments = control_points.size() - 1;
  size_t samples_per_segment = count / num_segments;
  size_t remainder = count % num_segments;

  for (size_t seg = 0; seg < num_segments; ++seg) {
    size_t seg_samples = samples_per_segment + (seg < remainder ? 1 : 0);
    bool is_last_segment = (seg == num_segments - 1);

    // Get control points for Catmull-Rom (need 4 points)
    size_t i0 = (seg == 0) ? 0 : seg - 1;
    size_t i1 = seg;
    size_t i2 = seg + 1;
    size_t i3 = (seg + 2 < control_points.size()) ? seg + 2 : control_points.size() - 1;

    // For the last segment, don't include the endpoint (we'll add it at the end)
    size_t seg_count = is_last_segment ? seg_samples - 1 : seg_samples;

    for (size_t i = 0; i < seg_count; ++i) {
      float t = static_cast<float>(i) / seg_samples;

      // Use OkLab interpolation with Catmull-Rom
      using namespace conversion;
      auto c0 = color_cast<core::oklab_t>(control_points[i0]);
      auto c1 = color_cast<core::oklab_t>(control_points[i1]);
      auto c2 = color_cast<core::oklab_t>(control_points[i2]);
      auto c3 = color_cast<core::oklab_t>(control_points[i3]);

      // Apply parameterization
      float t_L =
          interpolate::details::cr_parameterize(c0.template get_index<0>(), c1.template get_index<0>(),
                                                c2.template get_index<0>(), c3.template get_index<0>(), t, mode);
      float t_A =
          interpolate::details::cr_parameterize(c0.template get_index<1>(), c1.template get_index<1>(),
                                                c2.template get_index<1>(), c3.template get_index<1>(), t, mode);
      float t_B =
          interpolate::details::cr_parameterize(c0.template get_index<2>(), c1.template get_index<2>(),
                                                c2.template get_index<2>(), c3.template get_index<2>(), t, mode);

      core::oklab_t mid{
          std::clamp(interpolate::details::catmull_rom(c0.template get_index<0>(), c1.template get_index<0>(),
                                                       c2.template get_index<0>(), c3.template get_index<0>(), t_L),
                     0.0f, 1.0f),
          std::clamp(interpolate::details::catmull_rom(c0.template get_index<1>(), c1.template get_index<1>(),
                                                       c2.template get_index<1>(), c3.template get_index<1>(), t_A),
                     -0.5f, 0.5f),
          std::clamp(interpolate::details::catmull_rom(c0.template get_index<2>(), c1.template get_index<2>(),
                                                       c2.template get_index<2>(), c3.template get_index<2>(), t_B),
                     -0.5f, 0.5f)};

      // Interpolate alpha
      auto r1 = color_cast<core::rgbaf_t>(control_points[i1]);
      auto r2 = color_cast<core::rgbaf_t>(control_points[i2]);
      auto out = color_cast<core::rgbaf_t>(mid);
      out.template get_index<3>() =
          interpolate::details::lerp_f(r1.template get_index<3>(), r2.template get_index<3>(), t);

      p.add(color_cast<Color>(out));
    }
  }

  // Add the last control point
  p.add(control_points.back());

  return p;
}

/**
 * @brief Generate smooth multi-color gradient.
 * @param colors Vector of colors to interpolate through.
 * @param count Total number of colors to generate.
 * @param method Interpolation method.
 * @return Palette with smooth multi-color gradient.
 */
template <typename Color>
auto multi_scale(const std::vector<Color>& colors, size_t count, lerp_method method = lerp_method::oklab) {
  palette_set<Color> p;

  if (colors.empty() || count == 0) return p;
  if (colors.size() == 1) {
    for (size_t i = 0; i < count; ++i) {
      p.add(colors[0]);
    }
    return p;
  }

  if (count == 1) {
    p.add(colors[0]);
    return p;
  }

  size_t num_segments = colors.size() - 1;
  size_t samples_per_segment = count / num_segments;
  size_t remainder = count % num_segments;

  for (size_t seg = 0; seg < num_segments; ++seg) {
    size_t seg_samples = samples_per_segment + (seg < remainder ? 1 : 0);
    // For the last segment, don't add the endpoint (we'll add it at the end)
    bool is_last_segment = (seg == num_segments - 1);
    size_t seg_count = is_last_segment ? seg_samples - 1 : seg_samples;

    for (size_t i = 0; i < seg_count; ++i) {
      float t = static_cast<float>(i) / seg_samples;

      switch (method) {
        case lerp_method::rgb:
          p.add(interpolate::lerp(colors[seg], colors[seg + 1], t));
          break;
        case lerp_method::hsl:
          p.add(interpolate::lerp_hsl(colors[seg], colors[seg + 1], t));
          break;
        case lerp_method::hsv:
          p.add(interpolate::lerp_hsv(colors[seg], colors[seg + 1], t));
          break;
        case lerp_method::oklab:
        default:
          p.add(interpolate::lerp_oklab(colors[seg], colors[seg + 1], t));
          break;
      }
    }
  }

  // Add the last color
  p.add(colors.back());

  return p;
}

}  // namespace colorcpp::operations::palette
