#pragma once

#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/core/rgb.hpp>
#include <cstddef>
#include <cstdint>

namespace color::literals {

namespace detail {
// 基础辅助：十六进制字符转整数
constexpr uint32_t hex_to_int(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return 0;
}

// 编译期十进制解析（支持数字分隔符 ' ）
template <char... Chars> constexpr uint64_t parse_dec() {
  constexpr char s[] = {Chars...};
  uint64_t res = 0;
  for (char c : s) {
    if (c >= '0' && c <= '9') {
      res = res * 10 + (c - '0');
    } else if (c == '\'') {
      continue; // 跳过 C++ 数字分隔符
    }
  }
  return res;
}

// 编译期十六进制解析
template <char... Chars> constexpr uint64_t parse_hex() {
  constexpr char s[] = {Chars...};
  constexpr size_t n = sizeof(s);
  size_t start = (n > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) ? 2 : 0;

  uint64_t res = 0;
  for (size_t i = start; i < n; ++i) {
    if (s[i] == '\'')
      continue;
    res = (res << 4) | hex_to_int(s[i]);
  }
  return res;
}
} // namespace detail

// --- 字面量实现 ---

// RGB/HEX 字面量 - 支持 0xFF0000_rgb 或 0xFF0000_hex
template <char... Chars> constexpr auto operator"" _rgb() {
  constexpr uint32_t val = static_cast<uint32_t>(detail::parse_hex<Chars...>());
  constexpr uint8_t r = (val >> 16) & 0xFF;
  constexpr uint8_t g = (val >> 8) & 0xFF;
  constexpr uint8_t b = val & 0xFF;
  return core::rgb8<r, g, b>{};
}

template <char... Chars> constexpr auto operator"" _hex() {
  return operator"" _rgb<Chars...>();
}

// HSV字面量 - 格式: H*10000 + S*100 + V (如 120100100_hsv)
template <char... Chars> constexpr auto operator"" _hsv() {
  constexpr uint64_t val = detail::parse_dec<Chars...>();
  constexpr int h = static_cast<int>(val / 10000);
  constexpr int s = static_cast<int>((val / 100) % 100);
  constexpr int v = static_cast<int>(val % 100);
  return core::hsv_int<h, s, v>{};
}

// HSL字面量 - 格式: H*10000 + S*100 + L (如 24010050_hsl)
template <char... Chars> constexpr auto operator"" _hsl() {
  constexpr uint64_t val = detail::parse_dec<Chars...>();
  constexpr int h = static_cast<int>(val / 10000);
  constexpr int s = static_cast<int>((val / 100) % 100);
  constexpr int l = static_cast<int>(val % 100);
  return core::hsl_int<h, s, l>{};
}

// 简化的HSV字面量 - 只指定色相
template <char... Chars> constexpr auto operator"" _hsv_hue() {
  constexpr int h = static_cast<int>(detail::parse_dec<Chars...>());
  return core::hsv_int<h, 100, 100>{};
}

// 简化的HSL字面量 - 只指定色相
template <char... Chars> constexpr auto operator"" _hsl_hue() {
  constexpr int h = static_cast<int>(detail::parse_dec<Chars...>());
  return core::hsl_int<h, 100, 50>{};
}

} // namespace color::literals