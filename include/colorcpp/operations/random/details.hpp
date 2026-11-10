/**
 * @file details.hpp
 * @brief Helper traits for random color generation.
 *
 * Used by @ref basic_random_generator and related templates; see @ref random.hpp for thread-safety and gamut notes.
 */

#pragma once

#include <colorcpp/algorithms/gamut/preserve.hpp>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/luv.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <cstddef>
#include <tuple>
#include <type_traits>

namespace colorcpp::operations::random::details {

/**
 * @brief Portable engine → [0, 1) mapping (issue #96).
 *
 * std::uniform_real_distribution leaves the engine→value mapping
 * implementation-defined: libstdc++, libc++ and MSVC disagree, so the same
 * seed produced different colors on different standard libraries. This maps
 * the engine's raw draw linearly over its full range in double - x = (u -
 * min) / range - which is exact for power-of-two engines like mt19937.
 */
template <typename Engine>
double portable_unit(Engine& rng) {
  const uint64_t lo = static_cast<uint64_t>(Engine::min());
  const uint64_t hi = static_cast<uint64_t>(Engine::max());
  const uint64_t u = static_cast<uint64_t>(rng()) - lo;
  const uint64_t range = hi - lo + 1;  // 0 when the engine spans all 2^64 values
  if (range == 0) return static_cast<double>(u) * 0x1.0p-64;
  return static_cast<double>(u) / static_cast<double>(range);
}

/**
 * @brief Portable uniform real in [min, max] (issue #96).
 *
 * Spec: min + (max - min) * x with x from @ref portable_unit, the whole
 * expression in double, converted to @p T once. (The std distributions
 * compute in the result type and their rounding differs across standard
 * libraries.)
 */
template <typename T, typename Engine>
T portable_real(Engine& rng, T min, T max) {
  const double x = portable_unit(rng);
  return static_cast<T>(static_cast<double>(min) + (static_cast<double>(max) - static_cast<double>(min)) * x);
}

/**
 * @brief Portable uniform integer in [min, max] (issue #96).
 *
 * Spec: u % span over the engine's range with the biasing tail rejected
 * (the std distribution's engine→value mapping is implementation-defined).
 * Channel bounds are non-negative in this library (uint8/uint16/...), which
 * the uint64 arithmetic below assumes.
 */
template <typename T, typename Engine>
T portable_int(Engine& rng, T min, T max) {
  static_assert(std::is_integral_v<T>, "portable_int requires an integral type");
  const uint64_t span = static_cast<uint64_t>(max) - static_cast<uint64_t>(min) + 1;
  const uint64_t lo = static_cast<uint64_t>(Engine::min());
  const uint64_t hi = static_cast<uint64_t>(Engine::max());
  const uint64_t range = hi - lo + 1;
  uint64_t u = static_cast<uint64_t>(rng()) - lo;
  if (range != 0 && span < range) {
    // Reject the tail beyond the largest multiple of span so u % span stays
    // unbiased. (span >= range cannot happen for our engines/channels; fall
    // through to plain modulo rather than rejecting every draw.)
    const uint64_t limit = range - (range % span);
    while (u >= limit) u = static_cast<uint64_t>(rng()) - lo;
  }
  return static_cast<T>(static_cast<uint64_t>(min) + u % span);
}

/**
 * @brief Finalize a sampled OkLab candidate into Color under the contract
 *        shared by luminance_generator and contrast_generator.
 *
 * With @p gamut_map the candidate is clipped to sRGB via the lightness-
 * preserving mapper BEFORE the cast (issue #61: color_cast clamps first, so
 * mapping afterwards would see an in-gamut color and silently discard the
 * sampled lightness). Without it the raw cast is tried first and the mapper
 * runs only when the received OkLab L lands outside [@p l_min, @p l_max]
 * (issue #95: the raw cast clamps RGB-family targets and moves their L by
 * up to ~0.09 - the bound is unconditional, gamut_map only opts into
 * always-on clipping); wide-gamut targets never clamp, so no fallback
 * triggers there.
 */
template <typename Color>
Color finalize_oklab_candidate(const core::oklab_t& lab, float l_min, float l_max, bool gamut_map) {
  if (gamut_map) {
    return conversion::color_cast<Color>(algorithms::gamut::gamut_clip_preserve_lightness(lab));
  }
  Color out = conversion::color_cast<Color>(lab);
  const float out_L = conversion::color_cast<core::oklab_t>(out).l();
  if (out_L < l_min || out_L > l_max) {
    out = conversion::color_cast<Color>(algorithms::gamut::gamut_clip_preserve_lightness(lab));
  }
  return out;
}

namespace impl {

// SFINAE helper to detect if a color type has an h() method (hue channel)
template <typename Color, typename = void>
struct has_hue_method : std::false_type {};

template <typename Color>
struct has_hue_method<Color, std::void_t<decltype(std::declval<const Color&>().h())>> : std::true_type {};

}  // namespace impl

/**
 * @brief Per-channel sampling bounds for random generation.
 *
 * Defaults to the channel's own [min, max]. Device-independent spaces override
 * it below: their channel ranges are *nominal* — deliberately widened so
 * conversions never clip (issue #67) — which makes them useless as sampling
 * boxes (most of Lab's ±400 a*, e.g., lies outside every display gamut).
 * Instead, each override samples that channel's perceptual working range.
 */
template <typename Channel>
struct sample_channel {
  static constexpr float min = static_cast<float>(Channel::min);
  static constexpr float max = static_cast<float>(Channel::max);
};

// OkLab / OkLCH: L [0, 1], a/b ±0.5, C [0, 0.4] (CSS relative-syntax 100%
// references — well inside every supported gamut).
template <>
struct sample_channel<core::oklab::channel::okl> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};
template <>
struct sample_channel<core::oklab::channel::oka> {
  static constexpr float min = -0.5f;
  static constexpr float max = 0.5f;
};
template <>
struct sample_channel<core::oklab::channel::okb> {
  static constexpr float min = -0.5f;
  static constexpr float max = 0.5f;
};
template <>
struct sample_channel<core::oklab::channel::okc> {
  static constexpr float min = 0.0f;
  static constexpr float max = 0.4f;
};

// CIELAB / CIELCH: L* [0, 100], a*/b* ±128, C* [0, 200] — the pre-#56
// perceptual box, before the nominal ranges widened to ±400 / 400.
template <>
struct sample_channel<core::lab::channel::lab_l> {
  static constexpr float min = 0.0f;
  static constexpr float max = 100.0f;
};
template <>
struct sample_channel<core::lab::channel::lab_a> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};
template <>
struct sample_channel<core::lab::channel::lab_b> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};
template <>
struct sample_channel<core::lab::channel::lab_c> {
  static constexpr float min = 0.0f;
  static constexpr float max = 200.0f;
};

// CIELUV: u*/v* ±128 (nominal ±400 since #56); L* [0, 100] already matches.
template <>
struct sample_channel<core::luv::channel::luv_u> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};
template <>
struct sample_channel<core::luv::channel::luv_v> {
  static constexpr float min = -128.0f;
  static constexpr float max = 128.0f;
};

// XYZ: nominal [-1, 2] (#56, keeps adaptation intermediates); the physical
// range of displayable colors is [0, ~1.09] — sample [0, 1] per axis.
template <>
struct sample_channel<core::xyz::channel::xyz_x> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};
template <>
struct sample_channel<core::xyz::channel::xyz_y> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};
template <>
struct sample_channel<core::xyz::channel::xyz_z> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

/**
 * @brief Color traits for random generation.
 * @tparam Color Color type.
 */
template <typename Color>
struct color_traits {
  using value_type = typename Color::value_type;
  using channels_tuple = typename Color::channels_tuple;
  static constexpr std::size_t size = Color::channels;

  // Detect hue channel by checking if h() method exists
  static constexpr bool has_hue = impl::has_hue_method<Color>::value;

  template <std::size_t I>
  static constexpr value_type min_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return channel_t::min;
  }

  template <std::size_t I>
  static constexpr value_type max_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return channel_t::max;
  }

  // Sampling bounds used by basic_random_generator: the channel range for
  // device spaces, the perceptual working range where sample_channel
  // overrides it (issue #67).
  template <std::size_t I>
  static constexpr value_type sample_min_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return static_cast<value_type>(sample_channel<channel_t>::min);
  }

  template <std::size_t I>
  static constexpr value_type sample_max_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return static_cast<value_type>(sample_channel<channel_t>::max);
  }

  static constexpr value_type hue_max() { return max_at<0>(); }
};

}  // namespace colorcpp::operations::random::details
