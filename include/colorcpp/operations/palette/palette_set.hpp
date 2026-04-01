/**
 * @file palette_set.hpp
 * @brief Ordered list of colors with wrap indexing and bounds-checked access.
 */

#pragma once

#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::palette {

/** @brief Ordered list of colors with wrap indexing and bounds-checked @ref at. */
template <typename Color>
class palette_set {
 public:
  using color_type = Color;

  palette_set() = default;
  palette_set(std::initializer_list<Color> colors) : colors_(colors) {}
  palette_set(const std::vector<Color>& colors) : colors_(colors) {}

  void add(const Color& c) { colors_.push_back(c); }
  const std::vector<Color>& colors() const { return colors_; }

  size_t size() const { return colors_.size(); }
  bool empty() const { return colors_.empty(); }

  Color operator[](size_t index) const {
    if (colors_.empty()) return Color{};
    return colors_[index % colors_.size()];
  }

  Color at(size_t index) const {
    if (index >= colors_.size()) throw std::out_of_range("colorcpp: palette index out of range");
    return colors_[index];
  }

  auto begin() { return colors_.begin(); }
  auto end() { return colors_.end(); }
  auto begin() const { return colors_.cbegin(); }
  auto end() const { return colors_.cend(); }
  auto cbegin() const { return colors_.cbegin(); }
  auto cend() const { return colors_.cend(); }

 private:
  std::vector<Color> colors_;
};

}  // namespace colorcpp::operations::palette
