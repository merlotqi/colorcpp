/**
 * @file display_p3.hpp
 * @brief CSS color(display-p3 ...) parsing.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/io/css/details.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

/**
 * @brief Parse color(display-p3 r g b [/ alpha]) — CSS Color Level 4.
 *
 * r, g, b: 0-1 (can use % or number)
 * Optional alpha with /
 */
inline std::optional<core::display_p3f_t> parse_display_p3_function(details::Cursor& c) {
  if (!c.consume_ci("color")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  if (!c.consume_ci("display-p3")) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_space = [&]() -> std::optional<core::display_p3f_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};

    // Parse r: 0-1 (can be percentage or number)
    auto r_cv = d.parse_component_value();
    if (!r_cv) return std::nullopt;
    float r = static_cast<float>(r_cv->first);
    if (r_cv->second) {
      // Percentage: 0-100% maps to 0-1
      r = std::clamp(r, 0.0f, 100.0f) / 100.0f;
    } else {
      r = std::clamp(r, 0.0f, 1.0f);
    }

    d.skip_ws();

    // Parse g: 0-1 (can be percentage or number)
    auto g_cv = d.parse_component_value();
    if (!g_cv) return std::nullopt;
    float g = static_cast<float>(g_cv->first);
    if (g_cv->second) {
      g = std::clamp(g, 0.0f, 100.0f) / 100.0f;
    } else {
      g = std::clamp(g, 0.0f, 1.0f);
    }

    d.skip_ws();

    // Parse b: 0-1 (can be percentage or number)
    auto b_cv = d.parse_component_value();
    if (!b_cv) return std::nullopt;
    float b = static_cast<float>(b_cv->first);
    if (b_cv->second) {
      b = std::clamp(b, 0.0f, 100.0f) / 100.0f;
    } else {
      b = std::clamp(b, 0.0f, 1.0f);
    }

    d.skip_ws();

    // Parse optional alpha (display_p3f_t doesn't have alpha channel, so we parse but ignore it)
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      // Alpha is parsed but not used since display_p3f_t doesn't have an alpha channel
      (void)av;
    }

    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;

    c.i = d.i;
    return core::display_p3f_t{r, g, b};
  };

  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
