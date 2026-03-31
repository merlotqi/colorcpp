/**
 * @file hex.hpp
 * @brief CSS hex color parsing (#RGB, #RRGGBB, #RGBA, #RRGGBBAA).
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css/details.hpp>
#include <optional>
#include <string_view>

namespace colorcpp::io::css {

inline std::optional<core::rgba8_t> parse_hex(std::string_view s) {
  if (s.empty() || s[0] != '#') return std::nullopt;
  s.remove_prefix(1);
  size_t len = s.size();

  auto get_val = [&](size_t i) -> std::optional<int> {
    int v = details::to_hex_digit(s[i]);
    if (v < 0) return std::nullopt;
    return v;
  };

  if (len == 3) {
    if (details::to_hex_digit(s[0]) < 0 || details::to_hex_digit(s[1]) < 0 ||
        details::to_hex_digit(s[2]) < 0)
      return std::nullopt;
    return core::rgba8_t{details::hex_expand(s[0]), details::hex_expand(s[1]),
                         details::hex_expand(s[2]), 255};
  }
  if (len == 4) {
    for (size_t i = 0; i < 4; ++i)
      if (details::to_hex_digit(s[i]) < 0) return std::nullopt;
    return core::rgba8_t{details::hex_expand(s[0]), details::hex_expand(s[1]),
                         details::hex_expand(s[2]), details::hex_expand(s[3])};
  }
  if (len == 6) {
    for (size_t i = 0; i < 6; ++i)
      if (details::to_hex_digit(s[i]) < 0) return std::nullopt;
    auto a0 = get_val(0), a1 = get_val(1), a2 = get_val(2), a3 = get_val(3), a4 = get_val(4),
         a5 = get_val(5);
    return core::rgba8_t{static_cast<uint8_t>((*a0 << 4) | *a1),
                         static_cast<uint8_t>((*a2 << 4) | *a3),
                         static_cast<uint8_t>((*a4 << 4) | *a5), 255};
  }
  if (len == 8) {
    for (size_t i = 0; i < 8; ++i)
      if (details::to_hex_digit(s[i]) < 0) return std::nullopt;
    auto v0 = get_val(0), v1 = get_val(1), v2 = get_val(2), v3 = get_val(3);
    auto v4 = get_val(4), v5 = get_val(5), v6 = get_val(6), v7 = get_val(7);
    return core::rgba8_t{static_cast<uint8_t>((*v0 << 4) | *v1),
                         static_cast<uint8_t>((*v2 << 4) | *v3),
                         static_cast<uint8_t>((*v4 << 4) | *v5),
                         static_cast<uint8_t>((*v6 << 4) | *v7)};
  }
  return std::nullopt;
}

}  // namespace colorcpp::io::css
