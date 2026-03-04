#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <type_traits>
#include <vector>

#include "../../core/hsl.hpp"
#include "../../core/hsv.hpp"
#include "../../core/rgb.hpp"

namespace color::operations::generation {
inline namespace random {

namespace details {

template <typename Color>
struct color_traits;

template <typename T, typename Scale>
struct color_traits<core::basic_rgba<T, Scale>> {
  using value_type = T;
  static constexpr bool has_hue = false;

  static constexpr T min() { return T(0); }
  static constexpr T max() { return core::basic_rgba<T, Scale>::full_range; }
  static constexpr T comp_max() { return core::basic_rgba<T, Scale>::full_range; }
};

template <typename T, typename ScaleH, typename ScaleSLA>
struct color_traits<core::basic_hsla<T, ScaleH, ScaleSLA>> {
  using value_type = T;
  static constexpr bool has_hue = true;

  static constexpr T min() { return T(0); }
  static constexpr T hue_max() { return core::basic_hsla<T, ScaleH, ScaleSLA>::h_full; }
  static constexpr T comp_max() { return core::basic_hsla<T, ScaleH, ScaleSLA>::sla_full; }
};

template <typename T, typename ScaleH, typename ScaleSVA>
struct color_traits<core::basic_hsva<T, ScaleH, ScaleSVA>> {
  using value_type = T;
  static constexpr bool has_hue = true;

  static constexpr T min() { return T(0); }
  static constexpr T hue_max() { return core::basic_hsva<T, ScaleH, ScaleSVA>::h_full; }
  static constexpr T comp_max() { return core::basic_hsva<T, ScaleH, ScaleSVA>::sva_full; }
};

}  // namespace details

template <typename Color, typename Engine = std::mt19937>
class basic_random_generator {
 protected:
  using traits = details::color_traits<Color>;
  using T = typename traits::value_type;

  mutable Engine rng;

  using dist_t = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
                                    std::uniform_int_distribution<T>>;

 public:
  explicit basic_random_generator(Engine& e) : rng(e) {}

  T random_value(T min = traits::min(), T max = traits::has_hue ? traits::comp_max() : traits::max()) const {
    dist_t dist(min, max);
    return dist(rng);
  }

  T random_hue() const {
    static_assert(traits::has_hue, "Color has no hue component");
    dist_t dist(T(0), traits::hue_max());
    return dist(rng);
  }
};

template <typename Color, typename Engine = std::mt19937>
class basic_rgb_generator : public basic_random_generator<Color, Engine> {
  using base = basic_random_generator<Color, Engine>;
  using traits = typename base::traits;
  using T = typename base::T;

 public:
  using result_type = Color;
  using base::base;

  result_type next(T alpha = traits::max()) const {
    return result_type{this->random_value(), this->random_value(), this->random_value(), alpha};
  }
};

template <typename Color, typename Engine = std::mt19937>
class basic_hsl_generator : public basic_random_generator<Color, Engine> {
  using base = basic_random_generator<Color, Engine>;
  using traits = typename base::traits;
  using T = typename base::T;

 public:
  using result_type = Color;

  struct options {
    T h_min{0};
    T h_max{traits::hue_max()};
    T s_min{traits::min()};
    T s_max{traits::comp_max()};
    T l_min{traits::min()};
    T l_max{traits::comp_max()};
  };

 private:
  options opts{};

 public:
  basic_hsl_generator(Engine& e, const options& o = {}) : base(e), opts(o) {}

  result_type next(T alpha = traits::comp_max()) const {
    return result_type{this->random_value(opts.h_min, opts.h_max), this->random_value(opts.s_min, opts.s_max),
                       this->random_value(opts.l_min, opts.l_max), alpha};
  }

  const options& get_options() const { return opts; }
  void set_options(const options& o) { opts = o; }
};

template <typename Color, typename Engine = std::mt19937>
class basic_distinct_generator : public basic_hsl_generator<Color, Engine> {
  using base = basic_hsl_generator<Color, Engine>;
  using traits = typename base::traits;
  using T = typename base::T;

  static_assert(traits::has_hue, "Distinct generator requires hue");

  mutable std::vector<T> used_hues;
  mutable T min_distance;

  T hue_distance(T h1, T h2) const {
    T diff = std::abs(h1 - h2);
    T max = traits::hue_max();
    return std::min(diff, max - diff);
  }

 public:
  using result_type = Color;

  explicit basic_distinct_generator(Engine& e, T distance = traits::hue_max() / 12) : base(e), min_distance(distance) {}

  result_type next() {
    constexpr int max_attempts = 100;
    int attempts = 0;
    T new_hue{};

    do {
      new_hue = this->random_hue();
      bool too_close = false;

      for (T h : used_hues) {
        if (hue_distance(new_hue, h) < min_distance) {
          too_close = true;
          break;
        }
      }

      if (!too_close) {
        used_hues.push_back(new_hue);
        break;
      }

    } while (++attempts < max_attempts);

    return result_type{new_hue, this->random_value(), this->random_value(), traits::comp_max()};
  }

  void reset() { used_hues.clear(); }
};

template <typename Color, typename Engine = std::mt19937>
class basic_golden_angle_generator : public basic_hsl_generator<Color, Engine> {
  using base = basic_hsl_generator<Color, Engine>;
  using traits = typename base::traits;
  using T = typename base::T;

  static_assert(traits::has_hue, "Golden angle requires hue");

  mutable T current_hue{};
  static constexpr double GOLDEN_ANGLE = 137.507764;

 public:
  using result_type = Color;

  explicit basic_golden_angle_generator(Engine& e) : base(e) {
    std::uniform_real_distribution<double> dist(0.0, 360.0);
    current_hue = static_cast<T>(dist(this->rng));
  }

  result_type next() const {
    if constexpr (std::is_floating_point_v<T>) {
      current_hue = std::fmod(current_hue + T(GOLDEN_ANGLE), traits::hue_max());
    } else {
      current_hue = (current_hue + T((GOLDEN_ANGLE / 360.0) * traits::hue_max())) % traits::hue_max();
    }

    return result_type{current_hue, this->random_value(), this->random_value(), traits::comp_max()};
  }
};

using rgb8_generator = basic_rgb_generator<core::rgba8_t>;
using rgbf_generator = basic_rgb_generator<core::rgba_float_t>;

using hsl_int_generator = basic_hsl_generator<core::hsla_int_t>;
using hsl_float_generator = basic_hsl_generator<core::hsla_float_t>;

using hsv_int_generator = basic_hsl_generator<core::hsva_int_t>;
using hsv_float_generator = basic_hsl_generator<core::hsva_float_t>;

using distinct_generator = basic_distinct_generator<core::hsla_float_t>;
using golden_angle_generator = basic_golden_angle_generator<core::hsla_float_t>;

template <typename Color, typename Engine>
inline auto make_generator(Engine& e) {
  return basic_random_generator<Color, Engine>(e);
}

template <typename Color, typename Engine>
inline auto make_rgb_generator(Engine& e) {
  return basic_rgb_generator<Color, Engine>(e);
}

template <typename Color, typename Engine>
inline auto make_hsl_generator(Engine& e) {
  return basic_hsl_generator<Color, Engine>(e);
}

}  // namespace random

}  // namespace color::operations::generation
