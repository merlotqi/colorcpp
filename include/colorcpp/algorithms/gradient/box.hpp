/**
 * @file box.hpp
 * @brief Box gradient generation.
 *
 * Box gradients interpolate colors using Chebyshev distance from the center.
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
 * @brief Box gradient generator.
 * @tparam Color The color type.
 */
template <typename Color>
class box_gradient {
 public:
  using color_type = Color;
  using stops_type = color_stops<Color>;
  using position_type = typename stops_type::position_type;
  using easing_type = easing::easing_function<position_type>;

  explicit box_gradient(const stops_type& stops, easing_type easing = easing::linear<position_type>)
      : stops_(stops), easing_(std::move(easing)) {
    details::validate_stops(stops_);
  }

  box_gradient(std::initializer_list<color_stop<Color>> stops, easing_type easing = easing::linear<position_type>)
      : stops_(stops), easing_(std::move(easing)) {
    details::validate_stops(stops_);
  }

  Color sample(position_type distance) const {
    auto interpolator = [](const Color& a, const Color& b, position_type t) {
      return operations::interpolate::lerp(a, b, t);
    };
    return details::sample_gradient(stops_, details::clamp_01(distance + phase_offset_), interpolator, easing_);
  }

  position_type position_at(float x, float y) const noexcept {
    const position_type dx = std::abs(static_cast<position_type>(x) - 0.5f);
    const position_type dy = std::abs(static_cast<position_type>(y) - 0.5f);
    return details::clamp_01(std::max(dx, dy) * 2.0f);
  }

  Color sample_at(float x, float y) const { return sample(position_at(x, y)); }

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

  const stops_type& stops() const noexcept { return stops_; }
  const easing_type& easing() const noexcept { return easing_; }
  void set_easing(easing_type easing) { easing_ = std::move(easing); }

  box_gradient<Color> reverse() const {
    typename stops_type::container_type reversed_stops;
    reversed_stops.reserve(stops_.size());
    for (const auto& stop : stops_) {
      reversed_stops.emplace_back(1.0f - stop.position, stop.color);
    }
    return box_gradient<Color>(stops_type(reversed_stops), easing_);
  }

  box_gradient<Color> blend(const box_gradient<Color>& other, float t) const {
    const std::size_t n = std::max<std::size_t>(3, std::max(stops_.size(), other.stops_.size()) * 8);
    typename stops_type::container_type blended_stops;
    blended_stops.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
      float u = static_cast<float>(i) / static_cast<float>(n - 1);
      Color c = operations::interpolate::lerp(sample(u), other.sample(u), t);
      blended_stops.emplace_back(u, c);
    }
    return box_gradient<Color>(stops_type(blended_stops), easing_);
  }

  box_gradient<Color> concat(const box_gradient<Color>& other) const {
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
    return box_gradient<Color>(stops_type(concat_stops), easing_);
  }

  box_gradient<Color> scale(float factor) const {
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

    box_gradient<Color> g(stops_type(scaled_stops), easing_);
    g.phase_offset_ = phase_offset_;
    return g;
  }

  box_gradient<Color> offset(float delta) const {
    box_gradient<Color> g(stops_, easing_);
    g.phase_offset_ = phase_offset_ + delta;
    return g;
  }

  box_gradient<Color> quantize(std::size_t levels) const {
    if (levels == 0) {
      throw std::invalid_argument("colorcpp: quantize levels must be at least 1");
    }

    if (levels == 1) {
      typename stops_type::container_type quantized_stops;
      quantized_stops.emplace_back(0.5f, sample(0.5f));
      return box_gradient<Color>(stops_type(quantized_stops), easing_);
    }

    typename stops_type::container_type quantized_stops;
    quantized_stops.reserve(levels);

    for (std::size_t i = 0; i < levels; ++i) {
      float pos = static_cast<float>(i) / static_cast<float>(levels - 1);
      quantized_stops.emplace_back(pos, sample(pos));
    }

    return box_gradient<Color>(stops_type(quantized_stops), easing_);
  }

 private:
  stops_type stops_;
  easing_type easing_;
  position_type phase_offset_{};
};

template <typename Color>
box_gradient<Color> box(
    const color_stops<Color>& stops,
    typename box_gradient<Color>::easing_type easing = [](typename box_gradient<Color>::position_type t) {
      return easing::linear(t);
    }) {
  return box_gradient<Color>(stops, std::move(easing));
}

template <typename Color>
box_gradient<Color> box(
    std::initializer_list<color_stop<Color>> stops,
    typename box_gradient<Color>::easing_type easing = [](typename box_gradient<Color>::position_type t) {
      return easing::linear(t);
    }) {
  return box_gradient<Color>(stops, std::move(easing));
}

}  // namespace colorcpp::algorithms::gradient
