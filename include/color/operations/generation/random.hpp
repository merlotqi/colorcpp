#pragma once

#include <algorithm>
#include <cmath>
#include <color/core/hsl.hpp>
#include <color/core/rgb.hpp>
#include <cstdint>
#include <random>
#include <type_traits>
#include <vector>

namespace color::operations::generation {
inline namespace random {

template <typename T, intptr_t Scale>
struct distribution_traits {
  template <typename U>
  using uniform_type = std::conditional_t<std::is_floating_point_v<U>, std::uniform_real_distribution<U>,
                                          std::uniform_int_distribution<U>>;

  static constexpr T min() { return T(0); }
  static constexpr T max() { return T(Scale); }
  static constexpr T hue_max() {
    if constexpr (std::is_floating_point<T>::value) {
      return T(360);
    } else {
      return T(Scale);
    }
  }
};

template <typename T, intptr_t Scale = 1, typename Engine = std::mt19937>
class basic_random_generator {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

 protected:
  mutable Engine rng;

 public:
  explicit basic_random_generator(Engine& rng) : rng(rng) {}

  T random_value(T min = distribution_traits<T, Scale>::min(), T max = distribution_traits<T, Scale>::max()) const {
    using value_dist = typename distribution_traits<T, Scale>::template uniform_type<T>;
    value_dist dist(min, max);
    return dist(rng);
  }

  T random_hue() const {
    using hue_dist = typename distribution_traits<T, Scale>::template uniform_type<T>;
    hue_dist dist(T(0), distribution_traits<T, Scale>::hue_max());
    return dist(rng);
  }
};

template <typename T, intptr_t Scale = 1, typename Engine = std::mt19937>
class basic_rgb_generator : public basic_random_generator<T, Scale> {
 public:
  using result_type = core::basic_rgba<T, Scale>;

  basic_rgb_generator() = default;
  explicit basic_rgb_generator(Engine& random_engine) : basic_random_generator<T, Scale>(random_engine) {}

  result_type next(T alpha = distribution_traits<T, Scale>::max()) const {
    return result_type{this->random_value(), this->random_value(), this->random_value(), alpha};
  }

  result_type next(T rmin, T rmax, T gmin, T gmax, T bmin, T bmax, T alpha = distribution_traits<T, Scale>::max()) {
    return result_type{this->random_value(rmin, rmax), this->random_value(gmin, gmax), this->random_value(bmin, bmax),
                       alpha};
  }
};

template <typename T, intptr_t Scale = 1, typename Engine = std::mt19937>
class basic_hsl_generator : public basic_random_generator<T, Scale> {
 public:
  using result_type = core::basic_hsla<T, Scale>;
  struct options {
    T h_min{0};
    T h_max{distribution_traits<T, Scale>::hue_max()};
    T s_min{distribution_traits<T, Scale>::min()};
    T s_max{distribution_traits<T, Scale>::max()};
    T l_min{distribution_traits<T, Scale>::min()};
    T l_max{distribution_traits<T, Scale>::max()};
  };

  basic_hsl_generator() = default;
  explicit basic_hsl_generator(Engine& random_engine) : basic_random_generator<T, Scale>(random_engine) {}
  explicit basic_hsl_generator(Engine& random_engine, const options& opt)
      : basic_random_generator<T, Scale>(random_engine), opts(opt) {}

  result_type next(T alpha = distribution_traits<T, Scale>::max()) const {
    return result_type{this->random_value(opts.h_min, opts.h_max), this->random_value(opts.s_min, opts.s_max),
                       this->random_value(opts.l_min, opts.l_max), alpha};
  }

 private:
  options opts;
};

template <typename T, intptr_t Scale = 1, typename Engine = std::mt19937>
class basic_hsv_generator : public basic_random_generator<T, Scale, Engine> {
  using base = basic_random_generator<T, Scale, Engine>;

 public:
  using result_type = core::basic_hsla<T, Scale>;

  struct options {
    T hue_min = T(0);
    T hue_max = distribution_traits<T, Scale>::hue_max();
    T saturation_min = distribution_traits<T, Scale>::min();
    T saturation_max = distribution_traits<T, Scale>::max();
    T value_min = distribution_traits<T, Scale>::min();
    T value_max = distribution_traits<T, Scale>::max();
  };

 private:
  options opts;

 public:
  explicit basic_hsv_generator(Engine& random_engine) : base(random_engine) {}

  basic_hsv_generator(Engine& random_engine, const options& opt) : base(random_engine), opts(opt) {}

  void set_options(const options& opt) { opts = opt; }
  const options& get_options() const { return opts; }

  result_type next(T alpha = distribution_traits<T, Scale>::max()) const {
    return result_type{this->random_value(opts.hue_min, opts.hue_max),
                       this->random_value(opts.saturation_min, opts.saturation_max),
                       this->random_value(opts.value_min, opts.value_max), alpha};
  }

  std::vector<result_type> next_n(size_t n, T alpha = distribution_traits<T, Scale>::max()) const {
    std::vector<result_type> colors;
    colors.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      colors.push_back(next(alpha));
    }
    return colors;
  }
};
template <typename T, intptr_t Scale = 1, typename Engine = std::mt19937>
class basic_distinct_generator : public basic_random_generator<T, Scale, Engine> {
  using base = basic_random_generator<T, Scale, Engine>;

 public:
  using result_type = core::basic_hsla<T, Scale>;

  struct options {
    T min_hue_distance = T(30);
    T saturation = T(Scale);
    T lightness = T(Scale / 2);
    T alpha = T(Scale);
    bool use_fixed_saturation = false;
    bool use_fixed_lightness = false;
  };

 private:
  options opts;
  mutable std::vector<T> used_hues;
  mutable T current_min_distance;

  T normalize_hue(T hue) const {
    T hue_max = distribution_traits<T, Scale>::hue_max();
    if constexpr (std::is_floating_point<T>::value) {
      T result = std::fmod(hue, hue_max);
      return result < 0 ? result + hue_max : result;
    } else {
      return hue % hue_max;
    }
  }

  T hue_distance(T h1, T h2) const {
    T hue_max = distribution_traits<T, Scale>::hue_max();
    T diff = std::abs(h1 - h2);
    if constexpr (std::is_floating_point<T>::value) {
      return std::min(diff, hue_max - diff);
    } else {
      T diff2 = hue_max - diff;
      return diff < diff2 ? diff : diff2;
    }
  }

 public:
  explicit basic_distinct_generator(Engine& random_engine)
      : base(random_engine), current_min_distance(opts.min_hue_distance) {
    static_assert(std::is_floating_point<T>::value || Scale > 0, "Scale must be positive for integer types");
  }

  basic_distinct_generator(Engine& random_engine, const options& opt)
      : base(random_engine), opts(opt), current_min_distance(opt.min_hue_distance) {}

  void reset() {
    used_hues.clear();
    current_min_distance = opts.min_hue_distance;
  }

  void set_options(const options& opt) {
    opts = opt;
    reset();
  }

  result_type next() {
    const int max_attempts = 100;
    int attempts = 0;
    T new_hue;
    T hue_max = distribution_traits<T, Scale>::hue_max();

    do {
      new_hue = this->random_value(0, hue_max);
      attempts++;

      bool too_close = false;
      for (T h : used_hues) {
        if (hue_distance(new_hue, h) < current_min_distance) {
          too_close = true;
          break;
        }
      }

      if (!too_close) {
        used_hues.push_back(new_hue);
        break;
      }

      if (attempts > max_attempts / 2) {
        if constexpr (std::is_floating_point<T>::value) {
          current_min_distance *= T(0.9);
        } else {
          current_min_distance = T(double(current_min_distance) * 0.9);
        }
      }
    } while (attempts < max_attempts);

    T s = opts.use_fixed_saturation ? opts.saturation : this->random_value(0, Scale);
    T l = opts.use_fixed_lightness ? opts.lightness : this->random_value(0, Scale);

    return result_type{new_hue, s, l, opts.alpha};
  }

  auto next_rgb() const { return next().template to_rgb<T, Scale>(); }

  std::vector<result_type> next_n(size_t n) {
    std::vector<result_type> colors;
    colors.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      colors.push_back(next());
    }
    return colors;
  }

  size_t used_count() const { return used_hues.size(); }
};

template <typename T, intptr_t Scale = 1, typename Engine = std::mt19937>
class basic_golden_angle_generator : public basic_random_generator<T, Scale, Engine> {
  using base = basic_random_generator<T, Scale, Engine>;

 public:
  using result_type = core::basic_hsla<T, Scale>;

  struct options {
    T saturation_min = T(Scale / 2);
    T saturation_max = T(Scale);
    T lightness_min = T(Scale / 3);
    T lightness_max = T(Scale * 2 / 3);
    T alpha = T(Scale);
  };

 private:
  options opts;
  mutable T current_hue;
  static constexpr double GOLDEN_ANGLE = 137.507764;

  T advance_hue() const {
    T hue_max = distribution_traits<T, Scale>::hue_max();

    if constexpr (std::is_floating_point<T>::value) {
      current_hue = std::fmod(current_hue + T(GOLDEN_ANGLE), hue_max);
    } else {
      double angle_step = (GOLDEN_ANGLE / 360.0) * Scale;
      double next = double(current_hue) + angle_step;
      if (next >= Scale) {
        next -= Scale;
      }
      current_hue = T(next);
    }
    return current_hue;
  }

 public:
  explicit basic_golden_angle_generator(Engine& random_engine) : base(random_engine) {
    std::uniform_real_distribution<double> dist(0, 360);
    current_hue = T(dist(this->rng));
  }

  basic_golden_angle_generator(Engine& random_engine, const options& opt) : base(random_engine), opts(opt) {
    std::uniform_real_distribution<double> dist(0, 360);
    current_hue = T(dist(this->rng));
  }

  void set_options(const options& opt) { opts = opt; }

  result_type next() const {
    T hue = advance_hue();
    T s = this->random_value(opts.saturation_min, opts.saturation_max);
    T l = this->random_value(opts.lightness_min, opts.lightness_max);

    return result_type{hue, s, l, opts.alpha};
  }

  auto next_rgb() const { return next().template to_rgb<T, Scale>(); }

  std::vector<result_type> next_n(size_t n) const {
    std::vector<result_type> colors;
    colors.reserve(n);
    for (size_t i = 0; i < n; ++i) {
      colors.push_back(next());
    }
    return colors;
  }

  void reset_hue(T hue = T(0)) { current_hue = normalize_hue(hue); }

 private:
  T normalize_hue(T hue) const {
    T hue_max = distribution_traits<T, Scale>::hue_max();
    if constexpr (std::is_floating_point<T>::value) {
      T result = std::fmod(hue, hue_max);
      return result < 0 ? result + hue_max : result;
    } else {
      return hue % hue_max;
    }
  }
};

template <typename Engine = std::mt19937>
using rgb8_generator = basic_rgb_generator<uint8_t, 255, Engine>;

template <typename Engine = std::mt19937>
using rgb16_generator = basic_rgb_generator<uint16_t, 65535, Engine>;

template <typename Engine = std::mt19937>
using rgb32_generator = basic_rgb_generator<uint32_t, 4294967295, Engine>;

template <typename Engine = std::mt19937>
using rgbf_generator = basic_rgb_generator<float, 1000, Engine>;

template <typename Engine = std::mt19937>
using rgbd_generator = basic_rgb_generator<double, 1000, Engine>;

template <typename Engine = std::mt19937>
using hsl8_generator = basic_hsl_generator<uint8_t, 255, Engine>;

template <typename Engine = std::mt19937>
using hslf_generator = basic_hsl_generator<float, 1000, Engine>;

template <typename Engine = std::mt19937>
using hsld_generator = basic_hsl_generator<double, 1000, Engine>;

template <typename Engine = std::mt19937>
using hsv8_generator = basic_hsv_generator<uint8_t, 255, Engine>;

template <typename Engine = std::mt19937>
using hsvf_generator = basic_hsv_generator<float, 1000, Engine>;

template <typename Engine = std::mt19937>
using hsvd_generator = basic_hsv_generator<double, 1000, Engine>;

template <typename Engine = std::mt19937>
using distinct_generator = basic_distinct_generator<float, 1, Engine>;

template <typename Engine = std::mt19937>
using distinct8_generator = basic_distinct_generator<uint8_t, 255, Engine>;

template <typename Engine = std::mt19937>
using golden_angle_generator = basic_golden_angle_generator<float, 1, Engine>;

template <typename Engine = std::mt19937>
using golden_angle8_generator = basic_golden_angle_generator<uint8_t, 255, Engine>;

template <typename Engine = std::mt19937, typename T = uint8_t, intptr_t Scale = 255>
inline auto make_rgb_generator(Engine& engine) {
  return basic_rgb_generator<T, Scale, Engine>(engine);
}

template <typename Engine = std::mt19937, typename T = float, intptr_t Scale = 1>
inline auto make_hsl_generator(Engine& engine) {
  return basic_hsl_generator<T, Scale, Engine>(engine);
}

template <typename T = uint8_t, intptr_t Scale = 255>
inline auto random_rgb() {
  static std::mt19937 rng(std::random_device{}());
  basic_rgb_generator<T, Scale, std::mt19937> gen(rng);
  return gen.next();
}

template <typename T = float, intptr_t Scale = 1>
inline auto random_hsl_rgb() {
  static std::mt19937 rng(std::random_device{}());
  basic_hsl_generator<T, Scale, std::mt19937> gen(rng);
  return gen.next_rgb();
}

template <typename Engine>
inline auto make_pastel_generator(Engine& engine) {
  typename basic_hsl_generator<float, 1, Engine>::options opts;
  opts.saturation_min = 0.2f;
  opts.saturation_max = 0.4f;
  opts.lightness_min = 0.7f;
  opts.lightness_max = 0.9f;
  return basic_hsl_generator<float, 1, Engine>(engine, opts);
}

template <typename Engine>
inline auto make_vibrant_generator(Engine& engine) {
  typename basic_hsl_generator<float, 1, Engine>::options opts;
  opts.saturation_min = 0.8f;
  opts.saturation_max = 1.0f;
  opts.lightness_min = 0.4f;
  opts.lightness_max = 0.6f;
  return basic_hsl_generator<float, 1, Engine>(engine, opts);
}

template <typename Engine>
inline auto make_dark_generator(Engine& engine) {
  typename basic_hsl_generator<float, 1, Engine>::options opts;
  opts.saturation_min = 0.5f;
  opts.saturation_max = 0.9f;
  opts.lightness_min = 0.1f;
  opts.lightness_max = 0.3f;
  return basic_hsl_generator<float, 1, Engine>(engine, opts);
}

template <typename Engine>
inline auto make_light_generator(Engine& engine) {
  typename basic_hsl_generator<float, 1, Engine>::options opts;
  opts.saturation_min = 0.2f;
  opts.saturation_max = 0.5f;
  opts.lightness_min = 0.8f;
  opts.lightness_max = 0.95f;
  return basic_hsl_generator<float, 1, Engine>(engine, opts);
}

template <typename Engine>
inline auto make_warm_generator(Engine& engine) {
  typename basic_hsl_generator<float, 1, Engine>::options opts;
  opts.hue_min = 0.0f;
  opts.hue_max = 60.0f;  // 红到黄
  opts.saturation_min = 0.6f;
  opts.saturation_max = 0.9f;
  opts.lightness_min = 0.4f;
  opts.lightness_max = 0.7f;
  return basic_hsl_generator<float, 1, Engine>(engine, opts);
}

template <typename Engine>
inline auto make_cool_generator(Engine& engine) {
  typename basic_hsl_generator<float, 1, Engine>::options opts;
  opts.hue_min = 180.0f;
  opts.hue_max = 240.0f;  // 青到蓝
  opts.saturation_min = 0.6f;
  opts.saturation_max = 0.9f;
  opts.lightness_min = 0.4f;
  opts.lightness_max = 0.7f;
  return basic_hsl_generator<float, 1, Engine>(engine, opts);
}

}  // namespace random
}  // namespace color::operations::generation
