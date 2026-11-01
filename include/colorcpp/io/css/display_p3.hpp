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
 *
 * @param alpha_out Optional sink for the parsed alpha in [0,1]; written only on
 *        success (see @ref parse_lab_function).
 */
inline std::optional<core::display_p3f_t> parse_display_p3_function(details::Cursor& c, float* alpha_out = nullptr) {
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

    // Parse optional alpha; display_p3f_t has no alpha channel, so hand it to
    // the caller through alpha_out (CSS default is opaque when "/" is omitted).
    float alpha = 1.0f;
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      alpha = std::clamp(static_cast<float>(*av), 0.0f, 1.0f);
    }

    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;

    c.i = d.i;
    if (alpha_out) *alpha_out = alpha;
    return core::display_p3f_t{r, g, b};
  };

  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
