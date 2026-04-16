/**
 * @file stepped.hpp
 * @brief Stepped gradient decorator.
 *
 * Applies hard quantization to normalized gradient positions while preserving the wrapped gradient API where possible.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/gradient/details.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace colorcpp::algorithms::gradient {

namespace details {
template <typename Gradient, typename = void>
struct has_position_at : std::false_type {};

template <typename Gradient>
struct has_position_at<Gradient, std::void_t<decltype(std::declval<const Gradient&>().position_at(0.0f, 0.0f))>>
    : std::true_type {};

template <typename Gradient>
inline constexpr bool has_position_at_v = has_position_at<Gradient>::value;

inline float stepped_position(float t, std::size_t levels) {
  t = clamp_01(t);
  if (levels == 0) {
    throw std::invalid_argument("colorcpp: stepped gradient levels must be at least 1");
  }
  if (levels == 1) {
    return 0.5f;
  }

  const float steps = static_cast<float>(levels - 1);
  return std::floor(t * steps) / steps;
}

}  // namespace details

/**
 * @brief Gradient decorator that quantizes normalized sampling positions into discrete steps.
 * @tparam Gradient Wrapped gradient type.
 */
template <typename Gradient>
class stepped_gradient {
 public:
  using gradient_type = Gradient;
  using color_type = typename Gradient::color_type;
  using position_type = float;

  stepped_gradient(Gradient gradient, std::size_t levels) : gradient_(std::move(gradient)), levels_(levels) {
    if (levels_ == 0) {
      throw std::invalid_argument("colorcpp: stepped gradient levels must be at least 1");
    }
  }

  color_type sample(position_type t) const { return gradient_.sample(details::stepped_position(t, levels_)); }

  template <typename G = Gradient, std::enable_if_t<details::has_position_at_v<G>, int> = 0>
  color_type sample_at(float x, float y) const {
    return sample(gradient_.position_at(x, y));
  }

  std::vector<color_type> palette(std::size_t count) const {
    if (count == 0) {
      return {};
    }
    if (count == 1) {
      return {sample(0.5f)};
    }

    std::vector<color_type> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
      out.push_back(sample(static_cast<float>(i) / static_cast<float>(count - 1)));
    }
    return out;
  }

  const Gradient& gradient() const noexcept { return gradient_; }
  Gradient& gradient() noexcept { return gradient_; }
  std::size_t levels() const noexcept { return levels_; }

  stepped_gradient<Gradient> with_levels(std::size_t levels) const {
    return stepped_gradient<Gradient>(gradient_, levels);
  }

  template <typename G = Gradient, typename = std::void_t<decltype(std::declval<const G&>().reverse())>>
  auto reverse() const -> stepped_gradient<decltype(std::declval<const G&>().reverse())> {
    return stepped_gradient<decltype(std::declval<const G&>().reverse())>(gradient_.reverse(), levels_);
  }

  template <typename G = Gradient, typename = std::void_t<decltype(std::declval<const G&>().scale(1.0f))>>
  auto scale(float factor) const -> stepped_gradient<decltype(std::declval<const G&>().scale(1.0f))> {
    return stepped_gradient<decltype(std::declval<const G&>().scale(1.0f))>(gradient_.scale(factor), levels_);
  }

  template <typename G = Gradient, typename = std::void_t<decltype(std::declval<const G&>().offset(1.0f))>>
  auto offset(float delta) const -> stepped_gradient<decltype(std::declval<const G&>().offset(1.0f))> {
    return stepped_gradient<decltype(std::declval<const G&>().offset(1.0f))>(gradient_.offset(delta), levels_);
  }

  template <typename G = Gradient,
            typename = std::void_t<decltype(std::declval<const G&>().blend(std::declval<const G&>(), 0.0f))>>
  auto blend(const G& other, float t) const
      -> stepped_gradient<decltype(std::declval<const G&>().blend(std::declval<const G&>(), 0.0f))> {
    return stepped_gradient<decltype(std::declval<const G&>().blend(std::declval<const G&>(), 0.0f))>(
        gradient_.blend(other, t), levels_);
  }

  template <typename G = Gradient,
            typename = std::void_t<decltype(std::declval<const G&>().blend(std::declval<const G&>(), 0.0f))>>
  auto blend(const stepped_gradient<G>& other, float t) const
      -> stepped_gradient<decltype(std::declval<const G&>().blend(std::declval<const G&>(), 0.0f))> {
    return stepped_gradient<decltype(std::declval<const G&>().blend(std::declval<const G&>(), 0.0f))>(
        gradient_.blend(other.gradient(), t), levels_);
  }

  template <typename G = Gradient,
            typename = std::void_t<decltype(std::declval<const G&>().concat(std::declval<const G&>()))>>
  auto concat(const G& other) const
      -> stepped_gradient<decltype(std::declval<const G&>().concat(std::declval<const G&>()))> {
    return stepped_gradient<decltype(std::declval<const G&>().concat(std::declval<const G&>()))>(
        gradient_.concat(other), levels_);
  }

  template <typename G = Gradient,
            typename = std::void_t<decltype(std::declval<const G&>().concat(std::declval<const G&>()))>>
  auto concat(const stepped_gradient<G>& other) const
      -> stepped_gradient<decltype(std::declval<const G&>().concat(std::declval<const G&>()))> {
    return stepped_gradient<decltype(std::declval<const G&>().concat(std::declval<const G&>()))>(
        gradient_.concat(other.gradient()), levels_);
  }

 private:
  Gradient gradient_;
  std::size_t levels_;
};

template <typename Gradient>
stepped_gradient<Gradient> stepped(Gradient gradient, std::size_t levels) {
  return stepped_gradient<Gradient>(std::move(gradient), levels);
}

}  // namespace colorcpp::algorithms::gradient
