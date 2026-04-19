/**
 * @file css_example.cpp
 * @brief Example: CSS color parsing across absolute colors, typed targets, and context-aware features.
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

#include "colorcpp/core/rgb.hpp"
#include "colorcpp/io/css/relative_color.hpp"

using namespace colorcpp;
using namespace colorcpp::core;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::css;
using namespace colorcpp::operations::conversion;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

static void print_parsed_rgba8(std::string_view input, const std::optional<rgba8_t>& parsed) {
  std::cout << "  " << std::left << std::setw(44) << std::string(input);
  if (!parsed) {
    std::cout << " -> rejected\n";
    return;
  }

  print_swatch(std::cout, *parsed, 6);
  std::cout << "  " << to_css_color_string(*parsed) << "  rgba(" << static_cast<int>(parsed->r()) << ", "
            << static_cast<int>(parsed->g()) << ", " << static_cast<int>(parsed->b()) << ", "
            << static_cast<int>(parsed->a()) << ")\n";
}

template <typename Color>
static void print_typed_parse(std::string_view label, std::string_view input) {
  auto parsed = parse_css_color<Color>(input);
  std::cout << "  " << bold() << label << reset() << '\n';
  std::cout << "    input:   " << input << '\n';
  if (!parsed) {
    std::cout << "    result:  rejected\n";
    return;
  }

  auto preview = color_cast<rgba8_t>(*parsed);
  std::cout << "    value:   " << *parsed << '\n';
  std::cout << "    preview: ";
  print_swatch(std::cout, preview, 6);
  std::cout << "  " << to_css_color_string(preview) << '\n';
}

int main() {
  std::cout << std::fixed << std::setprecision(4);

  section("Absolute CSS Colors");
  const std::vector<std::string_view> absolute_inputs = {
      "#ff634780",
      "transparent",
      "rgb(255 99 71 / 60%)",
      "oklch(62% 0.24 30)",
      "color(display-p3 1 0.45 0.10 / 80%)",
      "color(rec2020 0.8 0.1 0.05)",
      "device-cmyk(0% 61% 72% 0% / 75%)",
      "color-mix(in oklab, rebeccapurple 65%, gold)",
  };
  for (auto input : absolute_inputs) {
    print_parsed_rgba8(input, parse_css_color_rgba8(input));
  }

  section("Typed Targets Preserve More Information");
  const std::string_view wide_input = "color(display-p3 1 0.45 0.10 / 80%)";
  print_typed_parse<display_p3af_t>("Display P3A target", wide_input);
  print_typed_parse<xyz_t>("XYZ target", "color(rec2020 1 0 0)");
  print_typed_parse<oklab_t>("OkLab target", "color-mix(in oklab, rebeccapurple 65%, gold)");

  section("Context-Aware CSS Colors");
  parse_css_color_context context;
  context.dark_theme = false;
  context.current_color = rgbaf_t{0.12f, 0.42f, 0.92f, 0.60f};
  context.canvas = rgbaf_t{0.98f, 0.98f, 1.00f, 1.00f};
  context.canvas_text = rgbaf_t{0.10f, 0.12f, 0.16f, 1.00f};
  context.link_text = rgbaf_t{0.00f, 0.20f, 1.00f, 1.00f};

  const std::vector<std::string_view> contextual_inputs = {
      "currentColor",
      "CanvasText",
      "light-dark(currentColor, CanvasText)",
      "color-mix(in lab, currentColor 30%, Canvas)",
      "color-mix(in srgb, LinkText, transparent 25%)",
  };
  for (auto input : contextual_inputs) {
    print_parsed_rgba8(input, parse_css_color_rgba8(input, context));
  }

  section("Relative Colors & AST Parsing");
  std::cout << "  CSS Color Level 4 relative color syntax with channel expressions\n\n";

  const std::vector<std::string_view> relative_inputs = {
      "rgb(from #336699 r g calc(b * 1.2) / 0.8)",
      "rgb(from rebeccapurple calc(r * 0.8) calc(g + 20) b)",
      "color(from oklch(60% 0.18 240) in srgb r g b / 70%)",
      "color(from gold in display-p3 r calc(g * 0.95) b)",
  };

  for (auto input : relative_inputs) {
    print_parsed_rgba8(input, parse_css_color_rgba8(input, context));
  }

  section("Nested var() Support");
  std::cout << "  var() variables can appear anywhere inside expressions\n\n";

  parse_css_color_context var_context;
  var_context.variable_resolver = [](std::string_view name) -> std::optional<rgbaf_t> {
    if (name == "--primary") return rgbaf_t{0.2f, 0.6f, 0.9f, 1.0f};
    return std::nullopt;
  };
  var_context.numeric_variable_resolver = [](std::string_view name) -> std::optional<float> {
    if (name == "--opacity") return 0.6f;
    if (name == "--brightness") return 1.3f;
    return std::nullopt;
  };

  const std::vector<std::string_view> nested_var_inputs = {
      "rgb(from var(--primary) r g b / var(--opacity))",
      "rgb(from var(--primary) calc(r * var(--brightness)) g b)",
      "rgb(from var(--primary) r g b / calc(var(--opacity) * 0.75))",
      "color-mix(in oklab, var(--primary) var(--opacity)%, white)",
  };

  for (auto input : nested_var_inputs) {
    print_parsed_rgba8(input, parse_css_color_rgba8(input, var_context));
  }

  std::cout << "\n  AST mode: parse once, evaluate with different contexts\n";
  const std::string_view ast_input = "rgb(from var(--theme-color) r g b / calc(var(--opacity) * 0.75))";
  auto ast = parse_css_color_ast(ast_input);
  if (ast) {
    std::cout << "    input:        " << ast_input << '\n';
    std::cout << "    node type:    relative color with 4 expression trees\n";
    std::cout << "    nodes:        literal, variable_reference, mul\n\n";

    // Evaluate with different variable bindings
    {
      auto result = evaluate<rgba8_t>(
          *ast,
          [](std::string_view name) -> std::optional<rgbaf_t> {
            if (name == "--theme-color") return rgbaf_t{0.0f, 0.5f, 1.0f, 1.0f};
            return std::nullopt;
          },
          [](std::string_view name) -> std::optional<float> {
            if (name == "--opacity") return 0.8f;
            return std::nullopt;
          });
      std::cout << "    context 1: blue theme / 80% opacity →  ";
      print_swatch(std::cout, *result, 6);
      std::cout << "  " << to_css_color_string(*result) << '\n';
    }

    {
      auto result = evaluate<rgba8_t>(
          *ast,
          [](std::string_view name) -> std::optional<rgbaf_t> {
            if (name == "--theme-color") return rgbaf_t{1.0f, 0.2f, 0.0f, 1.0f};
            return std::nullopt;
          },
          [](std::string_view name) -> std::optional<float> {
            if (name == "--opacity") return 0.5f;
            return std::nullopt;
          });
      std::cout << "    context 2: orange theme / 50% opacity →  ";
      print_swatch(std::cout, *result, 6);
      std::cout << "  " << to_css_color_string(*result) << '\n';
    }
  }

  section("Rejections");
  const std::vector<std::string_view> invalid_inputs = {
      "rgb(255 99)",
      "color(unknown-space 1 0 0)",
      "color-mix(in srgb red, blue)",
      "light-dark(currentColor)",
      "device-cmyk(0, 1, 1, 0)",
  };
  for (auto input : invalid_inputs) {
    print_parsed_rgba8(input, parse_css_color_rgba8(input, context));
  }

  return 0;
}
