/**
 * @file angular.hpp
 * @brief Angular gradient generation.
 *
 * Angular gradients interpolate colors around a center point.
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
 * @brief Angular gradient generator.
 * @tparam Color The color type.
 */
template <typename Color>
class angular_gradient {
 public:
  using color_type = Color;
  using stops_type = color_stops<Color>;
  using position_type = typename stops_type::position_type;
  using easing_type = easing::easing_function<position_type>;

  /**
   * @brief Construct an angular gradient from color stops.
   * @param stops The color stops.
   * @param easing Easing function (default: linear).
   */
  explicit angular_gradient(const stops_type& stops, easing_type easing = easing::linear<position_type>)
      : stops_(stops), easing_(std::move(easing)) {
    details::validate_stops(stops_);
  }

  /**
   * @brief Construct an angular gradient from an initializer list.
   * @param stops Initializer list of color stops.
   * @param easing Easing function (default: linear).
   */
  angular_gradient(std::initializer_list<color_stop<Color>> stops, easing_type easing = easing::linear<position_type>)
      : stops_(stops), easing_(std::move(easing)) {
    details::validate_stops(stops_);
  }

  /**
   * @brief Sample the gradient at a given angle.
   * @param angle Normalized angle in [0, 1] using CSS/SVG-style orientation
   *        (0 = top, 0.25 = right, 0.5 = bottom, 0.75 = left).
   * @return The interpolated color.
   */
  Color sample(position_type angle) const {
    angle = angle + phase_offset_;
    // Normalize angle to [0, 1)
    angle = angle - std::floor(angle);
    auto interpolator = [](const Color& a, const Color& b, position_type t) {
      return operations::interpolate::lerp(a, b, t);
    };
    return details::sample_gradient(stops_, angle, interpolator, easing_);
  }

  /**
   * @brief Convert a 2D point into the normalized angular parameter.
   * @param x Normalized x coordinate in [0, 1].
   * @param y Normalized y coordinate in [0, 1].
   * @return Normalized angle in [0, 1] using CSS/SVG-style orientation.
   */
  position_type position_at(float x, float y) const noexcept {
    constexpr position_type pi = static_cast<position_type>(3.14159265358979323846f);
    position_type dx = static_cast<position_type>(x) - 0.5f;
    position_type dy = static_cast<position_type>(y) - 0.5f;

    position_type angle = std::atan2(dx, -dy) / (2.0f * pi);
    if (angle < 0.0f) {
      angle += 1.0f;
    }
    return angle;
  }

  /**
   * @brief Sample the gradient at a given 2D position.
   * @param x Normalized x coordinate in [0, 1].
   * @param y Normalized y coordinate in [0, 1].
   * @return The interpolated color.
   */
  Color sample_at(float x, float y) const {
    return sample(position_at(x, y));
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
      return {sample(0.5f)};
    }
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
      auto t = static_cast<float>(i) / static_cast<float>(count - 1);
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
  angular_gradient<Color> reverse() const {
    typename stops_type::container_type reversed_stops;
    reversed_stops.reserve(stops_.size());
    for (const auto& stop : stops_) {
      reversed_stops.emplace_back(1.0f - stop.position, stop.color);
    }
    return angular_gradient<Color>(stops_type(reversed_stops), easing_);
  }

  /**
   * @brief Blend this gradient with another gradient.
   * @param other Other gradient to blend with.
   * @param t Blend factor in [0, 1] (0 = this gradient, 1 = other gradient).
   * @return New blended gradient.
   */
  angular_gradient<Color> blend(const angular_gradient<Color>& other, float t) const {
    const std::size_t n = std::max<std::size_t>(3, std::max(stops_.size(), other.stops_.size()) * 8);
    typename stops_type::container_type blended_stops;
    blended_stops.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      float u = static_cast<float>(i) / static_cast<float>(n - 1);
      Color c = operations::interpolate::lerp(sample(u), other.sample(u), t);
      blended_stops.emplace_back(u, c);
    }
    return angular_gradient<Color>(stops_type(blended_stops), easing_);
  }

  /**
   * @brief Concatenate: first half of the circle to @c *this, second half to @c other.
   */
  angular_gradient<Color> concat(const angular_gradient<Color>& other) const {
    std::size_t n = std::max<std::size_t>(3, std::max(stops_.size(), other.stops_.size()) * 8);
    if (n % 2 == 0) {
      ++n;
    }
    constexpr float half = 0.5f;
    typename stops_type::container_type concat_stops;
    concat_stops.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      float u = static_cast<float>(i) / static_cast<float>(n - 1);
      Color c;
      if (u < half) {
        c = sample(u / half);
      } else if (u > half) {
        c = other.sample((u - half) / half);
      } else {
        c = operations::interpolate::lerp(sample(1.0f), other.sample(0.0f), 0.5f);
      }
      concat_stops.emplace_back(u, c);
    }
    return angular_gradient<Color>(stops_type(concat_stops), easing_);
  }

  /**
   * @brief Scale the angular stop positions by a factor.
   * @param factor Scale factor (> 1 compresses, < 1 stretches).
   */
  angular_gradient<Color> scale(float factor) const {
    if (factor <= 0.0f) {
      throw std::invalid_argument("colorcpp: scale factor must be positive");
    }

    typename stops_type::container_type scaled_stops;
    scaled_stops.reserve(stops_.size());

    for (const auto& stop : stops_) {
      float new_pos = stop.position / factor;
      new_pos = std::clamp(new_pos, 0.0f, 1.0f);
      scaled_stops.emplace_back(new_pos, stop.color);
    }

    angular_gradient<Color> g(stops_type(scaled_stops), easing_);
    g.phase_offset_ = phase_offset_;
    return g;
  }

  /**
   * @brief Rotate sampling around the circle: angle is wrapped after adding @p delta (mod 1).
   */
  angular_gradient<Color> offset(float delta) const {
    angular_gradient<Color> g(stops_, easing_);
    g.phase_offset_ = phase_offset_ + delta;
    return g;
  }

  /**
   * @brief Quantize the gradient to a discrete number of levels.
   * @param levels Number of discrete color levels.
   * @return New quantized gradient.
   */
  angular_gradient<Color> quantize(std::size_t levels) const {
    if (levels == 0) {
      throw std::invalid_argument("colorcpp: quantize levels must be at least 1");
    }

    if (levels == 1) {
      // Return a gradient with a single stop at 0.5
      typename stops_type::container_type quantized_stops;
      quantized_stops.emplace_back(0.5f, sample(0.5f));
      return angular_gradient<Color>(stops_type(quantized_stops), easing_);
    }

    typename stops_type::container_type quantized_stops;
    quantized_stops.reserve(levels);

    for (std::size_t i = 0; i < levels; ++i) {
      float pos = static_cast<float>(i) / static_cast<float>(levels - 1);
      quantized_stops.emplace_back(pos, sample(pos));
    }

    return angular_gradient<Color>(stops_type(quantized_stops), easing_);
  }

 private:
  stops_type stops_;
  easing_type easing_;
  position_type phase_offset_{};
};

/**
 * @brief Create an angular gradient from color stops.
 * @tparam Color Color type.
 * @param stops The color stops.
 * @param easing Easing function (default: linear).
 * @return Angular gradient object.
 */
template <typename Color>
angular_gradient<Color> angular(
    const color_stops<Color>& stops,
    typename angular_gradient<Color>::easing_type easing = [](typename angular_gradient<Color>::position_type t) {
      return easing::linear(t);
    }) {
  return angular_gradient<Color>(stops, std::move(easing));
}

/**
 * @brief Create an angular gradient from an initializer list.
 * @tparam Color Color type.
 * @param stops Initializer list of color stops.
 * @param easing Easing function (default: linear).
 * @return Angular gradient object.
 */
template <typename Color>
angular_gradient<Color> angular(
    std::initializer_list<color_stop<Color>> stops,
    typename angular_gradient<Color>::easing_type easing = [](typename angular_gradient<Color>::position_type t) {
      return easing::linear(t);
    }) {
  return angular_gradient<Color>(stops, std::move(easing));
}

}  // namespace colorcpp::algorithms::gradient
