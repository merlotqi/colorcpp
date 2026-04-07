/**
 * @file basic.hpp
 * @brief Basic random color generator with configurable engine.
 *
 * @par Thread safety
 * Not thread-safe: the engine is `mutable` and `const` methods (`next`, `random_value`) advance it. Do not share one
 * instance across threads without synchronization.
 *
 * @par Floating-point ranges
 * `std::uniform_real_distribution` yields values in **[a, b)**; the upper bound is not included (negligible for floats).
 *
 * @par Lab-like spaces
 * Independent uniform channels define a box in that space; results may lie outside common RGB display gamuts.
 */

#pragma once

#include <colorcpp/operations/random/details.hpp>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

namespace colorcpp::operations::random {

/**
 * @brief Base: uniform random values per channel within model min/max.
 *
 * The generator maintains an internal copy of the random engine.
 * This ensures reproducibility when using the same seed, while avoiding
 * shared state issues with external engines.
 *
 * @tparam Color Color type.
 * @tparam Engine Random engine type (default: std::mt19937).
 *
 * @note `next() const` still consumes randomness (see file-level thread-safety note).
 */
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
  using engine_type = Engine;

  /**
   * @brief Construct from existing engine (copies engine state).
   * @param e Engine to copy.
   */
  explicit basic_random_generator(const Engine& e) : rng(e) {}

  /**
   * @brief Construct from engine seed.
   * @param seed Seed for the random engine.
   */
  explicit basic_random_generator(typename Engine::result_type seed) : rng(seed) {}

  /**
   * @brief Construct by moving engine.
   * @param e Engine to move.
   */
  explicit basic_random_generator(Engine&& e) : rng(std::move(e)) {}

  // Copy constructor
  basic_random_generator(const basic_random_generator&) = default;

  // Move constructor
  basic_random_generator(basic_random_generator&&) = default;

  // Copy assignment
  basic_random_generator& operator=(const basic_random_generator&) = default;

  // Move assignment
  basic_random_generator& operator=(basic_random_generator&&) = default;

  /**
   * @brief Generate a random value in [min, max].
   * @param min Minimum value (inclusive).
   * @param max Maximum value (inclusive).
   * @return Random value in range.
   */
  T random_value(T min, T max) const {
    dist_t dist(static_cast<typename dist_t::result_type>(min), static_cast<typename dist_t::result_type>(max));
    return static_cast<T>(dist(rng));
  }

  /**
   * @brief Get a reference to the internal engine (const).
   * @return Const reference to the engine.
   */
  const Engine& engine() const { return rng; }

  /**
   * @brief Get a reference to the internal engine (mutable).
   * @return Mutable reference to the engine.
   */
  Engine& engine() { return rng; }

 public:
  /**
   * @brief Generate a single random color.
   * @return Random color with all channels at random values.
   */
  Color next() const { return generate_full_random(std::make_index_sequence<traits::size>{}); }

  /**
   * @brief Generate multiple random colors.
   * @param count Number of colors to generate.
   * @return Vector of random colors.
   */
  std::vector<Color> generate_n(std::size_t count) const {
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(next());
    return out;
  }

 protected:
  template <std::size_t... Is>
  Color generate_full_random(std::index_sequence<Is...>) const {
    return Color{this->random_value(traits::template min_at<Is>(), traits::template max_at<Is>())...};
  }
};

}  // namespace colorcpp::operations::random
