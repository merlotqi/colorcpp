/**
 * @file fuzz_parse_css_color.cpp
 * @brief libFuzzer entry for the CSS color parser.
 *
 * The parser must be total on arbitrary byte input: reject malformed strings
 * with std::nullopt, never crash, never read out of bounds.
 */

#include <cstddef>
#include <cstdint>
#include <string_view>

#include <colorcpp/colorcpp.hpp>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
  const std::string_view input(reinterpret_cast<const char*>(data), size);

  // Typed entry points (concrete and relative color syntax).
  (void)colorcpp::io::css::parse_css_color<colorcpp::core::rgbaf_t>(input);
  (void)colorcpp::io::css::parse_css_color<colorcpp::core::rgba8_t>(input);

  // AST entry — parses without evaluating the ambient context.
  (void)colorcpp::io::css::parse_css_color_ast(input);

  return 0;
}
