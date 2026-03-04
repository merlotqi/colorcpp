/**
 * @file io.hpp
 * @brief Input/Output stream operators for color types
 *
 * Provides comprehensive stream I/O support for all color space types:
 * - RGBA (Red-Green-Blue-Alpha)
 * - HSLA (Hue-Saturation-Lightness-Alpha)
 * - HSVA (Hue-Saturation-Value-Alpha)
 * - CMYK (Cyan-Magenta-Yellow-Key)
 *
 * Supports multiple input formats and automatic type conversion for different
 * numeric types (uint8_t, int8_t, float, etc.).
 *
 * @author Merlot.Qi
 */

#pragma once

#include <cctype>
#include <color/core/cmyk.hpp>
#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/core/rgb.hpp>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <string_view>
#include <type_traits>

namespace color::core {

inline namespace io {

namespace details {

template <typename Ratio, typename T>
constexpr T get_scale_v() {
  return static_cast<T>(Ratio::num) / static_cast<T>(Ratio::den);
}

template <typename Ratio>
constexpr intptr_t get_full_range() {
  return Ratio::den / Ratio::num;
}

/**
 * @brief Convert color component to printable format
 * @tparam T Color component type
 * @param val Value to convert
 * @return Printable representation (int for uint8_t/int8_t, original type otherwise)
 * @note Ensures proper display of 8-bit integer types as integers rather than characters
 */
template <typename T>
constexpr auto to_printable(T val) {
  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
    return static_cast<int>(val);
  } else {
    return val;
  }
}

/**
 * @brief Read a color component from input stream
 * @tparam T Target type for the component
 * @param is Input stream to read from
 * @param out Reference to store the read value
 * @return true if reading succeeded, false otherwise
 * @note For uint8_t/int8_t types, reads as int and casts to target type
 * @note For other types, reads directly as the target type
 */
template <typename T>
bool read_comp(std::istream& is, T& out) {
  is >> std::ws;
  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
    int temp;
    if (!(is >> temp)) return false;
    out = static_cast<T>(temp);
  } else {
    if (!(is >> out)) return false;
  }
  return true;
}

/**
 * @brief Skip comma separator and whitespace in input stream
 * @param is Input stream to process
 * @note Advances stream past optional comma and surrounding whitespace
 */
inline void skip_sep(std::istream& is) {
  is >> std::ws;
  if (is.peek() == ',') is.ignore();
  is >> std::ws;
}

/**
 * @brief Consume optional color format header from input stream
 * @param is Input stream to read from
 * @param prefix Expected prefix string (e.g., "rgb", "hsl", "hsv", "cmyk")
 * @param has_paren Output parameter indicating if opening parenthesis was found
 * @return true if header was successfully consumed or not present
 * @note Supports both prefixed formats ("rgba(...)") and plain formats ("r, g, b, a")
 */
inline bool consume_header(std::istream& is, std::string_view prefix, bool& has_paren) {
  is >> std::ws;
  if (is.peek() == prefix[0]) {
    char ch;
    while (is.get(ch) && ch != '(' && !is.fail());
    has_paren = (ch == '(');
    return true;
  }
  has_paren = false;
  return true;
}

/**
 * @brief Check if more color components are available in input stream
 * @param is Input stream to check
 * @return true if more components are available, false otherwise
 * @note Checks for digits, signs (+/-), or comma separators
 */
inline bool has_more_components(std::istream& is) {
  is >> std::ws;
  int next = is.peek();
  return std::isdigit(next) || next == '+' || next == '-' || next == ',';
}

/**
 * @brief Write a color component to output stream with format support
 * @tparam T Color component type
 * @param os Output stream to write to
 * @param val Value to write
 * @note Supports both decimal and hexadecimal output formats
 * @note For hexadecimal format, pads integral types to 2 digits with leading zeros
 * @note For non-integral types in hex mode, writes directly without padding
 * @note For decimal format, uses standard to_printable() conversion
 * @example
 * // Decimal output (default)
 * write_comp(os, 255); // Writes: "255"
 *
 * // Hexadecimal output (with std::hex)
 * write_comp(os, 255); // Writes: "ff"
 */
template <typename T>
void write_comp(std::ostream& os, T val) {
  if (os.flags() & std::ios_base::hex) {
    if constexpr (std::is_integral_v<T>) {
      os << std::setfill('0') << std::setw(2) << static_cast<int>(val);
    } else {
      os << val;
    }
  } else {
    os << to_printable(val);
  }
}

}  // namespace details

/**
 * @brief Output stream operator for RGBA color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param os Output stream to write to
 * @param c RGBA color object to output
 * @return Reference to the output stream
 * @note Supports multiple output formats:
 *       - Decimal format (default): "rgba(r, g, b, a)"
 *       - Hexadecimal format (with std::hex): "#rrggbbaa"
 * @note Automatically converts 8-bit integer types to int for proper display in decimal mode
 * @note Hexadecimal format pads each component to 2 digits with leading zeros
 * @note Supports all numeric types with appropriate formatting
 * @note Preserves stream formatting state (fill character) when using hex format
 * @example
 * rgba8_t red(255, 0, 0, 255);
 * std::cout << red;                    // Outputs: rgba(255, 0, 0, 255)
 * std::cout << std::hex << red;        // Outputs: #ff0000ff
 *
 * rgbaf_t blue(0.0f, 0.0f, 1.0f, 1.0f);
 * std::cout << blue;                   // Outputs: rgba(0, 0, 1, 1)
 * std::cout << std::hex << blue;       // Outputs: #00000101 (if float converts to int)
 */
template <typename T, typename Scale>
std::ostream& operator<<(std::ostream& os, const basic_rgba<T, Scale>& c) {
  if (os.flags() & std::ios_base::hex) {
    auto old_fill = os.fill();
    os << "#";
    details::write_comp(os, c.r);
    details::write_comp(os, c.g);
    details::write_comp(os, c.b);
    details::write_comp(os, c.a);
    os.fill(old_fill);
    return os;
  }

  return os << "rgba(" << details::to_printable(c.r) << ", " << details::to_printable(c.g) << ", "
            << details::to_printable(c.b) << ", " << details::to_printable(c.a) << ")";
}

/**
 * @brief Input stream operator for RGBA color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param is Input stream to read from
 * @param c RGBA color object to populate
 * @return Reference to the input stream
 * @note Supports multiple input formats:
 *       - "rgba(r, g, b, a)" - Full RGBA format with optional "rgba" prefix
 *       - "r, g, b, a" - Simple comma-separated values
 *       - "r, g, b" - RGB only (alpha defaults to maximum value)
 * @note Whitespace is automatically skipped
 * @note Alpha channel defaults to 255 for 8-bit integers or Scale for other types
 * @note For uint8_t/int8_t types, input values are read as integers
 * @note For other types, input values are read as the target type
 * @example
 * basic_rgba<uint8_t> color;
 * std::cin >> color; // Accepts: "rgba(255, 0, 0, 255)" or "255, 0, 0, 255" or "255, 0, 0"
 *
 * basic_rgba<float, 1000> color;
 * std::cin >> color; // Accepts: "rgba(1000, 0, 0, 1000)" or "1000, 0, 0, 1000"
 */
template <typename T, typename Scale>
std::istream& operator>>(std::istream& is, basic_rgba<T, Scale>& c) {
  T r{0}, g{0}, b{0}, a;
  bool p;
  details::consume_header(is, "rgb", p);
  if (!details::read_comp(is, r)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, g)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, b)) return is;
  if (details::has_more_components(is)) {
    if (is.peek() == ',') is.ignore();
    details::read_comp(is, a);
  } else {
    if constexpr (std::is_integral_v<T>) {
      a = static_cast<T>(details::get_full_range<Scale>());
    } else {
      a = static_cast<T>(1);
    }
  }
  if (p) {
    char ch;
    while (is.get(ch) && ch != ')');
  }
  if (!is.fail()) c = basic_rgba<T, Scale>(r, g, b, a);
  return is;
}

/**
 * @brief Output stream operator for HSLA color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param os Output stream to write to
 * @param c HSLA color object to output
 * @return Reference to the output stream
 * @note Outputs in format: "hsla(h, s, l, a)"
 * @note Hue, saturation, lightness, and alpha are displayed with appropriate formatting
 * @note Automatically converts 8-bit integer types to int for proper display
 * @example
 * hsla_int<120, 50, 75, 100> green;
 * std::cout << green; // Outputs: hsla(120, 50, 75, 100)
 */
template <typename T, typename Scale>
std::ostream& operator<<(std::ostream& os, const basic_hsla<T, Scale>& c) {
  return os << "hsla(" << details::to_printable(c.h) << ", " << details::to_printable(c.s) << ", "
            << details::to_printable(c.l) << ", " << details::to_printable(c.a) << ")";
}

/**
 * @brief Input stream operator for HSLA color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param is Input stream to read from
 * @param c HSLA color object to populate
 * @return Reference to the input stream
 * @note Supports multiple input formats:
 *       - "hsla(h, s, l, a)" - Full HSLA format with optional "hsla" prefix
 *       - "h, s, l, a" - Simple comma-separated values
 *       - "h, s, l" - HSL only (alpha defaults to maximum value)
 * @note Hue, saturation, lightness, and alpha are read with appropriate type conversion
 * @note Whitespace is automatically skipped
 * @note Alpha channel defaults to Scale (typically 100% or 1.0)
 * @example
 * basic_hsla<int, 100> color;
 * std::cin >> color; // Accepts: "hsla(120, 50, 75, 100)" or "120, 50, 75, 100" or "120, 50, 75"
 */
template <typename T, typename Scale>
std::istream& operator>>(std::istream& is, basic_hsla<T, Scale>& c) {
  T h{0}, s{0}, l{0}, a;
  bool p;
  details::consume_header(is, "hsl", p);
  if (!details::read_comp(is, h)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, s)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, l)) return is;
  if (details::has_more_components(is)) {
    if (is.peek() == ',') is.ignore();
    details::read_comp(is, a);
  } else {
    if constexpr (std::is_integral_v<T>) {
      a = static_cast<T>(details::get_full_range<Scale>());
    } else {
      a = static_cast<T>(1);
    }
  }
  if (p) {
    char ch;
    while (is.get(ch) && ch != ')');
  }
  if (!is.fail()) c = basic_hsla<T, Scale>(h, s, l, a);
  return is;
}

/**
 * @brief Output stream operator for HSVA color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param os Output stream to write to
 * @param c HSVA color object to output
 * @return Reference to the output stream
 * @note Outputs in format: "hsva(h, s, v, a)"
 * @note Hue, saturation, value, and alpha are displayed with appropriate formatting
 * @note Automatically converts 8-bit integer types to int for proper display
 * @example
 * hsva_int<240, 100, 50, 100> blue;
 * std::cout << blue; // Outputs: hsva(240, 100, 50, 100)
 */
template <typename T, typename Scale>
std::ostream& operator<<(std::ostream& os, const basic_hsva<T, Scale>& c) {
  return os << "hsva(" << details::to_printable(c.h) << ", " << details::to_printable(c.s) << ", "
            << details::to_printable(c.v) << ", " << details::to_printable(c.a) << ")";
}

/**
 * @brief Input stream operator for HSVA color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param is Input stream to read from
 * @param c HSVA color object to populate
 * @return Reference to the input stream
 * @note Supports multiple input formats:
 *       - "hsva(h, s, v, a)" - Full HSVA format with optional "hsva" prefix
 *       - "h, s, v, a" - Simple comma-separated values
 *       - "h, s, v" - HSV only (alpha defaults to maximum value)
 * @note Hue, saturation, value, and alpha are read with appropriate type conversion
 * @note Whitespace is automatically skipped
 * @note Alpha channel defaults to Scale (typically 100% or 1.0)
 * @example
 * basic_hsva<int, 100> color;
 * std::cin >> color; // Accepts: "hsva(240, 100, 50, 100)" or "240, 100, 50, 100" or "240, 100, 50"
 */
template <typename T, typename Scale>
std::istream& operator>>(std::istream& is, basic_hsva<T, Scale>& c) {
  T h{0}, s{0}, v{0}, a;
  bool p;
  details::consume_header(is, "hsv", p);
  if (!details::read_comp(is, h)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, s)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, v)) return is;
  if (details::has_more_components(is)) {
    if (is.peek() == ',') is.ignore();
    details::read_comp(is, a);
  } else {
    if constexpr (std::is_integral_v<T>) {
      a = static_cast<T>(details::get_full_range<Scale>());
    } else {
      a = static_cast<T>(1);
    }
  }
  if (p) {
    char ch;
    while (is.get(ch) && ch != ')');
  }
  if (!is.fail()) c = basic_hsva<T, Scale>(h, s, v, a);
  return is;
}

/**
 * @brief Output stream operator for CMYK color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param os Output stream to write to
 * @param c CMYK color object to output
 * @return Reference to the output stream
 * @note Outputs in format: "cmyk(c, m, y, k)"
 * @note Cyan, magenta, yellow, and black components are displayed with appropriate formatting
 * @note Automatically converts 8-bit integer types to int for proper display
 * @example
 * cmyk_int<0, 100, 100, 0> red;
 * std::cout << red; // Outputs: cmyk(0, 100, 100, 0)
 */
template <typename T, typename Scale>
std::ostream& operator<<(std::ostream& os, const basic_cmyk<T, Scale>& c) {
  return os << "cmyk(" << details::to_printable(c.c) << ", " << details::to_printable(c.m) << ", "
            << details::to_printable(c.y) << ", " << details::to_printable(c.k) << ")";
}

/**
 * @brief Input stream operator for CMYK color
 * @tparam T Color component type (uint8_t, int8_t, float, etc.)
 * @tparam Scale Scaling factor for the color space
 * @param is Input stream to read from
 * @param c CMYK color object to populate
 * @return Reference to the input stream
 * @note Supports multiple input formats:
 *       - "cmyk(c, m, y, k)" - Full CMYK format with optional "cmyk" prefix
 *       - "c, m, y, k" - Simple comma-separated values
 * @note Cyan, magenta, yellow, and black components are read with appropriate type conversion
 * @note Whitespace is automatically skipped
 * @note All four components (C, M, Y, K) are required for CMYK input
 * @example
 * basic_cmyk<int, 100> color;
 * std::cin >> color; // Accepts: "cmyk(0, 100, 100, 0)" or "0, 100, 100, 0"
 */
template <typename T, typename Scale>
std::istream& operator>>(std::istream& is, basic_cmyk<T, Scale>& c) {
  T cv{0}, m{0}, y{0}, k{0};
  bool p;
  details::consume_header(is, "cmyk", p);
  if (!details::read_comp(is, cv)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, m)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, y)) return is;
  details::skip_sep(is);
  if (!details::read_comp(is, k)) return is;
  if (p) {
    char ch;
    while (is.get(ch) && ch != ')');
  }
  if (!is.fail()) c = basic_cmyk<T, Scale>(cv, m, y, k);
  return is;
}

}  // namespace io
}  // namespace color::core

/**
 * @brief Import I/O operators into core namespace for convenience
 *
 * This brings the stream operators into the core namespace so they can be used
 * directly with color types without explicit namespace qualification.
 */
namespace color::core {
using color::core::io::operator<<;
using color::core::io::operator>>;
}  // namespace color::core
