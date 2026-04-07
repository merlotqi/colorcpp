/**
 * @file lch.hpp
 * @brief CSS lch() color parsing.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/io/css/details.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

/**
 * @brief Parse lch(L C H [/ alpha]) — CSS Color Level 4.
 *
 * L: 0-100% or 0-100 (lightness)
 * C: 0-150 or 0-150% (chroma)
 * H: hue angle with units (deg, grad, rad, turn)
 * Optional alpha with /
 */
inline std::optional<core::cielch_t> parse_lch_function(details::Cursor& c) {
  if (!c.consume_ci("lch")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_space = [&]() -> std::optional<core::cielch_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};

    float L = 0.f;
    if (!d.try_consume_none()) {
      auto l_cv = d.parse_component_value();
      if (!l_cv) return std::nullopt;
      L = static_cast<float>(l_cv->first);
      if (l_cv->second) {
        L = std::clamp(L, 0.0f, 100.0f);
      } else {
        L = std::clamp(L, 0.0f, 100.0f);
      }
    }

    d.skip_ws();

    float C = 0.f;
    if (!d.try_consume_none()) {
      auto c_cv = d.parse_component_value();
      if (!c_cv) return std::nullopt;
      C = static_cast<float>(c_cv->first);
      if (c_cv->second) {
        C = std::clamp(C, 0.0f, 100.0f) * 1.5f;
      } else {
        C = std::clamp(C, 0.0f, 150.0f);
      }
    }

    d.skip_ws();

    float H = 0.f;
    if (!d.try_consume_none()) {
      auto h = d.parse_hue_angle();
      if (!h) return std::nullopt;
      H = static_cast<float>(*h);
    }

    d.skip_ws();

    // Parse optional alpha (cielch_t doesn't have alpha channel, so we parse but ignore it)
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      // Alpha is parsed but not used since cielch_t doesn't have an alpha channel
      (void)av;
    }

    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;

    c.i = d.i;
    return core::cielch_t{L, C, H};
  };

  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
