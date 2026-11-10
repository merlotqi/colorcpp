/**
 * @file theme.hpp
 * @brief Heuristic semantic theme system derived from a seed color.
 * Semantic roles are derived through heuristic seed transformations.
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/accessibility/wcag.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/palette/details.hpp>

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
  /** @brief Interpret @p seed as 0xRRGGBB and construct an opaque seed color with alpha set to 0xFF. */
  static theme from_seed(uint32_t seed) {
    return from_seed(core::rgba8_t{static_cast<uint8_t>((seed >> 16) & 0xFF), static_cast<uint8_t>((seed >> 8) & 0xFF),
                                   static_cast<uint8_t>(seed & 0xFF), 0xFF});
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
    float new_h = details::rotate_hue(hsla.h(), degrees);
    return color_cast<core::rgba8_t>(details::modify_hue(hsla, new_h));
  }

  static core::rgba8_t adjust_lightness(core::rgba8_t c, float target_l) {
    auto hsla = operations::conversion::color_cast<core::hsla_float_t>(c);
    hsla.l() = std::clamp(target_l, 0.0f, 1.0f);
    return operations::conversion::color_cast<core::rgba8_t>(hsla);
  }

  static core::rgba8_t mix_toward(core::rgba8_t base, core::rgba8_t extreme, float m) {
    const auto ch = [m](uint8_t x, uint8_t y) {
      return static_cast<uint8_t>(std::lround(static_cast<float>(x) * (1.0f - m) + static_cast<float>(y) * m));
    };
    return core::rgba8_t{ch(base.r(), extreme.r()), ch(base.g(), extreme.g()), ch(base.b(), extreme.b()), 0xFF};
  }

  /**
   * @brief Text ink for @p bg (issue #93).
   *
   * Picks the better-contrast brand ink (dark 0x1C1B1F vs light 0xFFFBFE) by
   * WCAG contrast ratio - not by HSL lightness, which misreads mid-tones and
   * produced on-colors as low as 1.33:1. When neither ink clears AA normal
   * text (4.5:1) on its own, darkens/lightens the appropriate brand ink toward
   * black/white by the minimal mix factor that does. The untouched brand ink
   * is returned whenever it already complies.
   */
  static core::rgba8_t derive_on_color(core::rgba8_t bg) {
    using algorithms::accessibility::contrast_ratio;
    using algorithms::accessibility::is_wcag_compliant;
    const core::rgba8_t kDarkInk{0x1C, 0x1B, 0x1F, 0xFF};
    const core::rgba8_t kLightInk{0xFF, 0xFB, 0xFE, 0xFF};
    const core::rgba8_t kBlack{0x00, 0x00, 0x00, 0xFF};
    const core::rgba8_t kWhite{0xFF, 0xFF, 0xFF, 0xFF};

    const float c_dark = contrast_ratio(kDarkInk, bg);
    const float c_light = contrast_ratio(kLightInk, bg);
    const bool dark_ok = is_wcag_compliant(kDarkInk, bg);
    const bool light_ok = is_wcag_compliant(kLightInk, bg);
    if (dark_ok || light_ok) {
      if (dark_ok && light_ok) return c_dark >= c_light ? kDarkInk : kLightInk;
      return dark_ok ? kDarkInk : kLightInk;
    }

    // Mid-tone background: neither brand ink clears AA alone. The adjustment
    // direction must be one whose extreme (black or white) can actually reach
    // 4.5:1 here - the higher-contrast brand ink is not always on that side
    // (e.g. 0x787878: light ink leads 4.37:1 but even pure white tops out at
    // 4.48:1, while black reaches 4.69:1). At least one extreme always can,
    // and when both can, follow the better brand ink's side.
    const bool black_ok = is_wcag_compliant(kBlack, bg);
    const bool dark_dir = (black_ok && is_wcag_compliant(kWhite, bg)) ? (c_dark >= c_light) : black_ok;
    const core::rgba8_t base = dark_dir ? kDarkInk : kLightInk;
    const core::rgba8_t extreme = dark_dir ? kBlack : kWhite;

    // Binary-search the minimal mix factor whose quantized result complies.
    float lo = 0.0f;
    float hi = 1.0f;
    for (int i = 0; i < 24; ++i) {
      const float m = 0.5f * (lo + hi);
      if (is_wcag_compliant(mix_toward(base, extreme, m), bg))
        hi = m;
      else
        lo = m;
    }
    return mix_toward(base, extreme, hi);
  }

  static semantic_palette derive_light_palette(core::rgba8_t seed) {
    semantic_palette p;
    p.primary = seed;
    p.secondary = adjust_lightness(shift_hue(seed, 30), 0.40f);
    p.tertiary = adjust_lightness(shift_hue(seed, 90), 0.45f);
    p.error = core::rgba8_t{0xBA, 0x1A, 0x1A, 0xFF};
    p.surface = core::rgba8_t{0xFF, 0xFB, 0xFE, 0xFF};
    p.surface_variant = core::rgba8_t{0xF4, 0xEF, 0xF5, 0xFF};
    p.on_primary = derive_on_color(p.primary);
    p.on_secondary = derive_on_color(p.secondary);
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
    p.on_primary = derive_on_color(p.primary);
    p.on_secondary = derive_on_color(p.secondary);
    p.on_surface = core::rgba8_t{0xE6, 0xE1, 0xE5, 0xFF};
    p.outline = core::rgba8_t{0x93, 0x8F, 0x99, 0xFF};
    return p;
  }
};

}  // namespace colorcpp::operations::palette
