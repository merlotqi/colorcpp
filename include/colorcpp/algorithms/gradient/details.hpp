/**
 * @file details.hpp
 * @brief Internal implementation details for gradient generation.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gradient/easing.hpp>
#include <colorcpp/algorithms/gradient/stops.hpp>
#include <stdexcept>
#include <vector>

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
  auto eased_t = easing(local_t);

  // Clamp eased value to [0, 1] to handle easing functions that may overshoot
  eased_t = clamp_01(eased_t);

  return interpolator(stops[left].color, stops[right].color, eased_t);
}

/**
 * @brief Generate a palette of colors from a gradient.
 * @tparam Color Color type.
 * @tparam Interpolator Interpolator function type.
 * @param stops The color stops.
 * @param count Number of colors to generate.
 * @param interpolator Function to interpolate between two colors.
 * @param easing Easing function to apply.
 * @return Vector of colors.
 */
template <typename Color, typename Interpolator>
std::vector<Color> generate_palette(
    const color_stops<Color>& stops, std::size_t count, Interpolator&& interpolator,
    easing::easing_function<gradient_position_t<Color>> easing = easing::linear<gradient_position_t<Color>>) {
  if (count == 0) {
    return {};
  }

  if (count == 1) {
    return {sample_gradient(stops, static_cast<gradient_position_t<Color>>(0.5),
                            std::forward<Interpolator>(interpolator), easing)};
  }

  std::vector<Color> palette;
  palette.reserve(count);

  for (std::size_t i = 0; i < count; ++i) {
    auto t = static_cast<gradient_position_t<Color>>(i) / static_cast<gradient_position_t<Color>>(count - 1);
    palette.push_back(sample_gradient(stops, t, std::forward<Interpolator>(interpolator), easing));
  }

  return palette;
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

  // Check that positions are in [0, 1]
  for (const auto& stop : stops) {
    if (stop.position < 0.0f || stop.position > 1.0f) {
      throw std::out_of_range("colorcpp: stop position must be in [0, 1]");
    }

    // Check that color values are finite (not NaN or Inf)
    // We check the first channel value as a representative
    if (!is_finite(stop.color.template get_index<0>())) {
      throw std::invalid_argument("colorcpp: stop color contains non-finite values (NaN or Inf)");
    }
  }

  return true;
}

}  // namespace colorcpp::algorithms::gradient::details
