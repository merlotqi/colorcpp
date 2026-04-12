/**
 * @file details.hpp
 * @brief Internal helpers for binary IO: file parsing, float reading.
 */

#pragma once

#include <cctype>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace colorcpp::io::binary_io::details {

/**
 * @brief Check if a character is whitespace.
 */
inline bool is_space(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f'; }

/**
 * @brief Trim leading/trailing whitespace from a string_view.
 */
inline void trim(std::string_view& s) {
  while (!s.empty() && is_space(s.front())) s.remove_prefix(1);
  while (!s.empty() && is_space(s.back())) s.remove_suffix(1);
}

/**
 * @brief Check if a string_view starts with a prefix (case-insensitive).
 */
inline bool starts_with_ci(std::string_view s, std::string_view prefix) {
  if (s.size() < prefix.size()) return false;
  for (std::size_t i = 0; i < prefix.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(s[i])) != std::tolower(static_cast<unsigned char>(prefix[i])))
      return false;
  }
  return true;
}

/**
 * @brief Parse a float from a string_view.
 */
inline std::optional<float> parse_float(std::string_view s) {
  trim(s);
  if (s.empty()) return std::nullopt;
  float result;

#if defined(__APPLE__)
  char* end_ptr = nullptr;
  result = std::strtof(s.data(), &end_ptr);

  if (end_ptr == s.data() || end_ptr != s.data() + s.size() || result == HUGE_VALF || result == -HUGE_VALF) {
    return std::nullopt;
  }
#else
  auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
  if (ec != std::errc{} || ptr != s.data() + s.size()) return std::nullopt;
#endif

  return result;
}

/**
 * @brief Parse an integer from a string_view.
 */
inline std::optional<int> parse_int(std::string_view s) {
  trim(s);
  if (s.empty()) return std::nullopt;
  int result;
  auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
  if (ec != std::errc{} || ptr != s.data() + s.size()) return std::nullopt;
  return result;
}

/**
 * @brief Split a string_view by whitespace, returning non-empty tokens.
 */
inline std::vector<std::string_view> split_ws(std::string_view s) {
  std::vector<std::string_view> tokens;
  std::size_t i = 0;
  while (i < s.size()) {
    while (i < s.size() && is_space(s[i])) ++i;
    if (i >= s.size()) break;
    std::size_t start = i;
    while (i < s.size() && !is_space(s[i])) ++i;
    tokens.push_back(s.substr(start, i - start));
  }
  return tokens;
}

/**
 * @brief Read a line from an input stream, returning it without trailing newline.
 */
inline std::optional<std::string> read_line(std::istream& is) {
  std::string line;
  if (!std::getline(is, line)) return std::nullopt;
  // Remove trailing \r if present
  if (!line.empty() && line.back() == '\r') line.pop_back();
  return line;
}

}  // namespace colorcpp::io::binary_io::details
