/**
 * @file oklab.hpp
 * @brief CSS oklab() color parsing.
 */

#pragma once

#include <algorithm>
#include <colorcpp/core/oklab.hpp>
#include <colorcpp/io/css/details.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

/**
 * @brief Parse oklab(L a b [/ alpha]) — CSS Color Level 4.
 *
 * L: 0-100% or 0-1
 * a, b: -0.5 to 0.5 (can use % or number)
 * Optional alpha with /
 */
inline std::optional<core::oklab_t> parse_oklab_function(details::Cursor& c) {
  if (!c.consume_ci("oklab")) return std::nullopt;
  c.skip_ws();
  if (!c.consume_char('(')) return std::nullopt;
  c.skip_ws();

  size_t checkpoint = c.i;

  auto try_space = [&]() -> std::optional<core::oklab_t> {
    c.i = checkpoint;
    details::Cursor d{c.s, c.i};

    float L = 0.f;
    if (!d.try_consume_none()) {
      auto l_cv = d.parse_component_value();
      if (!l_cv) return std::nullopt;
      L = static_cast<float>(l_cv->first);
      if (l_cv->second) {
        L = std::clamp(L, 0.0f, 100.0f) / 100.0f;
      } else {
        L = std::clamp(L, 0.0f, 1.0f);
      }
    }

    d.skip_ws();

    float A = 0.f;
    if (!d.try_consume_none()) {
      auto a_cv = d.parse_component_value();
      if (!a_cv) return std::nullopt;
      A = static_cast<float>(a_cv->first);
      if (a_cv->second) {
        A = std::clamp(A, -100.0f, 100.0f) / 200.0f;
      } else {
        A = std::clamp(A, -0.5f, 0.5f);
      }
    }

    d.skip_ws();

    float B = 0.f;
    if (!d.try_consume_none()) {
      auto b_cv = d.parse_component_value();
      if (!b_cv) return std::nullopt;
      B = static_cast<float>(b_cv->first);
      if (b_cv->second) {
        B = std::clamp(B, -100.0f, 100.0f) / 200.0f;
      } else {
        B = std::clamp(B, -0.5f, 0.5f);
      }
    }

    d.skip_ws();

    // Parse optional alpha (oklab_t doesn't have alpha channel, so we parse but ignore it)
    if (d.consume_char('/')) {
      auto av = d.parse_alpha_value();
      if (!av) return std::nullopt;
      // Alpha is parsed but not used since oklab_t doesn't have an alpha channel
      (void)av;
    }

    d.skip_ws();
    if (!d.consume_char(')')) return std::nullopt;
    d.skip_ws();
    if (!d.eof()) return std::nullopt;

    c.i = d.i;
    return core::oklab_t{L, A, B};
  };

  if (auto r = try_space()) return r;
  return std::nullopt;
}

}  // namespace colorcpp::io::css
