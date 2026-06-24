/**
 * @file luminance.hpp
 * @brief OkLab lightness-constrained random color generator.
 *
 * Generates random colors constrained to a specific OkLab perceptual
 * lightness range. Uses OkLCH polar coordinates for candidate generation
 * with optional gamut mapping.
 *
 * @par Thread safety
 * Not thread-safe (inherited mutable engine). Same as @ref basic_random_generator.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/gamut/preserve.hpp>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/random/basic.hpp>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::random {

/**
 * @brief Random color generator constrained to an OkLab lightness range.
 *
 * Generates candidates in OkLCH polar coordinates (L, C, H), converts to
 * the target color space, and optionally gamut-maps to sRGB. The OkLab
 * lightness L is preserved by the gamut mapper.
 *
 * @tparam Color Target output color type.
 * @tparam Engine Random engine type (default: std::mt19937).
 */
template <typename Color, typename Engine = std::mt19937>
class luminance_generator : public basic_random_generator<Color, Engine> {
  using base = basic_random_generator<Color, Engine>;
  using T = typename base::T;

 public:
  /** @brief Generation options. */
  struct options {
    float l_min = 0.0f;       ///< OkLab lightness lower bound [0, 1].
    float l_max = 1.0f;       ///< OkLab lightness upper bound [0, 1].
    float chroma_max = 0.4f;  ///< OkLCH chroma upper bound [0, 0.4].
    bool gamut_map = true;    ///< Clip to sRGB gamut via lightness-preserving mapper.
  };

  explicit luminance_generator(const Engine& e, const options& o = {}) : base(e), opts_(o) { validate(opts_); }

  explicit luminance_generator(typename Engine::result_type seed, const options& o = {}) : base(seed), opts_(o) {
    validate(opts_);
  }

  explicit luminance_generator(Engine&& e, const options& o = {}) : base(std::move(e)), opts_(o) { validate(opts_); }

  /**
   * @brief Generate a single random color within the lightness range.
   * @return Random color.
   */
  Color next() const { return generate_candidate(); }

  /**
   * @brief Generate multiple random colors within the lightness range.
   * @param count Number of colors to generate.
   * @return Vector of random colors.
   */
  std::vector<Color> generate_n(std::size_t count) const {
    std::vector<Color> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) out.push_back(next());
    return out;
  }

  const options& get_options() const { return opts_; }

  void set_options(const options& o) {
    validate(o);
    opts_ = o;
  }

 private:
  static constexpr float kPi = 3.14159265358979323846f;

  static void validate(const options& o) {
    if (o.l_min > o.l_max) throw std::invalid_argument("colorcpp: l_min must not exceed l_max");
    if (o.chroma_max < 0.0f) throw std::invalid_argument("colorcpp: chroma_max must be non-negative");
  }

  Color generate_candidate() const {
    const float L = this->random_value(opts_.l_min, opts_.l_max);
    const float C = this->random_value(0.0f, opts_.chroma_max);
    const float H = this->random_value(0.0f, 360.0f);

    const float H_rad = H * kPi / 180.0f;
    const float a = C * std::cos(H_rad);
    const float b = C * std::sin(H_rad);

    core::oklab_t lab{L, a, b};
    Color result = conversion::color_cast<Color>(lab);

    if (opts_.gamut_map) {
      result = algorithms::gamut::gamut_clip_preserve_lightness(result);
    }
    return result;
  }

  options opts_;
};

}  // namespace colorcpp::operations::random
