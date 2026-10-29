/**
 * @file perceptual.hpp
 * @brief Perceptual color comparison policy using selectable ΔE metrics.
 */

#pragma once

#include <colorcpp/algorithms/delta_e.hpp>
#include <tuple>
#include <type_traits>
#include <utility>

namespace colorcpp::operations::compare {

/**
 * @brief How alpha participates in a perceptual comparison.
 */
enum class alpha_mode {
  ignore,  ///< Alpha is irrelevant; compare colors only (ΔE metrics convert to non-alpha spaces).
  exact,   ///< Alpha must be exactly equal when both inputs carry an alpha channel.
};

namespace detail {

/** @brief Whether a color's channel tuple includes a straight alpha channel. */
template <typename Color, std::size_t... Is>
constexpr bool has_alpha_impl(std::index_sequence<Is...>) noexcept {
  return ((std::is_same_v<typename std::tuple_element_t<Is, typename Color::channels_tuple>::tag_type,
                          traits::alpha_tag>) ||
          ...);
}

template <typename Color>
inline constexpr bool color_has_alpha_v = has_alpha_impl<Color>(std::make_index_sequence<Color::channels>{});

/** @brief Alpha gate for @ref alpha_mode: exact requires equal alpha when both sides have it. */
template <typename ColorA, typename ColorB>
constexpr bool alpha_compatible(const ColorA& a, const ColorB& b, alpha_mode am) noexcept {
  if (am == alpha_mode::ignore) return true;
  if constexpr (!color_has_alpha_v<ColorA> || !color_has_alpha_v<ColorB>) {
    return true;  // nothing to compare on this side
  } else {
    return a.template get<traits::alpha_tag>() == b.template get<traits::alpha_tag>();
  }
}

}  // namespace detail

/**
 * @brief CIEDE2000 metric for @ref basic_perceptual_policy.
 */
struct de2000_metric {
  static constexpr float default_threshold = 1.0f;  ///< ~imperceptible on the ΔE2000 scale.
  template <typename ColorA, typename ColorB>
  static float distance(const ColorA& a, const ColorB& b, float kL, float kC, float kH) {
    return algorithms::delta_e::delta_e_2000(a, b, kL, kC, kH);
  }
};

/**
 * @brief OkLab Euclidean metric for @ref basic_perceptual_policy.
 *
 * Consistent with the library's gamut-mapping path, which uses @ref delta_e::delta_e_ok
 * and @ref delta_e::oklab_jnd_typical. The kL/kC/kH weights have no meaning here and
 * are ignored.
 */
struct oklab_metric {
  static constexpr float default_threshold = algorithms::delta_e::oklab_jnd_typical;  ///< typical JND ≈ 0.02.
  template <typename ColorA, typename ColorB>
  static float distance(const ColorA& a, const ColorB& b, float /*kL*/, float /*kC*/, float /*kH*/) {
    return algorithms::delta_e::delta_e_ok(a, b);
  }
};

/**
 * @brief Perceptual comparison policy over a selectable ΔE metric.
 *
 * Compares colors with @c Metric::distance <= threshold (inclusive boundary:
 * distance == threshold counts as equal). Alpha handling is explicit via
 * @ref alpha_mode (default: ignore — ΔE metrics convert to non-alpha spaces).
 * Mixed-type and cross-space inputs work as long as both convert via
 * @ref operations::conversion::color_cast. Not constexpr (ΔE math).
 *
 * @tparam Metric Distance metric providing @c distance and @c default_threshold.
 */
template <typename Metric = de2000_metric>
struct basic_perceptual_policy {
  /**
   * @brief Compare two colors perceptually.
   * @param threshold Inclusive distance threshold (default: Metric::default_threshold).
   * @param am Alpha participation (default: ignore).
   * @param kL Lightness weight (ΔE2000 only; ignored by metrics without weights).
   * @param kC Chroma weight (ΔE2000 only).
   * @param kH Hue weight (ΔE2000 only).
   * @return True if distance(a, b) <= threshold and alpha passes the mode gate.
   */
  template <typename ColorA, typename ColorB>
  static bool compare(const ColorA& a, const ColorB& b, float threshold = Metric::default_threshold,
                      alpha_mode am = alpha_mode::ignore, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f) {
    if (!detail::alpha_compatible(a, b, am)) return false;
    return Metric::distance(a, b, kL, kC, kH) <= threshold;
  }
};

/** @brief Default perceptual policy: CIEDE2000. */
using perceptual_policy = basic_perceptual_policy<de2000_metric>;

/** @brief Perceptual policy on OkLab Euclidean distance (JND ≈ 0.02 scale). */
using perceptual_oklab_policy = basic_perceptual_policy<oklab_metric>;

/**
 * @brief Compare two colors perceptually using CIEDE2000.
 * @param threshold Inclusive ΔE2000 threshold (default: 1.0).
 * @param am Alpha participation (default: ignore).
 * @param kL Lightness weight.
 * @param kC Chroma weight.
 * @param kH Hue weight.
 * @return True if ΔE2000 <= threshold and alpha passes the mode gate.
 */
template <typename ColorA, typename ColorB>
bool perceptual_equal_de2000(const ColorA& a, const ColorB& b, float threshold = de2000_metric::default_threshold,
                             alpha_mode am = alpha_mode::ignore, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f) {
  return perceptual_policy::compare(a, b, threshold, am, kL, kC, kH);
}

/**
 * @brief Compare two colors perceptually using CIEDE2000 (alias of @ref perceptual_equal_de2000).
 */
template <typename ColorA, typename ColorB>
bool perceptual_equal(const ColorA& a, const ColorB& b, float threshold = de2000_metric::default_threshold,
                      alpha_mode am = alpha_mode::ignore, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f) {
  return perceptual_equal_de2000(a, b, threshold, am, kL, kC, kH);
}

/**
 * @brief Compare two colors perceptually using OkLab Euclidean distance.
 * @param threshold Inclusive ΔE_OK threshold (default: oklab_jnd_typical ≈ 0.02).
 * @param am Alpha participation (default: ignore).
 * @return True if ΔE_OK <= threshold and alpha passes the mode gate.
 */
template <typename ColorA, typename ColorB>
bool perceptual_equal_ok(const ColorA& a, const ColorB& b, float threshold = oklab_metric::default_threshold,
                         alpha_mode am = alpha_mode::ignore) {
  return perceptual_oklab_policy::compare(a, b, threshold, am);
}

/**
 * @brief Perceptual distance (CIEDE2000) between two colors.
 *
 * The value behind the boolean: thresholds, bucketing, and closest-color
 * searches can rank candidates directly instead of re-deriving ΔE.
 *
 * @param a First color.
 * @param b Second color.
 * @param kL Lightness weight.
 * @param kC Chroma weight.
 * @param kH Hue weight.
 * @return ΔE2000 distance (no threshold, no alpha gate).
 */
template <typename ColorA, typename ColorB>
float perceptual_distance_de2000(const ColorA& a, const ColorB& b, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f) {
  return de2000_metric::distance(a, b, kL, kC, kH);
}

/**
 * @brief Perceptual distance (CIEDE2000) between two colors (alias of @ref perceptual_distance_de2000).
 */
template <typename ColorA, typename ColorB>
float perceptual_distance(const ColorA& a, const ColorB& b, float kL = 1.0f, float kC = 1.0f, float kH = 1.0f) {
  return perceptual_distance_de2000(a, b, kL, kC, kH);
}

/**
 * @brief Perceptual distance (OkLab Euclidean) between two colors.
 * @param a First color.
 * @param b Second color.
 * @return ΔE_OK distance on the JND ≈ 0.02 scale.
 */
template <typename ColorA, typename ColorB>
float perceptual_distance_ok(const ColorA& a, const ColorB& b) {
  return oklab_metric::distance(a, b, 1.0f, 1.0f, 1.0f);
}

}  // namespace colorcpp::operations::compare
