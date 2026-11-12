/**
 * @file details.hpp
 * @brief Internal implementation details for gradient generation.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gradient/easing.hpp>
#include <colorcpp/algorithms/gradient/stops.hpp>
#include <colorcpp/operations/interpolate/lerp.hpp>
#include <stdexcept>
#include <utility>

namespace colorcpp::algorithms::gradient::details {

template <typename Color>
using gradient_position_t = typename color_stops<Color>::position_type;

/**
 * @brief Find the two stops that bracket a given position.
 * @tparam Color Color type.
 * @param stops The color stops.
 * @param position The position to find stops for.
 * @return Pair of indices (left, right) into the stops array.
 */
template <typename Color>
std::pair<std::size_t, std::size_t> find_bracketing_stops(const color_stops<Color>& stops,
                                                          gradient_position_t<Color> position) {
  if (stops.empty()) {
    throw std::invalid_argument("colorcpp: gradient has no stops");
  }

  // Clamp position to [0, 1]
  position =
      std::clamp(position, static_cast<gradient_position_t<Color>>(0), static_cast<gradient_position_t<Color>>(1));

  if (position <= stops.front().position) {
    return {0, 0};
  }

  if (position >= stops.back().position) {
    return {stops.size() - 1, stops.size() - 1};
  }

  auto it = std::lower_bound(stops.begin(), stops.end(), position,
                             [](const typename color_stops<Color>::stop_type& stop, gradient_position_t<Color> value) {
                               return stop.position < value;
                             });

  if (it == stops.end()) {
    return {stops.size() - 1, stops.size() - 1};
  }

  const auto right = static_cast<std::size_t>(std::distance(stops.begin(), it));
  if (it->position == position) {
    return {right, right};
  }

  return {right - 1, right};
}

/**
 * @brief Calculate local t value between two stops.
 * @tparam Color Color type.
 * @param stops The color stops.
 * @param left Index of left stop.
 * @param right Index of right stop.
 * @param position Global position.
 * @return Local t value in [0, 1].
 */
template <typename Color>
gradient_position_t<Color> calculate_local_t(const color_stops<Color>& stops, std::size_t left, std::size_t right,
                                             gradient_position_t<Color> position) {
  if (left == right) {
    return static_cast<gradient_position_t<Color>>(0);
  }

  auto left_pos = stops[left].position;
  auto right_pos = stops[right].position;

  if (right_pos == left_pos) {
    return static_cast<gradient_position_t<Color>>(0);
  }

  return (position - left_pos) / (right_pos - left_pos);
}

/**
 * @brief Interpolate two colors at an eased factor WITHOUT re-clamping t.
 *
 * The public interpolate::lerp clamps t to [0, 1] (NaN -> 0, issue #82) so
 * callers cannot accidentally extrapolate. Inside the gradient pipeline the
 * factor is the output of an easing function, and cubic_bezier easings with
 * y1/y2 outside [0, 1] overshoot on purpose (W3C css-easing-1); clamping
 * here would erase that overshoot a third time (issue #106). NaN still maps
 * to factor 0.
 */
template <typename Color>
Color lerp_eased(const Color& a, const Color& b, float t) {
  if (std::isnan(t)) {
    t = 0.0f;
  }
  return operations::interpolate::details::lerp_channels(a, b, t);
}

/**
 * @brief Clamp a value to [0, 1] range.
 * @tparam T Value type.
 * @param value Value to clamp.
 * @return Clamped value.
 */
template <typename T>
constexpr T clamp_01(T value) noexcept {
  return std::clamp(value, static_cast<T>(0), static_cast<T>(1));
}

/**
 * @brief Check if a value is finite (not NaN or Inf).
 * @tparam T Value type.
 * @param value Value to check.
 * @return True if finite.
 */
template <typename T>
bool is_finite(T value) noexcept {
  return std::isfinite(value);
}

/**
 * @brief Sample a gradient at a given position.
 * @tparam Color Color type.
 * @tparam Interpolator Interpolator function type.
 * @param stops The color stops.
 * @param position The position to sample at [0, 1].
 * @param interpolator Function to interpolate between two colors.
 * @param easing Easing function to apply.
 * @return The interpolated color.
 */
template <typename Color, typename Interpolator>
Color sample_gradient(
    const color_stops<Color>& stops, gradient_position_t<Color> position, Interpolator&& interpolator,
    easing::easing_function<gradient_position_t<Color>> easing = easing::linear<gradient_position_t<Color>>) {
  if (stops.empty()) {
    throw std::invalid_argument("colorcpp: gradient has no stops");
  }

  auto [left, right] = find_bracketing_stops(stops, position);

  if (left == right) {
    return stops[left].color;
  }

  auto local_t = calculate_local_t(stops, left, right, position);
  // Eased value is deliberately NOT clamped: easings such as cubic_bezier
  // with y1/y2 outside [0, 1] overshoot on purpose (W3C css-easing-1), and
  // a second clamp here erased that signal (issue #106).
  auto eased_t = easing(local_t);

  return interpolator(stops[left].color, stops[right].color, eased_t);
}

/**
 * @brief Check that every channel of a color is finite (not NaN or Inf).
 * @tparam Color Color type.
 * @param c Color to check.
 * @return True if all channels are finite.
 */
template <typename Color, std::size_t... Is>
bool is_finite_color_impl(const Color& c, std::index_sequence<Is...>) {
  return (is_finite(c.template get_index<Is>()) && ...);
}

template <typename Color>
bool is_finite_color(const Color& c) {
  return is_finite_color_impl(c, std::make_index_sequence<Color::channels>{});
}

/**
 * @brief Validate that stops are properly formed.
 * @tparam Color Color type.
 * @param stops The color stops to validate.
 * @return True if valid, throws exception if invalid.
 */
template <typename Color>
bool validate_stops(const color_stops<Color>& stops) {
  if (stops.empty()) {
    throw std::invalid_argument("colorcpp: gradient must have at least one stop");
  }

  for (const auto& stop : stops) {
    // Position is validated by the color_stop constructor, but a non-const
    // operator[] reference can mutate it afterwards — re-check here.
    if (stop.position < 0.0f || stop.position > 1.0f) {
      throw std::out_of_range("colorcpp: stop position must be in [0, 1]");
    }

    if (!is_finite_color(stop.color)) {
      throw std::invalid_argument("colorcpp: stop color contains non-finite values (NaN or Inf)");
    }
  }

  return true;
}

}  // namespace colorcpp::algorithms::gradient::details
