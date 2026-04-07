/**
 * @file hsl.hpp
 * @brief Random HSL/HSV color generator with configurable options.
 *
 * @par Thread safety
 * Same as @ref basic_random_generator: not thread-safe across threads on one object.
 *
 * @par Four-channel models (HSLA, HSVA, HWBA)
 * Alpha is drawn uniformly in `[a_min, a_max]` (defaults `[0, 1]`). `set_options` validates `a_min <= a_max` when the
 * color type has four channels.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/operations/random/basic.hpp>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::random {

/**
 * @brief Random HSL or HSV colors with optional per-channel ranges.
 * @tparam Color Color type (HSL or HSV).
 * @tparam Engine Random engine type (default: std::mt19937).
 */
template <typename Color, typename Engine = std::mt19937>
class basic_hsl_generator : public basic_random_generator<Color, Engine> {
  using base = basic_random_generator<Color, Engine>;

 protected:
  using traits = details::color_traits<Color>;
  using T = typename base::T;

 public:
  /** @brief Min/max for H, S, and V or L when generating; for 4-channel types, also @a a_min / @a a_max for alpha. */
  struct options {
    T h_min{T(0)}, h_max{traits::hue_max()};
    T s_min{T(0)}, s_max{traits::template max_at<1>()};
    T l_min{T(0)}, l_max{traits::template max_at<2>()};
    T a_min{T(0)}, a_max{T(1)};
  };

 protected:
  options opts{};

 public:
  basic_hsl_generator(Engine& e, const options& o = {}) : base(e), opts(o) { validate_options(opts); }

  explicit basic_hsl_generator(typename Engine::result_type seed, const options& o = {}) : base(seed), opts(o) {
    validate_options(opts);
  }

  Color next() const { return next_impl(std::make_index_sequence<traits::size>{}); }

  std::vector<Color> generate_n(std::size_t count) const {
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(next());
    return out;
  }

  static Color step_value(const Color& base_c, float factor) {
    Color c = base_c;
    constexpr std::size_t val_idx = 2;  // H, S, [V/L]
    float current = static_cast<float>(c.template get_index<val_idx>());
    float max_val = static_cast<float>(traits::template max_at<val_idx>());
    c.template get_index<val_idx>() = static_cast<T>(std::clamp(current * factor, 0.0f, max_val));
    return c;
  }

  const options& get_options() const { return opts; }

  /** @brief Replace options; throws `std::invalid_argument` if any min > max (including alpha for 4-channel types). */
  void set_options(const options& o) {
    validate_options(o);
    opts = o;
  }

 private:
  static void validate_options(const options& o) {
    if (o.h_min > o.h_max || o.s_min > o.s_max || o.l_min > o.l_max)
      throw std::invalid_argument("colorcpp: generator options min must not exceed max");
    if constexpr (traits::size >= 4) {
      if (o.a_min > o.a_max) throw std::invalid_argument("colorcpp: generator alpha min must not exceed max");
    }
  }

  template <std::size_t I>
  T get_channel_val() const {
    if constexpr (I == 0)
      return this->random_value(opts.h_min, opts.h_max);
    else if constexpr (I == 1)
      return this->random_value(opts.s_min, opts.s_max);
    else if constexpr (I == 2)
      return this->random_value(opts.l_min, opts.l_max);
    else if constexpr (I == 3 && traits::size >= 4)
      return this->random_value(opts.a_min, opts.a_max);
    else
      return traits::template max_at<I>();
  }

  template <std::size_t... Is>
  Color next_impl(std::index_sequence<Is...>) const {
    return Color{get_channel_val<Is>()...};
  }
};

/** @brief Typedefs for common HSL/HSV models. */
using hsl_generator = basic_hsl_generator<core::hsl_float_t>;
using hsv_generator = basic_hsl_generator<core::hsv_float_t>;

}  // namespace colorcpp::operations::random
