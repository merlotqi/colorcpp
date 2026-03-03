#include <gtest/gtest.h>

#include <color/color.hpp>

TEST(literals, rgb) {
  using namespace color::literals;

  // Test template literals
  constexpr auto red_template = 0xFF0000_rgb;
  constexpr auto blue_template = 0x0000FF_rgb;
  constexpr auto green_template = 0x00FF00_rgb;

  // Verify RGB values
  EXPECT_EQ(red_template.r, 255);
  EXPECT_EQ(red_template.g, 0);
  EXPECT_EQ(red_template.b, 0);
  EXPECT_EQ(red_template.a, 255);

  EXPECT_EQ(blue_template.r, 0);
  EXPECT_EQ(blue_template.g, 0);
  EXPECT_EQ(blue_template.b, 255);
  EXPECT_EQ(blue_template.a, 255);

  EXPECT_EQ(green_template.r, 0);
  EXPECT_EQ(green_template.g, 255);
  EXPECT_EQ(green_template.b, 0);
  EXPECT_EQ(green_template.a, 255);
}

TEST(literals, rgba) {
  using namespace color::literals;

  // Test template literals with alpha
  constexpr auto transparent_red_template = 0x80FF0000_rgba;
  constexpr auto semi_blue_template = 0x400000FF_rgba;

  // Verify RGBA values
  EXPECT_EQ(transparent_red_template.r, 128);
  EXPECT_EQ(transparent_red_template.g, 255);
  EXPECT_EQ(transparent_red_template.b, 0);
  EXPECT_EQ(transparent_red_template.a, 0);

  EXPECT_EQ(semi_blue_template.r, 64);
  EXPECT_EQ(semi_blue_template.g, 0);
  EXPECT_EQ(semi_blue_template.b, 0);
  EXPECT_EQ(semi_blue_template.a, 255);
}

TEST(literals, hsv) {
  using namespace color::literals;

  // Test numeric literals
  auto hsv_color = 120500750_hsv;   // Hue=120, Sat=500, Val=750
  auto hsv_color2 = 240750500_hsv;  // Hue=240, Sat=750, Val=500

  // Verify HSV values
  EXPECT_EQ(hsv_color.h, 120);
  EXPECT_EQ(hsv_color.s, 500);
  EXPECT_EQ(hsv_color.v, 750);
  EXPECT_EQ(hsv_color.a, 1000);

  EXPECT_EQ(hsv_color2.h, 240);
  EXPECT_EQ(hsv_color2.s, 750);
  EXPECT_EQ(hsv_color2.v, 500);
  EXPECT_EQ(hsv_color2.a, 1000);
}

TEST(literals, hsva) {
  using namespace color::literals;

  // Test numeric literals with alpha
  auto hsva_color = 120500750800_hsva;   // Hue=120, Sat=500, Val=750, Alpha=800
  auto hsva_color2 = 300600400900_hsva;  // Hue=300, Sat=600, Val=400, Alpha=900

  // Verify HSVA values
  EXPECT_EQ(hsva_color.h, 120);
  EXPECT_EQ(hsva_color.s, 500);
  EXPECT_EQ(hsva_color.v, 750);
  EXPECT_EQ(hsva_color.a, 800);

  EXPECT_EQ(hsva_color2.h, 300);
  EXPECT_EQ(hsva_color2.s, 600);
  EXPECT_EQ(hsva_color2.v, 400);
  EXPECT_EQ(hsva_color2.a, 900);
}

TEST(literals, hsl) {
  using namespace color::literals;

  // Test numeric literals
  auto hsl_color = 240750500_hsl;  // Hue=240, Sat=750, Light=500
  auto hsl_color2 = 60500750_hsl;  // Hue=60, Sat=500, Light=750

  // Verify HSL values
  EXPECT_EQ(hsl_color.h, 240);
  EXPECT_EQ(hsl_color.s, 750);
  EXPECT_EQ(hsl_color.l, 500);
  EXPECT_EQ(hsl_color.a, 1000);

  EXPECT_EQ(hsl_color2.h, 60);
  EXPECT_EQ(hsl_color2.s, 500);
  EXPECT_EQ(hsl_color2.l, 750);
  EXPECT_EQ(hsl_color2.a, 1000);
}

TEST(literals, hsla) {
  using namespace color::literals;

  // Test numeric literals with alpha
  auto hsla_color = 240750500900_hsla;   // Hue=240, Sat=750, Light=500, Alpha=900
  auto hsla_color2 = 180600400700_hsla;  // Hue=180, Sat=600, Light=400, Alpha=700

  // Verify HSLA values
  EXPECT_EQ(hsla_color.h, 240);
  EXPECT_EQ(hsla_color.s, 750);
  EXPECT_EQ(hsla_color.l, 500);
  EXPECT_EQ(hsla_color.a, 900);

  EXPECT_EQ(hsla_color2.h, 180);
  EXPECT_EQ(hsla_color2.s, 600);
  EXPECT_EQ(hsla_color2.l, 400);
  EXPECT_EQ(hsla_color2.a, 700);
}

TEST(literals, hsv_hue) {
  using namespace color::literals;

  // Test numeric literals
  auto pure_red = 0_hsv_hue;      // Hue=0, Sat=1000, Val=1000, Alpha=1000
  auto pure_green = 120_hsv_hue;  // Hue=120, Sat=1000, Val=1000, Alpha=1000
  auto pure_blue = 240_hsv_hue;   // Hue=240, Sat=1000, Val=1000, Alpha=1000

  // Verify HSV hue shortcut values
  EXPECT_EQ(pure_red.h, 0);
  EXPECT_EQ(pure_red.s, 1000);
  EXPECT_EQ(pure_red.v, 1000);
  EXPECT_EQ(pure_red.a, 1000);

  EXPECT_EQ(pure_green.h, 120);
  EXPECT_EQ(pure_green.s, 1000);
  EXPECT_EQ(pure_green.v, 1000);
  EXPECT_EQ(pure_green.a, 1000);

  EXPECT_EQ(pure_blue.h, 240);
  EXPECT_EQ(pure_blue.s, 1000);
  EXPECT_EQ(pure_blue.v, 1000);
  EXPECT_EQ(pure_blue.a, 1000);
}

TEST(literals, hsl_hue) {
  using namespace color::literals;

  // Test numeric literals
  auto pure_red = 0_hsl_hue;      // Hue=0, Sat=1000, Light=500, Alpha=1000
  auto pure_green = 120_hsl_hue;  // Hue=120, Sat=1000, Light=500, Alpha=1000
  auto pure_blue = 240_hsl_hue;   // Hue=240, Sat=1000, Light=500, Alpha=1000

  // Verify HSL hue shortcut values
  EXPECT_EQ(pure_red.h, 0);
  EXPECT_EQ(pure_red.s, 1000);
  EXPECT_EQ(pure_red.l, 500);
  EXPECT_EQ(pure_red.a, 1000);

  EXPECT_EQ(pure_green.h, 120);
  EXPECT_EQ(pure_green.s, 1000);
  EXPECT_EQ(pure_green.l, 500);
  EXPECT_EQ(pure_green.a, 1000);

  EXPECT_EQ(pure_blue.h, 240);
  EXPECT_EQ(pure_blue.s, 1000);
  EXPECT_EQ(pure_blue.l, 500);
  EXPECT_EQ(pure_blue.a, 1000);
}

TEST(literals, template_literals) {
  using namespace color::literals;

  // Test template literals for all color spaces
  constexpr auto rgb_template = 0xFF0000_rgb;
  constexpr auto rgba_template = 0x80FF0000_argb;
  constexpr auto hsv_template = 120500750_hsv;
  constexpr auto hsva_template = 120500750800_hsva;
  constexpr auto hsl_template = 240750500_hsl;
  constexpr auto hsla_template = 240750500900_hsla;
  constexpr auto hsv_hue_template = 120_hsv_hue;
  constexpr auto hsl_hue_template = 240_hsl_hue;

  // Verify template literal values
  EXPECT_EQ(rgb_template.r, 255);
  EXPECT_EQ(rgb_template.g, 0);
  EXPECT_EQ(rgb_template.b, 0);
  EXPECT_EQ(rgb_template.a, 255);

  EXPECT_EQ(rgba_template.r, 255);
  EXPECT_EQ(rgba_template.g, 0);
  EXPECT_EQ(rgba_template.b, 0);
  EXPECT_EQ(rgba_template.a, 128);

  EXPECT_EQ(hsv_template.h, 120);
  EXPECT_EQ(hsv_template.s, 500);
  EXPECT_EQ(hsv_template.v, 750);
  EXPECT_EQ(hsv_template.a, 1000);

  EXPECT_EQ(hsva_template.h, 120);
  EXPECT_EQ(hsva_template.s, 500);
  EXPECT_EQ(hsva_template.v, 750);
  EXPECT_EQ(hsva_template.a, 800);

  EXPECT_EQ(hsl_template.h, 240);
  EXPECT_EQ(hsl_template.s, 750);
  EXPECT_EQ(hsl_template.l, 500);
  EXPECT_EQ(hsl_template.a, 1000);

  EXPECT_EQ(hsla_template.h, 240);
  EXPECT_EQ(hsla_template.s, 750);
  EXPECT_EQ(hsla_template.l, 500);
  EXPECT_EQ(hsla_template.a, 900);

  EXPECT_EQ(hsv_hue_template.h, 120);
  EXPECT_EQ(hsv_hue_template.s, 1000);
  EXPECT_EQ(hsv_hue_template.v, 1000);
  EXPECT_EQ(hsv_hue_template.a, 1000);

  EXPECT_EQ(hsl_hue_template.h, 240);
  EXPECT_EQ(hsl_hue_template.s, 1000);
  EXPECT_EQ(hsl_hue_template.l, 500);
  EXPECT_EQ(hsl_hue_template.a, 1000);
}

TEST(literals, css_hex) {
  using namespace color::literals;

  constexpr auto red_short = "#f00"_hex;
  static_assert(red_short.r == 255 && red_short.g == 0 && red_short.b == 0 && red_short.a == 255);
  EXPECT_EQ(red_short.r, 255);
  EXPECT_EQ(red_short.g, 0);
  EXPECT_EQ(red_short.b, 0);
  EXPECT_EQ(red_short.a, 255);

  constexpr auto green_short = "#0f0"_hex;
  EXPECT_EQ(green_short.r, 0);
  EXPECT_EQ(green_short.g, 255);
  EXPECT_EQ(green_short.b, 0);
  EXPECT_EQ(green_short.a, 255);

  constexpr auto blue_short = "#00f"_hex;
  EXPECT_EQ(blue_short.r, 0);
  EXPECT_EQ(blue_short.g, 0);
  EXPECT_EQ(blue_short.b, 255);
  EXPECT_EQ(blue_short.a, 255);

  constexpr auto gray_short = "#888"_hex;
  EXPECT_EQ(gray_short.r, 136);  // 0x88
  EXPECT_EQ(gray_short.g, 136);
  EXPECT_EQ(gray_short.b, 136);
  EXPECT_EQ(gray_short.a, 255);

  constexpr auto red_alpha_short = "#f008"_hex;
  EXPECT_EQ(red_alpha_short.r, 255);
  EXPECT_EQ(red_alpha_short.g, 0);
  EXPECT_EQ(red_alpha_short.b, 0);
  EXPECT_EQ(red_alpha_short.a, 136);  // 0x88

  constexpr auto green_alpha_short = "#0f08"_hex;
  EXPECT_EQ(green_alpha_short.r, 0);
  EXPECT_EQ(green_alpha_short.g, 255);
  EXPECT_EQ(green_alpha_short.b, 0);
  EXPECT_EQ(green_alpha_short.a, 136);

  constexpr auto blue_alpha_short = "#00f8"_hex;
  EXPECT_EQ(blue_alpha_short.r, 0);
  EXPECT_EQ(blue_alpha_short.g, 0);
  EXPECT_EQ(blue_alpha_short.b, 255);
  EXPECT_EQ(blue_alpha_short.a, 136);

  constexpr auto transparent = "#0000"_hex;
  EXPECT_EQ(transparent.r, 0);
  EXPECT_EQ(transparent.g, 0);
  EXPECT_EQ(transparent.b, 0);
  EXPECT_EQ(transparent.a, 0);

  constexpr auto blue = "0000ff"_hex;
  EXPECT_EQ(blue.r, 0);
  EXPECT_EQ(blue.g, 0);
  EXPECT_EQ(blue.b, 255);
  EXPECT_EQ(blue.a, 255);

  constexpr auto cyan = "00ffff"_hex;
  EXPECT_EQ(cyan.r, 0);
  EXPECT_EQ(cyan.g, 255);
  EXPECT_EQ(cyan.b, 255);
  EXPECT_EQ(cyan.a, 255);

  constexpr auto magenta = "ff00ff"_hex;
  EXPECT_EQ(magenta.r, 255);
  EXPECT_EQ(magenta.g, 0);
  EXPECT_EQ(magenta.b, 255);
  EXPECT_EQ(magenta.a, 255);

  constexpr auto red = "#ff0000"_hex;
  EXPECT_EQ(red.r, 255);
  EXPECT_EQ(red.g, 0);
  EXPECT_EQ(red.b, 0);
  EXPECT_EQ(red.a, 255);

  constexpr auto orange = "#ffa500"_hex;
  EXPECT_EQ(orange.r, 255);
  EXPECT_EQ(orange.g, 165);
  EXPECT_EQ(orange.b, 0);
  EXPECT_EQ(orange.a, 255);

  constexpr auto purple = "#800080"_hex;
  EXPECT_EQ(purple.r, 128);
  EXPECT_EQ(purple.g, 0);
  EXPECT_EQ(purple.b, 128);
  EXPECT_EQ(purple.a, 255);

  constexpr auto green_semi = "#00ff0080"_hex;
  EXPECT_EQ(green_semi.r, 0);
  EXPECT_EQ(green_semi.g, 255);
  EXPECT_EQ(green_semi.b, 0);
  EXPECT_EQ(green_semi.a, 128);  // 0x80

  constexpr auto red_semi = "#ff000040"_hex;
  EXPECT_EQ(red_semi.r, 255);
  EXPECT_EQ(red_semi.g, 0);
  EXPECT_EQ(red_semi.b, 0);
  EXPECT_EQ(red_semi.a, 64);  // 0x40

  constexpr auto blue_semi = "0000ffcc"_hex;
  EXPECT_EQ(blue_semi.r, 0);
  EXPECT_EQ(blue_semi.g, 0);
  EXPECT_EQ(blue_semi.b, 255);
  EXPECT_EQ(blue_semi.a, 204);  // 0xCC

  constexpr auto black_25 = "#00000040"_hex;
  EXPECT_EQ(black_25.r, 0);
  EXPECT_EQ(black_25.g, 0);
  EXPECT_EQ(black_25.b, 0);
  EXPECT_EQ(black_25.a, 64);

  constexpr auto white_50 = "#ffffff80"_hex;
  EXPECT_EQ(white_50.r, 255);
  EXPECT_EQ(white_50.g, 255);
  EXPECT_EQ(white_50.b, 255);
  EXPECT_EQ(white_50.a, 128);

  constexpr auto upper_case = "#FFA500"_hex;
  constexpr auto lower_case = "#ffa500"_hex;
  constexpr auto mixed_case = "#FfA500"_hex;

  EXPECT_EQ(upper_case.r, 255);
  EXPECT_EQ(upper_case.g, 165);
  EXPECT_EQ(upper_case.b, 0);

  EXPECT_EQ(upper_case.r, lower_case.r);
  EXPECT_EQ(upper_case.g, lower_case.g);
  EXPECT_EQ(upper_case.b, lower_case.b);

  EXPECT_EQ(mixed_case.r, 255);
  EXPECT_EQ(mixed_case.g, 165);
  EXPECT_EQ(mixed_case.b, 0);
}
