/**
 * @file blend_example.cpp
 * @brief Example: All blend modes, separable vs non-separable, and SIMD acceleration.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

static void demo_blend(const char* name, operations::blend::blend_mode mode, rgba8_t bg, rgba8_t fg) {
  auto result = operations::blend::blend(bg, fg, mode);
  std::cout << "  " << std::left << std::setw(20) << name << "  ";
  print_swatch(std::cout, bg, 3);
  std::cout << " + ";
  print_swatch(std::cout, fg, 3);
  std::cout << " = ";
  print_swatch(std::cout, result, 4);
  std::cout << "  " << std::hex << result << std::dec << '\n';
}

int main() {
  std::cout << std::fixed << std::setprecision(2);

  auto white = 0xFFFFFF_rgb;
  auto black = 0x000000_rgb;
  auto coral = 0xFF6347_rgb;
  auto steelblue = 0x4682B4_rgb;

  // ========================================================================
  section("1. Basic Porter-Duff Blend Modes");

  using operations::blend::blend_mode;

  demo_blend("normal", blend_mode::normal, steelblue, coral);
  demo_blend("multiply", blend_mode::multiply, steelblue, coral);
  demo_blend("screen", blend_mode::screen, steelblue, coral);
  demo_blend("overlay", blend_mode::overlay, steelblue, coral);
  demo_blend("darken", blend_mode::darken, steelblue, coral);
  demo_blend("lighten", blend_mode::lighten, steelblue, coral);
  demo_blend("color_dodge", blend_mode::color_dodge, steelblue, coral);
  demo_blend("color_burn", blend_mode::color_burn, steelblue, coral);
  demo_blend("hard_light", blend_mode::hard_light, steelblue, coral);
  demo_blend("soft_light", blend_mode::soft_light, steelblue, coral);
  demo_blend("difference", blend_mode::difference, steelblue, coral);
  demo_blend("exclusion", blend_mode::exclusion, steelblue, coral);

  // ========================================================================
  section("2. Non-Separable Blend Modes");

  demo_blend("hue", blend_mode::hue, steelblue, coral);
  demo_blend("saturation", blend_mode::saturation, steelblue, coral);
  demo_blend("color", blend_mode::color_blend, steelblue, coral);
  demo_blend("luminosity", blend_mode::luminosity, steelblue, coral);

  // ========================================================================
  section("3. Additional Blend Modes");

  demo_blend("addition", blend_mode::addition, steelblue, coral);
  demo_blend("subtraction", blend_mode::subtraction, steelblue, coral);
  demo_blend("divide", blend_mode::divide, steelblue, coral);

  // ========================================================================
  section("4. Alpha Blending Behavior");

  auto coral_transparent = rgba8_t{0xFF, 0x63, 0x47, 0x80};  // 50% alpha
  std::cout << "  Background:        ";
  print_swatch(std::cout, steelblue, 4);
  std::cout << "  steelblue\n";
  std::cout << "  Foreground (50%):  ";
  print_swatch(std::cout, coral_transparent, 4);
  std::cout << "  coral @ 50%\n";

  auto blended = operations::blend::blend(steelblue, coral_transparent);
  std::cout << "  Normal blend:      ";
  print_swatch(std::cout, blended, 4);
  std::cout << "  result\n";

  // ========================================================================
  section("5. Blend Mode Matrix");

  std::cout << "\n  Blend table (bg: steelblue | fg: coral at different opacity):\n\n";
  std::cout << "  Opacity:   ";
  for (int a = 0; a <= 100; a += 20) {
    std::cout << std::setw(4) << a << "% ";
  }
  std::cout << "\n";

  for (int a = 0; a <= 100; a += 20) {
    auto fg = rgba8_t{coral.r(), coral.g(), coral.b(), static_cast<uint8_t>(a * 255 / 100)};
    auto result = operations::blend::blend(steelblue, fg);
    std::cout << "             ";
    print_swatch(std::cout, result, 2);
    std::cout << " ";
  }
  std::cout << "\n\n";

  // ========================================================================
  section("6. Common Use Cases");

  std::cout << "  Tint white with color:      ";
  print_swatch(std::cout, operations::blend::blend(white, coral, blend_mode::multiply), 4);
  std::cout << "  multiply(white, color)\n";

  std::cout << "  Shade black with color:     ";
  print_swatch(std::cout, operations::blend::blend(black, coral, blend_mode::screen), 4);
  std::cout << "  screen(black, color)\n";

  std::cout << "  Glow effect:                ";
  print_swatch(std::cout, operations::blend::blend(steelblue, coral, blend_mode::addition), 4);
  std::cout << "  addition / linear dodge\n";

  std::cout << "  Burn effect:                ";
  print_swatch(std::cout, operations::blend::blend(steelblue, coral, blend_mode::color_burn), 4);
  std::cout << "  color_burn\n";

  std::cout << "\n";
  return 0;
}
