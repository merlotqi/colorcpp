/**
 * @file harmony.hpp
 * @brief Harmony-based color generator.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/operations/palette.hpp>
#include <colorcpp/operations/random/hsl.hpp>
#include <vector>

namespace colorcpp::operations::random {

/** @brief Harmony layout for @ref harmony_generator. */
enum class harmony_mode {
  analogous,
  complementary,
  split_complementary,
  triadic,
  tetradic,
  square
};

/**
 * @brief Builds harmony-related hues from a base color.
 * @tparam Color Color type (HSL or HSV).
 * @tparam Engine Random engine type (default: std::mt19937).
 */
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

  /**
   * @brief Generate a color with hue sampled via Poisson disk sampling.
   *
   * Ensures the new hue is at least min_dist away from all existing samples.
   * Uses rejection sampling with configurable attempt limit.
   *
   * @param samples Vector of existing hue samples (modified in place).
   * @param min_dist Minimum angular distance between hues (default: 1/10 of hue range).
   * @param max_attempts Maximum rejection sampling attempts (default: 200).
   * @return Color with Poisson-sampled hue, or random fallback if max_attempts exceeded.
   */
  Color next_poisson(std::vector<T>& samples, T min_dist = traits::hue_max() / 10, std::size_t max_attempts = 200) {
    T h_max = traits::hue_max();

    for (std::size_t i = 0; i < max_attempts; ++i) {
      T candidate = this->random_value(T(0), h_max);
      bool ok = true;

      // Check distance against all existing samples
      for (T s : samples) {
        T diff = std::abs(candidate - s);
        T circular_diff = std::min(diff, h_max - diff);  // Handle wrap-around
        if (circular_diff < min_dist) {
          ok = false;
          break;
        }
      }

      if (ok) {
        samples.push_back(candidate);
        return construct_from_hue(this->next(), candidate, std::make_index_sequence<traits::size>{});
      }
    }

    // Fallback: return random color if max attempts exceeded
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

/** @brief Typedef for common HSLA model. */
using harmony_gen = harmony_generator<core::hsla_float_t>;

}  // namespace colorcpp::operations::random
