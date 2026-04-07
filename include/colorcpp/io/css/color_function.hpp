/**
 * @file color_function.hpp
 * @brief CSS @c color() function: @c srgb, @c srgb-linear, @c xyz-d65, @c display-p3.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/linear_rgb.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <optional>

namespace colorcpp::io::css {

namespace details_color_fn {

/** P3 → sRGB via linear P3 → XYZ → linear sRGB (avoids a cyclic path in generic @c color_cast). */
inline core::rgbaf_t display_p3_channels_to_rgbaf(float r, float g, float b, float a) {
  const core::display_p3f_t p3{r, g, b};
  const auto lin_p3 = operations::conversion::color_cast<core::linear_display_p3f_t>(p3);
  const auto xyz = operations::conversion::color_cast<core::xyz_t>(lin_p3);
  const auto lin_srgb = operations::conversion::color_cast<core::linear_rgbf_t>(xyz);
  const auto srgb = operations::conversion::color_cast<core::rgbf_t>(lin_srgb);
  return core::rgbaf_t{srgb.r(), srgb.g(), srgb.b(), a};
}

inline bool parse_channel_01(details::Cursor& d, float& out) {
  if (d.try_consume_none()) {
    out = 0.f;
    return true;
  }
  auto cv = d.parse_component_value();
  if (!cv) return false;
  double v = cv->first;
  if (cv->second) {
    v = std::clamp(v, 0.0, 100.0) / 100.0;
  } else {
    v = std::clamp(v, 0.0, 1.0);
  }
  out = static_cast<float>(v);
  return true;
}

inline bool parse_triplet_01(details::Cursor& d, float& r, float& g, float& b) {
  if (!parse_channel_01(d, r)) return false;
  d.skip_ws();
  if (!parse_channel_01(d, g)) return false;
  d.skip_ws();
  return parse_channel_01(d, b);
}

/** CSS @c xyz-d65: numbers or percentages (map % to [0,2] span). */
inline bool parse_triplet_xyz(details::Cursor& d, float& x, float& y, float& z) {
  for (float* ch : {&x, &y, &z}) {
    if (d.try_consume_none()) {
      *ch = 0.f;
      d.skip_ws();
      continue;
    }
    auto cv = d.parse_component_value();
    if (!cv) return false;
    double v = cv->first;
    if (cv->second) {
      v = std::clamp(v, 0.0, 100.0) / 100.0 * 2.0;
    } else {
      v = std::clamp(v, 0.0, 2.0);
    }
    *ch = static_cast<float>(v);
    d.skip_ws();
  }
  return true;
}

inline std::optional<float> parse_optional_slash_alpha(details::Cursor& d) {
  if (!d.consume_char('/')) return 1.0f;
  auto av = d.parse_alpha_value();
  if (!av) return std::nullopt;
  return static_cast<float>(std::clamp(*av, 0.0, 1.0));
}

inline std::optional<core::rgba8_t> finish_color_paren(details::Cursor& c, size_t after_coords,
                                                       const core::rgbaf_t& premul) {
  details::Cursor d{c.s, after_coords};
  d.skip_ws();
  if (!d.consume_char(')')) return std::nullopt;
  d.skip_ws();
  if (!d.eof()) return std::nullopt;
  c.i = d.i;
  return operations::conversion::color_cast<core::rgba8_t>(premul);
}

}  // namespace details_color_fn

/**
 * @brief Parse @c color( &lt;space&gt; coords [/ alpha] ) into sRGB 8-bit (CSS Color 4 subset).
 *
 * Supported spaces: @c srgb, @c srgb-linear, @c xyz-d65, @c display-p3.
 */
inline std::optional<core::rgba8_t> parse_color_function_rgba8(details::Cursor& c) {
  if (!c.consume_ci("color")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  details::Cursor d{c.s, c.i};

  enum class Space {
    srgb_linear,
    srgb,
    xyz_d65,
    display_p3
  } space{};

  if (d.consume_ci("srgb-linear")) {
    space = Space::srgb_linear;
  } else if (d.consume_ci("srgb")) {
    space = Space::srgb;
  } else if (d.consume_ci("xyz-d65")) {
    space = Space::xyz_d65;
  } else if (d.consume_ci("display-p3")) {
    space = Space::display_p3;
  } else {
    return std::nullopt;
  }

  d.skip_ws();

  float r = 0, g = 0, b = 0;
  if (space == Space::xyz_d65) {
    if (!details_color_fn::parse_triplet_xyz(d, r, g, b)) return std::nullopt;
  } else {
    if (!details_color_fn::parse_triplet_01(d, r, g, b)) return std::nullopt;
  }

  d.skip_ws();
  auto alpha_opt = details_color_fn::parse_optional_slash_alpha(d);
  if (!alpha_opt) return std::nullopt;
  const float a = *alpha_opt;

  core::rgbaf_t premul{};
  switch (space) {
    case Space::srgb_linear: {
      core::linear_rgbaf_t lin{r, g, b, a};
      premul = operations::conversion::color_cast<core::rgbaf_t>(lin);
      break;
    }
    case Space::srgb: {
      premul = core::rgbaf_t{r, g, b, a};
      break;
    }
    case Space::xyz_d65: {
      core::xyz_t xyz{r, g, b};
      premul = operations::conversion::color_cast<core::rgbaf_t>(xyz);
      premul.a() = a;
      break;
    }
    case Space::display_p3:
      premul = details_color_fn::display_p3_channels_to_rgbaf(r, g, b, a);
      break;
  }

  return details_color_fn::finish_color_paren(c, d.i, premul);
}

/**
 * @brief Same as @ref parse_color_function_rgba8 but keeps float sRGB (avoids 8-bit quantization for @c color()).
 */
inline std::optional<core::rgbaf_t> parse_color_function_rgbaf(details::Cursor& c) {
  if (!c.consume_ci("color")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  details::Cursor d{c.s, c.i};

  enum class Space {
    srgb_linear,
    srgb,
    xyz_d65,
    display_p3
  } space{};

  if (d.consume_ci("srgb-linear")) {
    space = Space::srgb_linear;
  } else if (d.consume_ci("srgb")) {
    space = Space::srgb;
  } else if (d.consume_ci("xyz-d65")) {
    space = Space::xyz_d65;
  } else if (d.consume_ci("display-p3")) {
    space = Space::display_p3;
  } else {
    return std::nullopt;
  }

  d.skip_ws();

  float r = 0, g = 0, b = 0;
  if (space == Space::xyz_d65) {
    if (!details_color_fn::parse_triplet_xyz(d, r, g, b)) return std::nullopt;
  } else {
    if (!details_color_fn::parse_triplet_01(d, r, g, b)) return std::nullopt;
  }

  d.skip_ws();
  auto alpha_opt = details_color_fn::parse_optional_slash_alpha(d);
  if (!alpha_opt) return std::nullopt;
  const float a = *alpha_opt;

  core::rgbaf_t out{};
  switch (space) {
    case Space::srgb_linear: {
      core::linear_rgbaf_t lin{r, g, b, a};
      out = operations::conversion::color_cast<core::rgbaf_t>(lin);
      break;
    }
    case Space::srgb:
      out = core::rgbaf_t{r, g, b, a};
      break;
    case Space::xyz_d65: {
      core::xyz_t xyz{r, g, b};
      out = operations::conversion::color_cast<core::rgbaf_t>(xyz);
      out.a() = a;
      break;
    }
    case Space::display_p3:
      out = details_color_fn::display_p3_channels_to_rgbaf(r, g, b, a);
      break;
  }

  d.skip_ws();
  if (!d.consume_char(')')) return std::nullopt;
  d.skip_ws();
  if (!d.eof()) return std::nullopt;
  c.i = d.i;
  return out;
}

}  // namespace colorcpp::io::css
