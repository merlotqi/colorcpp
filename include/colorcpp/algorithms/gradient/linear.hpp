/**
 * @file linear.hpp
 * @brief Linear gradient generation.
 *
 * Linear gradients interpolate colors along a straight line.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gradient/details.hpp>
#include <colorcpp/algorithms/gradient/easing.hpp>
#include <colorcpp/algorithms/gradient/stops.hpp>
#include <colorcpp/operations/interpolate.hpp>
#include <vector>

namespace colorcpp::algorithms::gradient {

/**
 * @brief Linear gradient generator.
 * @tparam Color The color type.
 */
template <typename Color>
class linear_gradient {
 public:
  using color_type = Color;
  using value_type = typename Color::value_type;
  using stops_type = color_stops<Color>;
  using easing_type = easing::easing_function<value_type>;

  /**
   * @brief Construct a linear gradient from color stops.
   * @param stops The color stops.
   * @param easing Easing function (default: linear).
   */
  explicit linear_gradient(
      const stops_type& stops, easing_type easing = [](value_type t) { return easing::linear(t); })
      : stops_(stops), easing_(std::move(easing)) {
    details::validate_stops(stops_);
  }

  /**
   * @brief Construct a linear gradient from an initializer list.
   * @param stops Initializer list of color stops.
   * @param easing Easing function (default: linear).
   */
  linear_gradient(
      std::initializer_list<color_stop<Color>> stops,
      easing_type easing = [](value_type t) { return easing::linear(t); })
      : stops_(stops), easing_(std::move(easing)) {
    details::validate_stops(stops_);
  }

  /**
   * @brief Sample the gradient at a given position.
   * @param t Position in [0, 1].
   * @return The interpolated color.
   */
  Color sample(value_type t) const {
    auto interpolator = [](const Color& a, const Color& b, value_type t) {
      return operations::interpolate::lerp(a, b, t);
    };
    return details::sample_gradient(stops_, details::clamp_01(t + phase_offset_), interpolator, easing_);
  }

  /**
   * @brief Generate a palette of colors.
   * @param count Number of colors to generate.
   * @return Vector of colors.
   */
  std::vector<Color> palette(std::size_t count) const {
    if (count == 0) {
      return {};
    }
    if (count == 1) {
      return {sample(static_cast<value_type>(0.5))};
    }
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
      auto t = static_cast<value_type>(i) / static_cast<value_type>(count - 1);
      out.push_back(sample(t));
    }
    return out;
  }

  /**
   * @brief Get the color stops.
   * @return Reference to the color stops.
   */
  const stops_type& stops() const noexcept { return stops_; }

  /**
   * @brief Get the easing function.
   * @return Reference to the easing function.
   */
  const easing_type& easing() const noexcept { return easing_; }

  /**
   * @brief Set the easing function.
   * @param easing New easing function.
   */
  void set_easing(easing_type easing) { easing_ = std::move(easing); }

  /**
   * @brief Create a reversed copy of this gradient.
   * @return New gradient with reversed stops.
   */
  linear_gradient<Color> reverse() const {
    typename stops_type::container_type reversed_stops;
    reversed_stops.reserve(stops_.size());
    for (const auto& stop : stops_) {
      reversed_stops.emplace_back(static_cast<value_type>(1) - stop.position, stop.color);
    }
    return linear_gradient<Color>(stops_type(reversed_stops), easing_);
  }

  /**
   * @brief Blend this gradient with another gradient.
   *
   * At each parameter @c u in [0, 1], the result color is
   * @c lerp(this->sample(u), other.sample(u), t). Rebuilt as evenly spaced stops (not a merged stop list).
   */
  linear_gradient<Color> blend(const linear_gradient<Color>& other, value_type t) const {
    const std::size_t n = std::max<std::size_t>(3, std::max(stops_.size(), other.stops_.size()) * 8);
    typename stops_type::container_type blended_stops;
    blended_stops.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      value_type u = static_cast<value_type>(i) / static_cast<value_type>(n - 1);
      Color c = operations::interpolate::lerp(sample(u), other.sample(u), t);
      blended_stops.emplace_back(u, c);
    }
    return linear_gradient<Color>(stops_type(blended_stops), easing_);
  }

  /**
   * @brief Concatenate: first half maps to @c *this, second half to @c other.
   *
   * At @c u < 0.5 uses @c sample(u/0.5); at @c u > 0.5 uses @c other.sample((u-0.5)/0.5);
   * at @c u == 0.5 uses the start of @p other (right-continuous seam).
   */
  linear_gradient<Color> concat(const linear_gradient<Color>& other) const {
    // Odd count so i/(n-1) hits 0.5 exactly at the seam (pure second segment start).
    std::size_t n = std::max<std::size_t>(3, std::max(stops_.size(), other.stops_.size()) * 8);
    if (n % 2 == 0) {
      ++n;
    }
    constexpr value_type half = static_cast<value_type>(0.5);
    typename stops_type::container_type concat_stops;
    concat_stops.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      value_type u = static_cast<value_type>(i) / static_cast<value_type>(n - 1);
      Color c;
      if (u < half) {
        c = sample(u / half);
      } else if (u > half) {
        c = other.sample((u - half) / half);
      } else {
        c = other.sample(static_cast<value_type>(0));
      }
      concat_stops.emplace_back(u, c);
    }
    return linear_gradient<Color>(stops_type(concat_stops), easing_);
  }

  /**
   * @brief Scale the gradient positions by a factor.
   * @param factor Scale factor (> 1 compresses, < 1 stretches).
   * @return New scaled gradient.
   */
  linear_gradient<Color> scale(value_type factor) const {
    if (factor <= static_cast<value_type>(0)) {
      throw std::invalid_argument("colorcpp: scale factor must be positive");
    }

    typename stops_type::container_type scaled_stops;
    scaled_stops.reserve(stops_.size());

    for (const auto& stop : stops_) {
      value_type new_pos = stop.position / factor;
      new_pos = std::clamp(new_pos, static_cast<value_type>(0), static_cast<value_type>(1));
      scaled_stops.emplace_back(new_pos, stop.color);
    }

    linear_gradient<Color> g(stops_type(scaled_stops), easing_);
    g.phase_offset_ = phase_offset_;
    return g;
  }

  /**
   * @brief Shift where the gradient is sampled along [0, 1].
   *
   * Equivalent to sampling the same stops at @c clamp_01(t + delta) instead of @c t
   * (clamped linear parameter, not moving stop positions). Positive @p delta shifts
   * the ramp toward lower @c t (e.g. offset(0.5) then sample(0) matches sample(0.5) on the original).
   *
   * @param delta Phase offset added to every sample position before clamping to [0, 1].
   * @return New gradient with the same stops and accumulated phase offset.
   */
  linear_gradient<Color> offset(value_type delta) const {
    linear_gradient<Color> g(stops_, easing_);
    g.phase_offset_ = phase_offset_ + delta;
    return g;
  }

  /**
   * @brief Quantize the gradient to a discrete number of levels.
   * @param levels Number of discrete color levels.
   * @return New quantized gradient.
   */
  linear_gradient<Color> quantize(std::size_t levels) const {
    if (levels == 0) {
      throw std::invalid_argument("colorcpp: quantize levels must be at least 1");
    }

    if (levels == 1) {
      // Return a gradient with a single stop at 0.5
      typename stops_type::container_type quantized_stops;
      quantized_stops.emplace_back(static_cast<value_type>(0.5), sample(static_cast<value_type>(0.5)));
      return linear_gradient<Color>(stops_type(quantized_stops), easing_);
    }

    typename stops_type::container_type quantized_stops;
    quantized_stops.reserve(levels);

    for (std::size_t i = 0; i < levels; ++i) {
      value_type pos = static_cast<value_type>(i) / static_cast<value_type>(levels - 1);
      quantized_stops.emplace_back(pos, sample(pos));
    }

    return linear_gradient<Color>(stops_type(quantized_stops), easing_);
  }

 private:
  stops_type stops_;
  easing_type easing_;
  /** Added to sample @c t before clamping to [0, 1]; see offset(). */
  value_type phase_offset_{};
};

/**
 * @brief Create a linear gradient from color stops.
 * @tparam Color Color type.
 * @param stops The color stops.
 * @param easing Easing function (default: linear).
 * @return Linear gradient object.
 */
template <typename Color>
linear_gradient<Color> linear(
    const color_stops<Color>& stops, easing::easing_function<typename Color::value_type> easing =
                                         [](typename Color::value_type t) { return easing::linear(t); }) {
  return linear_gradient<Color>(stops, std::move(easing));
}

/**
 * @brief Create a linear gradient from an initializer list.
 * @tparam Color Color type.
 * @param stops Initializer list of color stops.
 * @param easing Easing function (default: linear).
 * @return Linear gradient object.
 */
template <typename Color>
linear_gradient<Color> linear(
    std::initializer_list<color_stop<Color>> stops,
    easing::easing_function<typename Color::value_type> easing = [](typename Color::value_type t) {
      return easing::linear(t);
    }) {
  return linear_gradient<Color>(stops, std::move(easing));
}

}  // namespace colorcpp::algorithms::gradient
