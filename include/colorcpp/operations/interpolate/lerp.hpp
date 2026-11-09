/**
 * @file lerp.hpp
 * @brief Basic linear interpolation between colors in various color spaces.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gamut/multi_colorspace.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <utility>

namespace colorcpp::operations::interpolate {

namespace details {

inline float lerp_f(float a, float b, float t) { return a + (b - a) * t; }

// Shortest-path hue interpolation in degree space [0, 360).
// Wraps delta into [-180, 180] first so the shorter arc is always taken;
// an exact ±180 tie keeps the sign of (b - a) so the mapping is antisymmetric
// and lerp_angle_deg(a, b, t) == lerp_angle_deg(b, a, 1 - t) (issue #78).
// Inputs may hold any stored angle (e.g. -700° ≡ 20°): both the delta and the
// result are wrapped with sign-preserving fmod, not the old "+540"/"+360"
// tricks, which broke on the ±180 tie and on values below -360°.
inline float lerp_angle_deg(float a, float b, float t) {
  float delta = std::fmod(b - a, 360.0f);
  if (delta > 180.0f)
    delta -= 360.0f;
  else if (delta < -180.0f)
    delta += 360.0f;
  float result = std::fmod(a + delta * t, 360.0f);
  if (result < 0.0f) result += 360.0f;
  return result;
}

// Unit-space alpha of a source color (1.0 when the model carries no alpha).
template <typename Color>
inline float src_alpha(const Color& c) {
  return ::colorcpp::operations::conversion::details::get_src_alpha(c);
}

// CSS Color 4 premultiplied channel blend (issue #80): weight each color
// channel by its endpoint's unit alpha and un-premultiply by the blended
// alpha, matching io/css mix_in_rectangular_space / mix_in_polar_space.
// When the blended alpha is 0 the premultiplied sum passes through
// undivided (the un-premultiplied color is defined as 0 there). Hue and
// alpha channels are never premultiplied.
inline float lerp_premul(float va, float aa, float vb, float ab, float t, float a_mid) {
  const float blended = lerp_f(va * aa, vb * ab, t);
  return a_mid > 0.0f ? blended / a_mid : blended;
}

// Unit-space blend weight of a color's alpha, clamped to [0, 1] like the
// CSS mix reference.
template <typename Color>
inline float premul_weight(const Color& c) {
  return std::clamp(src_alpha(c), 0.0f, 1.0f);
}

/**
 * @brief CSS Color 4 powerless-hue substitution (issue #51).
 *
 * A hue stored next to ~zero chroma/saturation is missing (white's OkLCH hue
 * is 90° of atan2 noise at C ≈ 6e-8). One missing hue takes the other
 * endpoint's; if both are missing the hue carries no information and passes
 * through. @p hue_a / @p hue_b are updated in place.
 */
template <typename Polar>
inline void substitute_powerless_hue(const Polar& a, const Polar& b, float& hue_a, float& hue_b) {
  const bool a_missing = ::colorcpp::core::is_achromatic(a);
  const bool b_missing = ::colorcpp::core::is_achromatic(b);
  if (a_missing && !b_missing)
    hue_a = hue_b;
  else if (b_missing && !a_missing)
    hue_b = hue_a;
}

// Write unit-space alpha into Color's own alpha channel (no-op when the
// model has none).
template <typename Color>
inline void write_alpha(Color& c, float alpha) {
  if constexpr (::colorcpp::operations::conversion::details::has_alpha_v<Color>) {
    constexpr std::size_t ai = ::colorcpp::operations::conversion::details::alpha_index_v<Color>;
    c.template get_index<ai>() = ::colorcpp::operations::conversion::details::from_unit<Color, ai>(alpha);
  }
}

/**
 * @brief Assemble an interpolation result: working-space midpoint + unit-space alpha.
 *
 * Casts the mid straight to Color so an out-of-gamut wide-space midpoint
 * (e.g. lerp_oklab with Color = oklab_t) is never clamped through sRGB;
 * RGB destinations still clamp via the normal conversion path. Alpha is
 * written into Color's own alpha channel when the model has one.
 *
 * When Color has a bounded target gamut (issue #79) an out-of-gamut
 * midpoint is first mapped into that gamut by OkLCH chroma reduction at
 * fixed lightness and hue (CSS Color 4), instead of letting the conversion
 * clip channels and drag hue with it: the green↔blue OkLCH midpoint keeps
 * its 203° hue rather than hard-clipping to 220.6°. In-gamut midpoints and
 * unbounded or extended-range destinations (issue #76) take the plain cast
 * path unchanged.
 */
template <typename Color, typename Mid>
inline Color lerp_result(const Mid& mid, float alpha) {
  if constexpr (::colorcpp::algorithms::gamut::has_target_gamut_v<Color>) {
    constexpr ::colorcpp::algorithms::gamut::gamut_type kGamut = ::colorcpp::algorithms::gamut::target_gamut_v<Color>;
    if (!::colorcpp::algorithms::gamut::is_in_gamut(mid, kGamut)) {
      const auto lab = ::colorcpp::operations::conversion::color_cast<core::oklab_t>(mid);
      const auto mapped = ::colorcpp::algorithms::gamut::gamut_clip_to_gamut(lab, kGamut);
      Color out = ::colorcpp::operations::conversion::color_cast<Color>(mapped);
      write_alpha(out, alpha);
      return out;
    }
  }
  Color out = ::colorcpp::operations::conversion::color_cast<Color>(mid);
  write_alpha(out, alpha);
  return out;
}

// Channel-wise interpolation in Color's own channel space. No conversion
// through a fixed hub type: RGB-family callers keep their own gamut, so
// wide-gamut colors are never clipped to sRGB before interpolating
// (issue #50). Alpha is just another channel here; callers with a policy
// (lerp_alpha_mode) overwrite it afterwards.
//
// Hue channels (hue_tag) interpolate on the shortest arc, not as plain
// numbers (issue #62): 350° → 10° must midpoint at 0°, never 180°.

template <typename Color>
inline constexpr bool has_hue_channel_v =
    ::colorcpp::traits::details::has_channel_tag_impl<typename Color::channels_tuple,
                                                      ::colorcpp::traits::hue_tag>::value;

// Per-channel dispatch: hue goes through the arc lerp, alpha blends
// linearly, and every other channel (S, L, C, a, b, …) blends
// premultiplied (issue #80).
template <typename Color, std::size_t I>
inline float lerp_channel_value(const Color& a, const Color& b, float t, float aa, float ab, float a_mid) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  const float va = static_cast<float>(a.template get_index<I>());
  const float vb = static_cast<float>(b.template get_index<I>());
  if constexpr (std::is_same_v<typename channel_t::tag_type, ::colorcpp::traits::hue_tag>)
    return lerp_angle_deg(va, vb, t);
  else if constexpr (std::is_same_v<typename channel_t::tag_type, ::colorcpp::traits::alpha_tag>)
    return lerp_f(va, vb, t);
  else
    return lerp_premul(va, aa, vb, ab, t, a_mid);
}

template <typename Color, std::size_t... I>
inline Color lerp_channels_impl(const Color& a, const Color& b, float t, float aa, float ab, float a_mid,
                                std::index_sequence<I...>) {
  namespace cdetails = ::colorcpp::operations::conversion::details;
  return cdetails::pack_to<Color>(
      cdetails::from_value<Color, I>(lerp_channel_value<Color, I>(a, b, t, aa, ab, a_mid))...);
}

template <typename Color>
inline Color lerp_channels(const Color& a, const Color& b, float t) {
  // Exact endpoints first (issue #80): premultiplied blending would divide
  // 0/0 at a zero-alpha endpoint, and callers pin t = 0/1 to the inputs.
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;
  const float aa = premul_weight(a);
  const float ab = premul_weight(b);
  const float a_mid = lerp_f(aa, ab, t);
  if constexpr (has_hue_channel_v<Color> && ::colorcpp::core::has_chroma_channel_v<Color>) {
    // Powerless-hue rule (issues #51/#62): chroma ≈ 0 makes the stored hue
    // missing, so it takes the other endpoint's hue before the arc lerp.
    Color ca = a;
    Color cb = b;
    constexpr std::size_t hi = ::colorcpp::traits::details::find_channel_index_impl<typename Color::channels_tuple,
                                                                                    ::colorcpp::traits::hue_tag>::value;
    float hue_a = static_cast<float>(ca.template get_index<hi>());
    float hue_b = static_cast<float>(cb.template get_index<hi>());
    substitute_powerless_hue(ca, cb, hue_a, hue_b);
    ca.template get_index<hi>() = hue_a;
    cb.template get_index<hi>() = hue_b;
    return lerp_channels_impl(ca, cb, t, aa, ab, a_mid, std::make_index_sequence<Color::channels>{});
  } else {
    return lerp_channels_impl(a, b, t, aa, ab, a_mid, std::make_index_sequence<Color::channels>{});
  }
}

}  // namespace details

/**
 * @brief Channel-wise interpolation in @p Color's own color space.
 *
 * Interpolated directly in the caller's space — no conversion through a
 * fixed hub type — so RGB-family callers keep their own gamut: Display P3 /
 * Rec. 2020 endpoints are never clipped to sRGB (issue #50). Color channels
 * blend on their CSS premultiplied form (issue #80): weighted by each
 * endpoint's alpha and un-premultiplied by the blended alpha, so a fully
 * transparent endpoint does not tint the result. Hue takes the shortest arc
 * with the powerless-hue rule applied first (issues #51/#62), matching
 * @ref lerp_hsl, @ref lerp_hsv, and @ref lerp_oklch; alpha blends linearly.
 * @param t Interpolation factor in [0, 1] (clamped).
 */
template <typename Color>
Color lerp(const Color& a, const Color& b, float t) {
  t = std::clamp(t, 0.0f, 1.0f);
  return details::lerp_channels(a, b, t);
}

/**
 * @brief Interpolation in linear sRGB (gamma-free weighting).
 *
 * Converts both endpoints through @c linear_rgbaf_t, so this is the explicit
 * opt-in for "srgb-linear" interpolation; endpoints outside the sRGB gamut
 * are clipped by that conversion. Use @ref lerp to stay in the caller's own
 * space.
 */
template <typename Color>
Color lerp_linear(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);

  const auto ca = color_cast<core::linear_rgbaf_t>(a);
  const auto cb = color_cast<core::linear_rgbaf_t>(b);
  return color_cast<Color>(details::lerp_channels(ca, cb, t));
}

namespace details {

// Unclamped HSL blend core (issue #81): the public @ref lerp_hsl clamps its
// t entry to [0, 1], but ease_hsl feeds the RAW eased factor here so back
// easing's overshoot (t outside [0, 1]) extrapolates instead of being erased
// by a second clamp. Exact t = 0 / 1 still short-circuit to the endpoints.

template <typename Color>
Color lerp_hsl_unclamped(const Color& a, const Color& b, float t) {
  using namespace conversion;
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;

  auto ca = color_cast<core::hsla_float_t>(a);
  auto cb = color_cast<core::hsla_float_t>(b);

  float hue_a = ca.template get_index<0>();
  float hue_b = cb.template get_index<0>();
  details::substitute_powerless_hue(ca, cb, hue_a, hue_b);  // S = 0 → hue missing (issue #51)

  const float aa = details::premul_weight(a);
  const float ab = details::premul_weight(b);
  const float a_mid = details::lerp_f(aa, ab, t);

  core::hsla_float_t out{details::lerp_angle_deg(hue_a, hue_b, t),
                         details::lerp_premul(ca.template get_index<1>(), aa, cb.template get_index<1>(), ab, t, a_mid),
                         details::lerp_premul(ca.template get_index<2>(), aa, cb.template get_index<2>(), ab, t, a_mid),
                         details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t)};

  return color_cast<Color>(out);
}

}  // namespace details

/** @brief HSL interpolation with shortest-arc hue; S, L premultiplied (issue #80), A linear. */
template <typename Color>
Color lerp_hsl(const Color& a, const Color& b, float t) {
  return details::lerp_hsl_unclamped(a, b, std::clamp(t, 0.0f, 1.0f));
}

/** @brief HSV interpolation with shortest-arc hue; S, V premultiplied (issue #80), A linear. */
template <typename Color>
Color lerp_hsv(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;

  auto ca = color_cast<core::hsva_float_t>(a);
  auto cb = color_cast<core::hsva_float_t>(b);

  float hue_a = ca.template get_index<0>();
  float hue_b = cb.template get_index<0>();
  details::substitute_powerless_hue(ca, cb, hue_a, hue_b);  // S = 0 → hue missing (issue #51)

  const float aa = details::premul_weight(a);
  const float ab = details::premul_weight(b);
  const float a_mid = details::lerp_f(aa, ab, t);

  core::hsva_float_t out{details::lerp_angle_deg(hue_a, hue_b, t),
                         details::lerp_premul(ca.template get_index<1>(), aa, cb.template get_index<1>(), ab, t, a_mid),
                         details::lerp_premul(ca.template get_index<2>(), aa, cb.template get_index<2>(), ab, t, a_mid),
                         details::lerp_f(ca.template get_index<3>(), cb.template get_index<3>(), t)};

  return color_cast<Color>(out);
}

namespace details {

// Unclamped OkLab blend core (issue #81): same contract as
// lerp_hsl_unclamped — ease_oklab feeds the raw eased factor.

template <typename Color>
Color lerp_oklab_unclamped(const Color& a, const Color& b, float t) {
  using namespace conversion;
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;

  auto ca = color_cast<core::oklab_t>(a);
  auto cb = color_cast<core::oklab_t>(b);

  const float aa = details::premul_weight(a);
  const float ab = details::premul_weight(b);
  const float a_mid = details::lerp_f(aa, ab, t);

  core::oklab_t mid{details::lerp_premul(ca.template get_index<0>(), aa, cb.template get_index<0>(), ab, t, a_mid),
                    details::lerp_premul(ca.template get_index<1>(), aa, cb.template get_index<1>(), ab, t, a_mid),
                    details::lerp_premul(ca.template get_index<2>(), aa, cb.template get_index<2>(), ab, t, a_mid)};

  return details::lerp_result<Color>(mid, a_mid);
}

}  // namespace details

/** @brief OkLab interpolation (perceptually uniform); channels premultiplied (issue #80). */
template <typename Color>
Color lerp_oklab(const Color& a, const Color& b, float t) {
  return details::lerp_oklab_unclamped(a, b, std::clamp(t, 0.0f, 1.0f));
}

namespace details {

// Unclamped OkLCH blend core (issue #81): same contract as
// lerp_hsl_unclamped — ease_oklch feeds the raw eased factor.

template <typename Color>
Color lerp_oklch_unclamped(const Color& a, const Color& b, float t) {
  using namespace conversion;
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;

  auto ca = color_cast<core::oklch_t>(a);
  auto cb = color_cast<core::oklch_t>(b);

  float hue_a = ca.template get_index<2>();
  float hue_b = cb.template get_index<2>();
  details::substitute_powerless_hue(ca, cb, hue_a, hue_b);  // C ≈ 0 → hue missing (issue #51)

  const float aa = details::premul_weight(a);
  const float ab = details::premul_weight(b);
  const float a_mid = details::lerp_f(aa, ab, t);

  core::oklch_t mid{details::lerp_premul(ca.template get_index<0>(), aa, cb.template get_index<0>(), ab, t, a_mid),
                    details::lerp_premul(ca.template get_index<1>(), aa, cb.template get_index<1>(), ab, t, a_mid),
                    details::lerp_angle_deg(hue_a, hue_b, t)};

  return details::lerp_result<Color>(mid, a_mid);
}

}  // namespace details

/** @brief OkLCH interpolation: shortest-arc hue; L, C premultiplied (issue #80). */
template <typename Color>
Color lerp_oklch(const Color& a, const Color& b, float t) {
  return details::lerp_oklch_unclamped(a, b, std::clamp(t, 0.0f, 1.0f));
}

/** @brief CIELAB interpolation; channels premultiplied (issue #80). */
template <typename Color>
Color lerp_lab(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;

  auto ca = color_cast<core::cielab_t>(a);
  auto cb = color_cast<core::cielab_t>(b);

  const float aa = details::premul_weight(a);
  const float ab = details::premul_weight(b);
  const float a_mid = details::lerp_f(aa, ab, t);

  core::cielab_t mid{details::lerp_premul(ca.template get_index<0>(), aa, cb.template get_index<0>(), ab, t, a_mid),
                     details::lerp_premul(ca.template get_index<1>(), aa, cb.template get_index<1>(), ab, t, a_mid),
                     details::lerp_premul(ca.template get_index<2>(), aa, cb.template get_index<2>(), ab, t, a_mid)};

  return details::lerp_result<Color>(mid, a_mid);
}

/** @brief CIELCH interpolation: shortest-arc hue; L*, C* premultiplied (issue #80). */
template <typename Color>
Color lerp_lch(const Color& a, const Color& b, float t) {
  using namespace conversion;
  t = std::clamp(t, 0.0f, 1.0f);
  if (t == 0.0f) return a;
  if (t == 1.0f) return b;

  auto ca = color_cast<core::cielch_t>(a);
  auto cb = color_cast<core::cielch_t>(b);

  float hue_a = ca.template get_index<2>();
  float hue_b = cb.template get_index<2>();
  details::substitute_powerless_hue(ca, cb, hue_a, hue_b);  // C* ≈ 0 → hue missing (issue #51)

  const float aa = details::premul_weight(a);
  const float ab = details::premul_weight(b);
  const float a_mid = details::lerp_f(aa, ab, t);

  core::cielch_t mid{details::lerp_premul(ca.template get_index<0>(), aa, cb.template get_index<0>(), ab, t, a_mid),
                     details::lerp_premul(ca.template get_index<1>(), aa, cb.template get_index<1>(), ab, t, a_mid),
                     details::lerp_angle_deg(hue_a, hue_b, t)};

  return details::lerp_result<Color>(mid, a_mid);
}

}  // namespace colorcpp::operations::interpolate
