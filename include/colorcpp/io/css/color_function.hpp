/**
 * @file color_function.hpp
 * @brief CSS @c color() function for the predefined CSS Color 4 spaces.
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

struct rgb3f {
  float x;
  float y;
  float z;
};

inline rgb3f mul3x3(const float m[9], float x, float y, float z) {
  return {
      m[0] * x + m[1] * y + m[2] * z,
      m[3] * x + m[4] * y + m[5] * z,
      m[6] * x + m[7] * y + m[8] * z,
  };
}

inline float sign_pow(float v, float exp) { return std::copysign(std::pow(std::fabs(v), exp), v); }

inline float gamma_decode_srgb_like(float v) {
  const float av = std::fabs(v);
  return std::copysign((av <= 0.04045f) ? (av / 12.92f) : std::pow((av + 0.055f) / 1.055f, 2.4f), v);
}

inline float gamma_encode_srgb_clamped(float v) {
  v = std::clamp(v, 0.0f, 1.0f);
  return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
}

inline float gamma_decode_a98(float v) { return sign_pow(v, 563.0f / 256.0f); }

inline float gamma_decode_prophoto(float v) {
  const float av = std::fabs(v);
  return std::copysign((av <= (16.0f / 512.0f)) ? (av / 16.0f) : std::pow(av, 1.8f), v);
}

inline float gamma_decode_rec2020(float v) {
  static constexpr float alpha = 1.0992968f;
  static constexpr float beta = 0.0180540f;
  const float av = std::fabs(v);
  if (av < beta * 4.5f) return v / 4.5f;
  return std::copysign(std::pow((av + alpha - 1.0f) / alpha, 1.0f / 0.45f), v);
}

inline rgb3f xyz_d50_to_d65(float x, float y, float z) {
  static constexpr float m[9] = {
      0.9554734f, -0.0230985f, 0.0632592f, -0.0283697f, 1.0099954f, 0.0210414f, 0.0123140f, -0.0205076f, 1.3303659f,
  };
  return mul3x3(m, x, y, z);
}

inline rgb3f linear_display_p3_to_xyz_d65(float r, float g, float b) {
  static constexpr float m[9] = {
      0.4865709f, 0.2656677f, 0.1982173f, 0.2289746f, 0.6917385f, 0.0792869f, 0.0000000f, 0.0451134f, 1.0439444f,
  };
  return mul3x3(m, r, g, b);
}

inline rgb3f linear_a98_to_xyz_d65(float r, float g, float b) {
  static constexpr float m[9] = {
      0.5766690f, 0.1855582f, 0.1882286f, 0.2973450f, 0.6273636f, 0.0752915f, 0.0270314f, 0.0706889f, 0.9913375f,
  };
  return mul3x3(m, r, g, b);
}

inline rgb3f linear_prophoto_to_xyz_d50(float r, float g, float b) {
  static constexpr float m[9] = {
      0.7977605f, 0.1351858f, 0.0313493f, 0.2880711f, 0.7118432f, 0.0000857f, 0.0000000f, 0.0000000f, 0.8251046f,
  };
  return mul3x3(m, r, g, b);
}

inline rgb3f linear_rec2020_to_xyz_d65(float r, float g, float b) {
  static constexpr float m[9] = {
      0.6369580f, 0.1446169f, 0.1688810f, 0.2627002f, 0.6779981f, 0.0593017f, 0.0000000f, 0.0280727f, 1.0609851f,
  };
  return mul3x3(m, r, g, b);
}

inline core::rgbaf_t xyz_d65_to_rgbaf(float x, float y, float z, float a) {
  const float lr = 3.2404542f * x - 1.5371385f * y - 0.4985314f * z;
  const float lg = -0.9692660f * x + 1.8760108f * y + 0.0415560f * z;
  const float lb = 0.0556434f * x - 0.2040259f * y + 1.0572252f * z;
  return core::rgbaf_t{
      gamma_encode_srgb_clamped(lr),
      gamma_encode_srgb_clamped(lg),
      gamma_encode_srgb_clamped(lb),
      std::clamp(a, 0.0f, 1.0f),
  };
}

inline core::rgbaf_t linear_srgb_to_rgbaf(float r, float g, float b, float a) {
  return core::rgbaf_t{
      gamma_encode_srgb_clamped(r),
      gamma_encode_srgb_clamped(g),
      gamma_encode_srgb_clamped(b),
      std::clamp(a, 0.0f, 1.0f),
  };
}

inline core::rgbaf_t linear_display_p3_to_rgbaf(float r, float g, float b, float a) {
  const auto xyz = linear_display_p3_to_xyz_d65(r, g, b);
  return xyz_d65_to_rgbaf(xyz.x, xyz.y, xyz.z, a);
}

inline core::rgbaf_t a98_to_rgbaf(float r, float g, float b, float a) {
  const auto xyz = linear_a98_to_xyz_d65(gamma_decode_a98(r), gamma_decode_a98(g), gamma_decode_a98(b));
  return xyz_d65_to_rgbaf(xyz.x, xyz.y, xyz.z, a);
}

inline core::rgbaf_t prophoto_to_rgbaf(float r, float g, float b, float a) {
  const auto xyz_d50 =
      linear_prophoto_to_xyz_d50(gamma_decode_prophoto(r), gamma_decode_prophoto(g), gamma_decode_prophoto(b));
  const auto xyz_d65 = xyz_d50_to_d65(xyz_d50.x, xyz_d50.y, xyz_d50.z);
  return xyz_d65_to_rgbaf(xyz_d65.x, xyz_d65.y, xyz_d65.z, a);
}

inline core::rgbaf_t rec2020_to_rgbaf(float r, float g, float b, float a) {
  const auto xyz = linear_rec2020_to_xyz_d65(gamma_decode_rec2020(r), gamma_decode_rec2020(g), gamma_decode_rec2020(b));
  return xyz_d65_to_rgbaf(xyz.x, xyz.y, xyz.z, a);
}

inline bool parse_channel_unbounded(details::Cursor& d, float& out) {
  if (d.try_consume_none()) {
    out = 0.f;
    return true;
  }
  auto cv = d.parse_component_value();
  if (!cv) return false;
  const double v = cv->second ? (cv->first / 100.0) : cv->first;
  out = static_cast<float>(v);
  return true;
}

inline bool parse_triplet_unbounded(details::Cursor& d, float& r, float& g, float& b) {
  if (!parse_channel_unbounded(d, r)) return false;
  d.skip_ws();
  if (!parse_channel_unbounded(d, g)) return false;
  d.skip_ws();
  return parse_channel_unbounded(d, b);
}

inline bool parse_triplet_xyz(details::Cursor& d, float& x, float& y, float& z) {
  return parse_triplet_unbounded(d, x, y, z);
}

inline std::optional<float> parse_optional_slash_alpha(details::Cursor& d) {
  if (!d.consume_char('/')) return 1.0f;
  auto av = d.parse_alpha_value();
  if (!av) return std::nullopt;
  return static_cast<float>(std::clamp(*av, 0.0, 1.0));
}

enum class Space {
  srgb_linear,
  srgb,
  display_p3_linear,
  display_p3,
  a98_rgb,
  prophoto_rgb,
  rec2020,
  xyz_d50,
  xyz_d65
};

inline std::optional<Space> parse_predefined_space(details::Cursor& d) {
  if (d.consume_ci("srgb-linear")) return Space::srgb_linear;
  if (d.consume_ci("srgb")) return Space::srgb;
  if (d.consume_ci("display-p3-linear")) return Space::display_p3_linear;
  if (d.consume_ci("display-p3")) return Space::display_p3;
  if (d.consume_ci("a98-rgb")) return Space::a98_rgb;
  if (d.consume_ci("prophoto-rgb")) return Space::prophoto_rgb;
  if (d.consume_ci("rec2020")) return Space::rec2020;
  if (d.consume_ci("xyz-d50")) return Space::xyz_d50;
  if (d.consume_ci("xyz-d65")) return Space::xyz_d65;
  if (d.consume_ci("xyz")) return Space::xyz_d65;
  return std::nullopt;
}

inline std::optional<core::rgbaf_t> parse_color_function_rgbaf_impl(details::Cursor& c) {
  if (!c.consume_ci("color")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  details::Cursor d{c.s, c.i};
  auto space_opt = parse_predefined_space(d);
  if (!space_opt) return std::nullopt;
  const Space space = *space_opt;

  d.skip_ws();

  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  if (space == Space::xyz_d50 || space == Space::xyz_d65) {
    if (!parse_triplet_xyz(d, r, g, b)) return std::nullopt;
  } else {
    if (!parse_triplet_unbounded(d, r, g, b)) return std::nullopt;
  }

  d.skip_ws();
  auto alpha_opt = parse_optional_slash_alpha(d);
  if (!alpha_opt) return std::nullopt;
  const float a = *alpha_opt;

  d.skip_ws();
  if (!d.consume_char(')')) return std::nullopt;
  d.skip_ws();
  if (!d.eof()) return std::nullopt;
  c.i = d.i;

  switch (space) {
    case Space::srgb_linear:
      return linear_srgb_to_rgbaf(r, g, b, a);
    case Space::srgb:
      return core::rgbaf_t{std::clamp(r, 0.0f, 1.0f), std::clamp(g, 0.0f, 1.0f), std::clamp(b, 0.0f, 1.0f),
                           std::clamp(a, 0.0f, 1.0f)};
    case Space::display_p3_linear:
      return linear_display_p3_to_rgbaf(r, g, b, a);
    case Space::display_p3:
      return linear_display_p3_to_rgbaf(gamma_decode_srgb_like(r), gamma_decode_srgb_like(g), gamma_decode_srgb_like(b),
                                        a);
    case Space::a98_rgb:
      return a98_to_rgbaf(r, g, b, a);
    case Space::prophoto_rgb:
      return prophoto_to_rgbaf(r, g, b, a);
    case Space::rec2020:
      return rec2020_to_rgbaf(r, g, b, a);
    case Space::xyz_d50: {
      const auto xyz = xyz_d50_to_d65(r, g, b);
      return xyz_d65_to_rgbaf(xyz.x, xyz.y, xyz.z, a);
    }
    case Space::xyz_d65:
      return xyz_d65_to_rgbaf(r, g, b, a);
  }

  return std::nullopt;
}

}  // namespace details_color_fn

/**
 * @brief Parse @c color( &lt;space&gt; coords [/ alpha] ) into sRGB 8-bit.
 *
 * Supported predefined spaces: @c srgb, @c srgb-linear, @c display-p3, @c display-p3-linear, @c a98-rgb,
 * @c prophoto-rgb, @c rec2020, @c xyz, @c xyz-d50, @c xyz-d65.
 */
inline std::optional<core::rgba8_t> parse_color_function_rgba8(details::Cursor& c) {
  auto out = details_color_fn::parse_color_function_rgbaf_impl(c);
  if (!out) return std::nullopt;
  return operations::conversion::color_cast<core::rgba8_t>(*out);
}

/**
 * @brief Same as @ref parse_color_function_rgba8 but keeps float sRGB (avoids 8-bit quantization for @c color()).
 */
inline std::optional<core::rgbaf_t> parse_color_function_rgbaf(details::Cursor& c) {
  return details_color_fn::parse_color_function_rgbaf_impl(c);
}

}  // namespace colorcpp::io::css
