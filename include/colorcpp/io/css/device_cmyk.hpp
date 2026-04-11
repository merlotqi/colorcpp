/**
 * @file device_cmyk.hpp
 * @brief CSS @c device-cmyk() subset (four components plus optional alpha).
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <optional>

namespace colorcpp::io::css {

inline std::optional<core::rgbaf_t> parse_device_cmyk_function_rgbaf(details::Cursor& c) {
  if (!c.consume_ci("device-cmyk")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  details::Cursor d{c.s, c.i};
  float cc = 0, m = 0, y = 0, k = 0;
  for (float* ch : {&cc, &m, &y, &k}) {
    if (d.try_consume_none()) {
      *ch = 0.f;
    } else {
      auto cv = d.parse_component_value();
      if (!cv) return std::nullopt;
      double v = cv->first;
      if (cv->second) {
        v = std::clamp(v, 0.0, 100.0) / 100.0;
      } else {
        v = std::clamp(v, 0.0, 1.0);
      }
      *ch = static_cast<float>(v);
    }
    d.skip_ws();
  }

  float alpha = 1.0f;
  if (d.consume_char('/')) {
    auto av = d.parse_alpha_value();
    if (!av) return std::nullopt;
    alpha = static_cast<float>(std::clamp(*av, 0.0, 1.0));
  }
  d.skip_ws();
  if (!d.consume_char(')')) return std::nullopt;
  d.skip_ws();
  if (!d.eof()) return std::nullopt;
  c.i = d.i;

  core::cmyk_float_t ink{cc, m, y, k};
  auto out = operations::conversion::color_cast<core::rgbaf_t>(ink);
  out.a() = alpha;
  return out;
}

inline std::optional<core::rgba8_t> parse_device_cmyk_function(details::Cursor& c) {
  auto out = parse_device_cmyk_function_rgbaf(c);
  if (!out) return std::nullopt;
  return operations::conversion::color_cast<core::rgba8_t>(*out);
}

}  // namespace colorcpp::io::css
