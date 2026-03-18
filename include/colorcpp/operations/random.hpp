#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/palette.hpp>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace colorcpp::operations::random {

enum class harmony_mode {
  analogous,
  complementary,
  split_complementary,
  triadic,
  tetradic,
  square
};

namespace details {

template <typename Color>
struct color_traits {
  using value_type = typename Color::value_type;
  using channels_tuple = typename Color::channels_tuple;
  static constexpr std::size_t size = Color::channels;

  static constexpr bool has_hue = traits::has_channel_tag_v<typename Color::trait_model, core::hsl::channel::h_tag> ||
                                  traits::has_channel_tag_v<typename Color::trait_model, core::hsv::channel::h_tag>;

  template <std::size_t I>
  static constexpr value_type min_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return channel_t::min;
  }

  template <std::size_t I>
  static constexpr value_type max_at() {
    using channel_t = std::tuple_element_t<I, channels_tuple>;
    return channel_t::max;
  }

  static constexpr value_type hue_max() { return max_at<0>(); }
};

}  // namespace details

template <typename Color, typename Engine = std::mt19937>
class basic_random_generator {
 protected:
  using traits = details::color_traits<Color>;
  using T = typename traits::value_type;
  mutable Engine rng;

  // uniform_int_distribution requires at least unsigned short (C++ standard §26.6.8.2)
  // uint8_t (unsigned char) is not a valid template argument, promote to unsigned short
  using int_dist_type = std::conditional_t<(sizeof(T) < sizeof(unsigned short)), unsigned short, T>;
  using dist_t = std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
                                    std::uniform_int_distribution<int_dist_type>>;

 public:
  using color_type = Color;

  explicit basic_random_generator(Engine& e) : rng(e) {}
  explicit basic_random_generator(typename Engine::result_type seed) : rng(seed) {}

  T random_value(T min, T max) const {
    dist_t dist(static_cast<typename dist_t::result_type>(min), static_cast<typename dist_t::result_type>(max));
    return static_cast<T>(dist(rng));
  }

 protected:
  template <std::size_t... Is>
  Color generate_full_random(std::index_sequence<Is...>) const {
    return Color{this->random_value(traits::template min_at<Is>(), traits::template max_at<Is>())...};
  }
};

template <typename Color, typename Engine = std::mt19937>
class basic_rgb_generator : public basic_random_generator<Color, Engine> {
  using base = basic_random_generator<Color, Engine>;

 public:
  using base::base;

  Color next() const {
    return this->generate_full_random(std::make_index_sequence<details::color_traits<Color>::size>{});
  }

  std::vector<Color> generate_n(std::size_t count) const {
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(next());
    return out;
  }
};

template <typename Color, typename Engine = std::mt19937>
class basic_hsl_generator : public basic_random_generator<Color, Engine> {
  using base = basic_random_generator<Color, Engine>;

 protected:
  using traits = details::color_traits<Color>;
  using T = typename base::T;

 public:
  struct options {
    T h_min{T(0)}, h_max{traits::hue_max()};
    T s_min{T(0)}, s_max{traits::template max_at<1>()};
    T l_min{T(0)}, l_max{traits::template max_at<2>()};
  };

 protected:
  options opts{};

 public:
  basic_hsl_generator(Engine& e, const options& o = {}) : base(e), opts(o) {
    if (opts.h_min > opts.h_max || opts.s_min > opts.s_max || opts.l_min > opts.l_max)
      throw std::invalid_argument("colorcpp: generator options min must not exceed max");
  }

  explicit basic_hsl_generator(typename Engine::result_type seed, const options& o = {}) : base(seed), opts(o) {
    if (opts.h_min > opts.h_max || opts.s_min > opts.s_max || opts.l_min > opts.l_max)
      throw std::invalid_argument("colorcpp: generator options min must not exceed max");
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
  void set_options(const options& o) { opts = o; }

 private:
  template <std::size_t I>
  T get_channel_val() const {
    if constexpr (I == 0)
      return this->random_value(opts.h_min, opts.h_max);
    else if constexpr (I == 1)
      return this->random_value(opts.s_min, opts.s_max);
    else if constexpr (I == 2)
      return this->random_value(opts.l_min, opts.l_max);
    else
      return traits::template max_at<I>();
  }

  template <std::size_t... Is>
  Color next_impl(std::index_sequence<Is...>) const {
    return Color{get_channel_val<Is>()...};
  }
};

template <typename Color, typename Engine = std::mt19937>
class harmony_generator : public basic_hsl_generator<Color, Engine> {
  using base = basic_hsl_generator<Color, Engine>;
  using traits = details::color_traits<Color>;
  using T = typename base::T;

 public:
  using base::base;

  Color next_harmony(const Color& base_c, harmony_mode mode, std::size_t index = 1) const {
    T h_max = traits::hue_max();
    T h_base = base_c.template get_index<0>();
    T h_new = h_base;

    switch (mode) {
      case harmony_mode::analogous:
        h_new = h_base + this->random_value(-h_max / 12, h_max / 12);
        break;
      case harmony_mode::complementary:
        h_new = h_base + h_max / 2;
        break;
      case harmony_mode::split_complementary:
        h_new = h_base + (index % 2 == 0 ? h_max * 5 / 12 : h_max * 7 / 12);
        break;
      case harmony_mode::triadic:
        h_new = h_base + (h_max * index / 3);
        break;
      case harmony_mode::tetradic:
        h_new = h_base + (index % 4 == 0   ? T(0)
                          : index % 4 == 1 ? h_max / 6
                          : index % 4 == 2 ? h_max / 2
                                           : h_max * 2 / 3);
        break;
      case harmony_mode::square:
        h_new = h_base + (h_max * index / 4);
        break;
    }

    h_new = std::fmod(h_new + h_max, h_max);
    return construct_from_hue(base_c, h_new, std::make_index_sequence<traits::size>{});
  }

  Color next_poisson(std::vector<T>& samples, T min_dist = traits::hue_max() / 10) {
    T h_max = traits::hue_max();
    for (int i = 0; i < 100; ++i) {
      T candidate = this->random_value(T(0), h_max);
      bool ok = true;
      for (T s : samples) {
        T diff = std::abs(candidate - s);
        if (std::min(diff, h_max - diff) < min_dist) {
          ok = false;
          break;
        }
      }
      if (ok) {
        samples.push_back(candidate);
        return construct_from_hue(this->next(), candidate, std::make_index_sequence<traits::size>{});
      }
    }
    Color fallback = this->next();
    samples.push_back(fallback.template get_index<0>());
    return fallback;
  }

  // Returns the complete set of colors for the given harmony mode.
  // The palette always starts with base_c itself, followed by its harmony partners.
  palette::palette_set<Color> generate_palette(const Color& base_c, harmony_mode mode) const {
    std::size_t partner_count = 0;
    switch (mode) {
      case harmony_mode::complementary:
        partner_count = 1;
        break;
      case harmony_mode::split_complementary:
        partner_count = 2;
        break;
      case harmony_mode::analogous:
        partner_count = 2;
        break;
      case harmony_mode::triadic:
        partner_count = 2;
        break;
      case harmony_mode::tetradic:
        partner_count = 3;
        break;
      case harmony_mode::square:
        partner_count = 3;
        break;
    }

    palette::palette_set<Color> result;
    result.add(base_c);
    for (std::size_t i = 1; i <= partner_count; ++i) result.add(next_harmony(base_c, mode, i));
    return result;
  }

 private:
  template <std::size_t... Is>
  Color construct_from_hue(const Color& base_c, T new_h, std::index_sequence<Is...>) const {
    return Color{(Is == 0 ? new_h : base_c.template get_index<Is>())...};
  }
};

template <typename Color, typename Engine = std::mt19937>
class golden_angle_generator : public basic_hsl_generator<Color, Engine> {
  using base = basic_hsl_generator<Color, Engine>;
  using traits = details::color_traits<Color>;
  using T = typename base::T;
  mutable T current_h;

 public:
  explicit golden_angle_generator(Engine& e, const typename base::options& o = {}) : base(e, o) {
    current_h = this->random_value(T(0), traits::hue_max());
  }

  explicit golden_angle_generator(typename Engine::result_type seed, const typename base::options& o = {})
      : base(seed, o) {
    current_h = this->random_value(T(0), traits::hue_max());
  }

  Color next() const {
    float step = (137.5077f / 360.0f) * static_cast<float>(traits::hue_max());
    current_h = static_cast<T>(std::fmod(static_cast<float>(current_h) + step, static_cast<float>(traits::hue_max())));
    return next_impl(std::make_index_sequence<traits::size>{});
  }

 private:
  template <std::size_t I>
  T channel_for_index() const {
    if constexpr (I == 0)
      return current_h;
    else if constexpr (I == 1)
      return this->opts.s_max;
    else if constexpr (I == 2)
      return this->opts.l_max;
    else
      return traits::template max_at<I>();
  }

  template <std::size_t... Is>
  Color next_impl(std::index_sequence<Is...>) const {
    return Color{channel_for_index<Is>()...};
  }

 public:
  std::vector<Color> generate_n(std::size_t count) const {
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(next());
    return out;
  }
};

using rgb8_generator = basic_rgb_generator<core::rgb8_t>;
using rgba8_generator = basic_rgb_generator<core::rgba8_t>;
using hsl_generator = basic_hsl_generator<core::hsl_float_t>;

using hsv_generator = basic_hsl_generator<core::hsv_float_t>;
using harmony_gen = harmony_generator<core::hsla_float_t>;
using golden_gen = golden_angle_generator<core::hsl_float_t>;

}  // namespace colorcpp::operations::random
