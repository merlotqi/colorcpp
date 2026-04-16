/**
 * @file stops.hpp
 * @brief Color stop definitions for gradient generation.
 *
 * A color stop represents a color at a specific position (0.0 to 1.0) along a gradient.
 */

#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace colorcpp::algorithms::gradient {

/**
 * @brief A color stop at a specific position along a gradient.
 * @tparam Color The color type (e.g., rgbf_t, oklab_t).
 */
template <typename Color>
struct color_stop {
  using color_type = Color;
  using value_type = float;

  value_type position;  // Position in [0, 1]
  Color color;          // Color at this position

  /**
   * @brief Construct a color stop.
   * @param pos Position in [0, 1].
   * @param c Color at this position.
   */
  constexpr color_stop(value_type pos, Color c) : position(pos), color(c) {
    if (pos < 0.0f || pos > 1.0f) {
      throw std::out_of_range("colorcpp: stop position must be in [0, 1]");
    }
  }
};

/**
 * @brief A collection of color stops, automatically sorted by position.
 * @tparam Color The color type.
 */
template <typename Color>
class color_stops {
 public:
  using stop_type = color_stop<Color>;
  using container_type = std::vector<stop_type>;
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;

  /**
   * @brief Construct from an initializer list of stops.
   * @param stops Initializer list of color stops.
   */
  color_stops(std::initializer_list<stop_type> stops) : stops_(stops) { sort(); }

  /**
   * @brief Construct from a vector of stops.
   * @param stops Vector of color stops.
   */
  explicit color_stops(const container_type& stops) : stops_(stops) { sort(); }

  /**
   * @brief Add a color stop.
   * @param stop The color stop to add.
   */
  void add(const stop_type& stop) {
    stops_.push_back(stop);
    sort();
  }

  /**
   * @brief Add a color stop at a position.
   * @param position Position in [0, 1].
   * @param color Color at this position.
   * @throws std::invalid_argument if position is out of range [0, 1].
   */
  void add(typename Color::value_type position, const Color& color) {
    if (position < 0.0f || position > 1.0f) {
      throw std::invalid_argument("colorcpp: stop position must be in [0, 1]");
    }
    stops_.emplace_back(position, color);
    sort();
  }

  /**
   * @brief Remove a color stop at the given index.
   * @param index Index of the stop to remove.
   * @throws std::out_of_range if index is invalid.
   */
  void remove(std::size_t index) {
    if (index >= stops_.size()) {
      throw std::out_of_range("colorcpp: stop index out of range");
    }
    stops_.erase(stops_.begin() + index);
  }

  /**
   * @brief Remove all stops with the given position (within epsilon).
   * @param position Position to remove.
   * @param epsilon Tolerance for position comparison (default: 1e-6).
   * @return Number of stops removed.
   */
  std::size_t remove_at(float position,
                        typename Color::value_type epsilon = static_cast<typename Color::value_type>(1e-6)) {
    auto it = std::remove_if(stops_.begin(), stops_.end(), [position, epsilon](const stop_type& stop) {
      return std::abs(stop.position - position) < epsilon;
    });
    std::size_t count = std::distance(it, stops_.end());
    stops_.erase(it, stops_.end());
    return count;
  }

  /**
   * @brief Update the color of a stop at the given index.
   * @param index Index of the stop to update.
   * @param color New color value.
   * @throws std::out_of_range if index is invalid.
   */
  void update_color(std::size_t index, const Color& color) {
    if (index >= stops_.size()) {
      throw std::out_of_range("colorcpp: stop index out of range");
    }
    stops_[index].color = color;
  }

  /**
   * @brief Update the position of a stop at the given index.
   * @param index Index of the stop to update.
   * @param position New position value.
   * @throws std::out_of_range if index is invalid.
   * @throws std::invalid_argument if position is out of range [0, 1].
   */
  void update_position(std::size_t index, float position) {
    if (index >= stops_.size()) {
      throw std::out_of_range("colorcpp: stop index out of range");
    }
    if (position < 0.0f || position > 1.0f) {
      throw std::invalid_argument("colorcpp: stop position must be in [0, 1]");
    }
    stops_[index].position = position;
    sort();
  }

  /**
   * @brief Get the number of stops.
   * @return Number of color stops.
   */
  std::size_t size() const noexcept { return stops_.size(); }

  /**
   * @brief Check if there are no stops.
   * @return True if empty.
   */
  bool empty() const noexcept { return stops_.empty(); }

  /**
   * @brief Clear all stops.
   */
  void clear() noexcept { stops_.clear(); }

  /**
   * @brief Get a stop by index.
   * @param index Index of the stop.
   * @return Reference to the stop.
   */
  const stop_type& operator[](std::size_t index) const { return stops_[index]; }

  /**
   * @brief Get a stop by index.
   * @param index Index of the stop.
   * @return Reference to the stop.
   */
  stop_type& operator[](std::size_t index) { return stops_[index]; }

  /**
   * @brief Get the first stop.
   * @return Reference to the first stop.
   */
  const stop_type& front() const { return stops_.front(); }

  /**
   * @brief Get the last stop.
   * @return Reference to the last stop.
   */
  const stop_type& back() const { return stops_.back(); }

  iterator begin() noexcept { return stops_.begin(); }
  iterator end() noexcept { return stops_.end(); }
  const_iterator begin() const noexcept { return stops_.begin(); }
  const_iterator end() const noexcept { return stops_.end(); }
  const_iterator cbegin() const noexcept { return stops_.cbegin(); }
  const_iterator cend() const noexcept { return stops_.cend(); }

 private:
  container_type stops_;

  void sort() {
    std::sort(stops_.begin(), stops_.end(),
              [](const stop_type& a, const stop_type& b) { return a.position < b.position; });
  }
};

}  // namespace colorcpp::algorithms::gradient
