/**
 * @file lab.hpp
 * @brief CSS lab() color parsing.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/cielab.hpp>
#include <colorcpp/io/css/details.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

/**
 * @brief Parse lab(L a b [/ alpha]) — CSS Color Level 4.
 *
 * L: 0-100% or 0-100 (lightness)
 * a: -125 to 125 (green-red axis)
 * b: -125 to 125 (blue-yellow axis)
 * Optional alpha with /
 */
inline std::optional<core::cielab_t> parse_lab_function(details::Cursor& c) {
  if (!c.consume_ci("lab")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_space = [&]() -> std::optional<core::cielab_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};

    // Parse L (lightness): 0-100% or 0-100
    auto l_cv = d.parse_component_value();
    if (!l_cv) return std::nullopt;
    float L = static_cast<float>(l_cv->first);
    if (l_cv->second) {
      // Percentage: 0-100% maps to 0-100
      L = std::clamp(L, 0.0f, 100.0f);
    } else {
      L = std::clamp(L, 0.0f, 100.0f);
    }

    d.skip_ws();

    // Parse a: -125 to 125 (can be percentage or number)
    auto a_cv = d.parse_component_value();
    if (!a_cv) return std::nullopt;
    float A = static_cast<float>(a_cv->first);
    if (a_cv->second) {
      // Percentage: -100% to 100% maps to -125 to 125
      A = std::clamp(A, -100.0f, 100.0f) * 1.25f;
    } else {
      A = std::clamp(A, -125.0f, 125.0f);
    }

    d.skip_ws();

    // Parse b: -125 to 125 (can be percentage or number)
    auto b_cv = d.parse_component_value();
    if (!b_cv) return std::nullopt;
    float B = static_cast<float>(b_cv->first);
    if (b_cv->second) {
      // Percentage: -100% to 100% maps to -125 to 125
      B = std::clamp(B, -100.0f, 100.0f) * 1.25f;
    } else {
      B = std::clamp(B, -125.0f, 125.0f);
    }

    d.skip_ws();

    // Parse optional alpha (cielab_t doesn't have alpha channel, so we parse but ignore it)
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      // Alpha is parsed but not used since cielab_t doesn't have an alpha channel
      (void)av;
    }

    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;

    c.i = d.i;
    return core::cielab_t{L, A, B};
  };

  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
