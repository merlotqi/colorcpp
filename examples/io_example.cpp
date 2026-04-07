/**
 * @file io_example.cpp
 * @brief Example: color input/output with ANSI terminal color output.
 */

#include <colorcpp/colorcpp.hpp>
#include <colorcpp/io/ansi.hpp>
#include <iostream>
#include <sstream>

using namespace colorcpp;
using namespace colorcpp::core;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::literals;
using namespace colorcpp::io::css;

static void section(const char* title) {
  std::cout << "\n" << ::colorcpp::io::ansi::bold() << "── " << title << " ──" << ::colorcpp::io::ansi::reset() << "\n";
}
template <typename Color>
Color parse(const std::string& s) {
  std::istringstream ss(s);
  Color c{};
  if (!(ss >> c)) std::cout << "  [parse failed for: \"" << s << "\"]\n";
  return c;
}

int main() {
  using namespace colorcpp::io::ansi;

  // 1. Output formats
  section("RGB output: text vs hex");

  rgba8_t coral = constants::coral;
  rgb8_t coral3 = color_cast<rgb8_t>(coral);

  std::cout << "rgba8  text: ";
  print_swatch(std::cout, coral, 6);
  std::cout << " " << coral << "\n";
  std::cout << "rgb8   text: ";
  print_swatch(std::cout, coral3, 6);
  std::cout << " " << coral3 << "\n";
  std::cout << "rgba8  hex:  ";
  print_swatch(std::cout, coral, 6);
  std::cout << " " << std::hex << coral << "\n";
  std::cout << "rgb8   hex:  " << coral3 << "\n" << std::dec;

  // 2. All color model output
  section("All color models: output");

  hsl_float_t hsl = color_cast<hsl_float_t>(coral);
  hsla_float_t hsla = color_cast<hsla_float_t>(coral);
  hsv_float_t hsv = color_cast<hsv_float_t>(coral);
  hsva_float_t hsva = color_cast<hsva_float_t>(coral);
  cmyk8_t cmyk = color_cast<cmyk8_t>(coral);

  std::cout << "hsl:   ";
  print_color_verbose(std::cout, coral);
  std::cout << "\n";
  std::cout << "hsla:  ";
  print_color_verbose(std::cout, hsla);
  std::cout << "\n";
  std::cout << "hsv:   ";
  print_color_verbose(std::cout, hsv);
  std::cout << "\n";
  std::cout << "hsva:  ";
  print_color_verbose(std::cout, hsva);
  std::cout << "\n";
  std::cout << "cmyk:  ";
  print_color_verbose(std::cout, cmyk);
  std::cout << "\n";

  // Hex output for non-RGB types normalises each channel to [0,255].
  std::cout << "hsl hex:  " << std::hex << hsl << "\n";
  std::cout << "cmyk hex: " << cmyk << "\n" << std::dec;

  // 3. Literals
  section("Literals");

  auto lit_rgb = 0xFF6347_rgb;           // -> rgba8_t{255,99,71,255}
  auto lit_rgba = 0xFF634780_rgba;       // -> rgba8_t{255,99,71,128}
  auto lit_argb = 0x80FF6347_argb;       // -> rgba8_t{255,99,71,128}  (AARRGGBB)
  auto lit_hex = "#FF6347"_hex;          // -> rgba8_t, shorthand string
  auto lit_hex3 = "#F63"_hex;            // shorthand #RGB -> #FF6633
  auto lit_hsl = 120'050'075_hsl;        // hsl_float_t{120, 50, 75}
  auto lit_hsla = 120'050'075'100_hsla;  // hsla_float_t{120, 50, 75, 100}
  auto lit_hsv = 210'080'090_hsv;        // hsv_float_t{210, 80, 90}
  auto lit_hsva = 210'080'090'075_hsva;  // hsva_float_t{210, 80, 90, 75}
  auto lit_cmyk = 50'030'000'020_cmyk;   // cmyk8_t{50, 30, 0, 20}

  std::cout << std::hex;
  std::cout << "0xFF6347_rgb:          ";
  print_swatch(std::cout, lit_rgb, 4);
  std::cout << " " << lit_rgb << "\n";
  std::cout << "0xFF634780_rgba:       ";
  print_swatch(std::cout, lit_rgba, 4);
  std::cout << " " << lit_rgba << "\n";
  std::cout << "0x80FF6347_argb:       ";
  print_swatch(std::cout, lit_argb, 4);
  std::cout << " " << lit_argb << "\n";
  std::cout << "\"#FF6347\"_hex:         ";
  print_swatch(std::cout, lit_hex, 4);
  std::cout << " " << lit_hex << "\n";
  std::cout << "\"#F63\"_hex:            ";
  print_swatch(std::cout, lit_hex3, 4);
  std::cout << " " << lit_hex3 << "\n";
  std::cout << std::dec;
  std::cout << "120'050'075_hsl:       ";
  print_swatch(std::cout, lit_hsl, 4);
  std::cout << " " << lit_hsl << "\n";
  std::cout << "120'050'075'100_hsla:  ";
  print_swatch(std::cout, lit_hsla, 4);
  std::cout << " " << lit_hsla << "\n";
  std::cout << "210'080'090_hsv:       ";
  print_swatch(std::cout, lit_hsv, 4);
  std::cout << " " << lit_hsv << "\n";
  std::cout << "210'080'090'075_hsva:  ";
  print_swatch(std::cout, lit_hsva, 4);
  std::cout << " " << lit_hsva << "\n";
  std::cout << "50'030'000'020_cmyk:   ";
  print_swatch(std::cout, lit_cmyk, 4);
  std::cout << " " << lit_cmyk << "\n";

  // 4. Parsing from strings
  section("Input: parsing from string");

  // RGB: bare numbers (no prefix)
  auto p_rgb8 = parse<rgb8_t>("255 99 71");
  auto p_rgba8 = parse<rgba8_t>("255 99 71 200");

  // HSL: hue [0, 360], saturation [0, 1], lightness [0, 1]
  // Note: S and L are NOT percentages -- they are in [0, 1] range
  auto p_hsl_prefix = parse<hsl_float_t>("hsl(16, 0.788, 0.639)");
  auto p_hsl_bare = parse<hsl_float_t>("16 0.788 0.639");

  // HSLA: prefix form, alpha [0, 1] optional - missing alpha defaults to channel max
  auto p_hsla_full = parse<hsla_float_t>("hsla(16, 0.788, 0.639, 0.80)");
  auto p_hsla_no_alpha = parse<hsla_float_t>("hsla(16, 0.788, 0.639)");

  // HSV
  auto p_hsv = parse<hsv_float_t>("hsv(16, 1, 1)");

  // CMYK
  auto p_cmyk = parse<cmyk8_t>("cmyk(0, 61, 72, 0)");

  std::cout << "rgb8  \"255 99 71\":            ";
  print_swatch(std::cout, p_rgb8, 4);
  std::cout << " " << std::hex << p_rgb8 << std::dec << "\n";
  std::cout << "rgba8 \"255 99 71 200\":        ";
  print_swatch(std::cout, p_rgba8, 4);
  std::cout << " " << std::hex << p_rgba8 << std::dec << "\n";
  std::cout << "hsl   \"hsl(16,0.788,0.639)\": ";
  print_swatch(std::cout, p_hsl_prefix, 4);
  std::cout << " " << p_hsl_prefix << "\n";
  std::cout << "hsl   \"16 0.788 0.639\":      ";
  print_swatch(std::cout, p_hsl_bare, 4);
  std::cout << " " << p_hsl_bare << "\n";
  std::cout << "hsla  full:                    ";
  print_swatch(std::cout, p_hsla_full, 4);
  std::cout << " " << p_hsla_full << "\n";
  std::cout << "hsla  no alpha (-> A=1.0):     ";
  print_swatch(std::cout, p_hsla_no_alpha, 4);
  std::cout << " " << p_hsla_no_alpha << "\n";
  std::cout << "hsv   \"hsv(16,100,100)\":      ";
  print_swatch(std::cout, p_hsv, 4);
  std::cout << " " << p_hsv << "\n";
  std::cout << "cmyk  \"cmyk(0,61,72,0)\":      ";
  print_swatch(std::cout, p_cmyk, 4);
  std::cout << " " << p_cmyk << "\n";

  // 4b. CSS Color 4-style strings (parse_css_color_rgba8)
  section("CSS strings: parse_css_color_rgba8");
  auto css_hex = parse_css_color_rgba8("#FF6347");
  auto css_rgb = parse_css_color_rgba8("rgb(255 99 71 / 80%)");
  auto css_hsl = parse_css_color_rgba8("hsl(0, 100%, 50%)");
  if (css_hex) {
    std::cout << "  ";
    print_swatch(std::cout, *css_hex, 4);
    std::cout << " #FF6347     -> " << std::hex << *css_hex << std::dec << "\n";
  }
  if (css_rgb) {
    std::cout << "  ";
    print_swatch(std::cout, *css_rgb, 4);
    std::cout << " rgb(.../80%) -> " << std::hex << *css_rgb << std::dec << "\n";
  }
  if (css_hsl) {
    std::cout << "  ";
    print_swatch(std::cout, *css_hsl, 4);
    std::cout << " hsl(red)    -> " << std::hex << *css_hsl << std::dec << "\n";
  }

  // 5. Round-trip: output -> re-parse -> compare
  section("Round-trip: output then re-parse");

  auto original = constants::steelblue;

  std::ostringstream oss_text, oss_hex;
  oss_text << original;
  oss_hex << std::hex << original;

  auto recovered_text = parse<rgba8_t>(oss_text.str());
  auto recovered_hex = parse<rgba8_t>(oss_hex.str());

  std::cout << "original:       ";
  print_swatch(std::cout, original, 4);
  std::cout << " " << std::hex << original << "\n";
  std::cout << "text form:      " << oss_text.str() << "\n";
  std::cout << "re-parsed:      ";
  print_swatch(std::cout, recovered_text, 4);
  std::cout << " " << recovered_text << "\n";
  std::cout << "hex form:       " << oss_hex.str() << "\n";
  std::cout << "re-parsed:      ";
  print_swatch(std::cout, recovered_hex, 4);
  std::cout << " " << recovered_hex << "\n" << std::dec;

  // HSL round-trip
  hsla_float_t hsl_orig = color_cast<hsla_float_t>(original);
  std::ostringstream oss_hsl;
  oss_hsl << hsl_orig;
  auto hsl_back = parse<hsla_float_t>(oss_hsl.str());
  std::cout << "hsl form:       " << oss_hsl.str() << "\n";
  std::cout << "hsl re-parsed:  ";
  print_swatch(std::cout, hsl_back, 4);
  std::cout << " " << hsl_back << "\n";

  return 0;
}
