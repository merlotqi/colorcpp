/**
 * @file theme.hpp
 * @brief Semantic theme system with automatic dark mode generation.
 */

#pragma once

#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/algorithms/palette/details.hpp>

namespace colorcpp::operations::palette {

struct semantic_palette {
  core::rgba8_t primary;
  core::rgba8_t secondary;
  core::rgba8_t tertiary;
  core::rgba8_t error;
  core::rgba8_t surface;
  core::rgba8_t surface_variant;
  core::rgba8_t on_primary;
  core::rgba8_t on_secondary;
  core::rgba8_t on_surface;
  core::rgba8_t outline;
};

class theme {
 public:
  static theme from_seed(uint32_t seed) {
    return from_seed(core::rgba8_t{
        static_cast<uint8_t>((seed >> 16) & 0xFF),
        static_cast<uint8_t>((seed >> 8) & 0xFF),
        static_cast<uint8_t>(seed & 0xFF),
        0xFF
    });
  }
  static theme from_seed(core::rgba8_t seed) { return theme(seed); }

  semantic_palette light_mode() const { return derive_light_palette(seed_); }
  semantic_palette dark_mode() const { return derive_dark_palette(seed_); }
  core::rgba8_t seed() const { return seed_; }

 private:
  core::rgba8_t seed_;
  explicit theme(core::rgba8_t seed) : seed_(seed) {}

  static core::rgba8_t shift_hue(core::rgba8_t c, float degrees) {
    using namespace conversion;
    auto hsla = color_cast<core::hsla_float_t>(c);
    float new_h = algorithms::palette::details::rotate_hue(hsla.h(), degrees);
    return color_cast<core::rgba8_t>(algorithms::palette::details::modify_hue(hsla, new_h));
  }

  static core::rgba8_t adjust_lightness(core::rgba8_t c, float target_l) {
    auto hsla = operations::conversion::color_cast<core::hsla_float_t>(c);
    hsla.l() = std::clamp(target_l, 0.0f, 1.0f);
    return operations::conversion::color_cast<core::rgba8_t>(hsla);
  }

  static core::rgba8_t derive_on_color(core::rgba8_t bg, float threshold = 0.5f) {
    auto hsla = operations::conversion::color_cast<core::hsla_float_t>(bg);
    return hsla.l() > threshold ? core::rgba8_t{0x1C, 0x1B, 0x1F, 0xFF} : core::rgba8_t{0xFF, 0xFB, 0xFE, 0xFF};
  }

  static semantic_palette derive_light_palette(core::rgba8_t seed) {
    semantic_palette p;
    p.primary = seed;
    p.secondary = adjust_lightness(shift_hue(seed, 30), 0.40f);
    p.tertiary = adjust_lightness(shift_hue(seed, 90), 0.45f);
    p.error = core::rgba8_t{0xBA, 0x1A, 0x1A, 0xFF};
    p.surface = core::rgba8_t{0xFF, 0xFB, 0xFE, 0xFF};
    p.surface_variant = core::rgba8_t{0xF4, 0xEF, 0xF5, 0xFF};
    p.on_primary = derive_on_color(p.primary, 0.35f);
    p.on_secondary = derive_on_color(p.secondary, 0.40f);
    p.on_surface = core::rgba8_t{0x1C, 0x1B, 0x1F, 0xFF};
    p.outline = core::rgba8_t{0x79, 0x74, 0x7E, 0xFF};
    return p;
  }

  static semantic_palette derive_dark_palette(core::rgba8_t seed) {
    semantic_palette p;
    auto base_hsla = operations::conversion::color_cast<core::hsla_float_t>(seed);
    float target_l = std::clamp(base_hsla.l() + 0.30f, 0.60f, 0.85f);
    p.primary = base_hsla.l() < 0.4f ? adjust_lightness(seed, target_l)
                                     : operations::conversion::color_cast<core::rgba8_t>(base_hsla);
    p.secondary = adjust_lightness(shift_hue(seed, 30), 0.70f);
    p.tertiary = adjust_lightness(shift_hue(seed, 90), 0.75f);
    p.error = core::rgba8_t{0xFF, 0xB4, 0xAB, 0xFF};
    p.surface = core::rgba8_t{0x1C, 0x1B, 0x1F, 0xFF};
    p.surface_variant = core::rgba8_t{0x48, 0x44, 0x4E, 0xFF};
    p.on_primary = derive_on_color(p.primary, 0.5f);
    p.on_secondary = derive_on_color(p.secondary, 0.5f);
    p.on_surface = core::rgba8_t{0xE6, 0xE1, 0xE5, 0xFF};
    p.outline = core::rgba8_t{0x93, 0x8F, 0x99, 0xFF};
    return p;
  }
};

}  // namespace colorcpp::operations::palette
