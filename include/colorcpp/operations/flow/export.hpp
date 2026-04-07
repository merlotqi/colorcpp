/**
 * @file export.hpp
 * @brief Multi-format export utilities for themes and palettes.
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/flow/pipeline.hpp>
#include <colorcpp/operations/flow/theme.hpp>
#include <sstream>
#include <string>

namespace colorcpp::operations::flow::export_ {

inline std::string to_hex(const core::rgba8_t& color) { return pipeline(color).to_hex(); }

inline std::string to_css_variables(const semantic_palette& palette) {
  std::ostringstream ss;
  ss << ":root {\n";
  ss << "  --primary: " << to_hex(palette.primary) << ";\n";
  ss << "  --secondary: " << to_hex(palette.secondary) << ";\n";
  ss << "  --tertiary: " << to_hex(palette.tertiary) << ";\n";
  ss << "  --error: " << to_hex(palette.error) << ";\n";
  ss << "  --surface: " << to_hex(palette.surface) << ";\n";
  ss << "  --surface-variant: " << to_hex(palette.surface_variant) << ";\n";
  ss << "  --on-primary: " << to_hex(palette.on_primary) << ";\n";
  ss << "  --on-secondary: " << to_hex(palette.on_secondary) << ";\n";
  ss << "  --on-surface: " << to_hex(palette.on_surface) << ";\n";
  ss << "  --outline: " << to_hex(palette.outline) << ";\n";
  ss << "}\n";
  return ss.str();
}

inline std::string to_scss_variables(const semantic_palette& palette) {
  std::ostringstream ss;
  ss << "// Theme colors\n";
  ss << "$primary: " << to_hex(palette.primary) << ";\n";
  ss << "$secondary: " << to_hex(palette.secondary) << ";\n";
  ss << "$tertiary: " << to_hex(palette.tertiary) << ";\n";
  ss << "$error: " << to_hex(palette.error) << ";\n";
  ss << "$surface: " << to_hex(palette.surface) << ";\n";
  ss << "$surface-variant: " << to_hex(palette.surface_variant) << ";\n";
  ss << "$on-primary: " << to_hex(palette.on_primary) << ";\n";
  ss << "$on-secondary: " << to_hex(palette.on_secondary) << ";\n";
  ss << "$on-surface: " << to_hex(palette.on_surface) << ";\n";
  ss << "$outline: " << to_hex(palette.outline) << ";\n";
  return ss.str();
}

inline std::string to_flutter_code(const semantic_palette& palette) {
  std::ostringstream ss;
  ss << "// Flutter Color constants\n";
  ss << "class AppColors {\n";
  ss << "  static const Color primary = Color(0xFF" << pipeline(palette.primary).to_hex().substr(1) << ");\n";
  ss << "  static const Color secondary = Color(0xFF" << pipeline(palette.secondary).to_hex().substr(1) << ");\n";
  ss << "  static const Color tertiary = Color(0xFF" << pipeline(palette.tertiary).to_hex().substr(1) << ");\n";
  ss << "  static const Color error = Color(0xFF" << pipeline(palette.error).to_hex().substr(1) << ");\n";
  ss << "  static const Color surface = Color(0xFF" << pipeline(palette.surface).to_hex().substr(1) << ");\n";
  ss << "  static const Color surfaceVariant = Color(0xFF" << pipeline(palette.surface_variant).to_hex().substr(1)
     << ");\n";
  ss << "  static const Color onPrimary = Color(0xFF" << pipeline(palette.on_primary).to_hex().substr(1) << ");\n";
  ss << "  static const Color onSecondary = Color(0xFF" << pipeline(palette.on_secondary).to_hex().substr(1) << ");\n";
  ss << "  static const Color onSurface = Color(0xFF" << pipeline(palette.on_surface).to_hex().substr(1) << ");\n";
  ss << "  static const Color outline = Color(0xFF" << pipeline(palette.outline).to_hex().substr(1) << ");\n";
  ss << "}\n";
  return ss.str();
}

inline std::string to_swiftui_code(const semantic_palette& palette) {
  std::ostringstream ss;
  ss << "// SwiftUI Color extensions\n";
  ss << "extension Color {\n";
  ss << "  static let primary = Color(\"primary\")\n";
  ss << "  static let secondary = Color(\"secondary\")\n";
  ss << "  static let tertiary = Color(\"tertiary\")\n";
  ss << "  static let error = Color(\"error\")\n";
  ss << "  static let surface = Color(\"surface\")\n";
  ss << "  static let surfaceVariant = Color(\"surfaceVariant\")\n";
  ss << "  static let onPrimary = Color(\"onPrimary\")\n";
  ss << "  static let onSecondary = Color(\"onSecondary\")\n";
  ss << "  static let onSurface = Color(\"onSurface\")\n";
  ss << "  static let outline = Color(\"outline\")\n";
  ss << "}\n";
  ss << "\n// Asset catalog colors:\n/*\n";
  ss << "primary: " << pipeline(palette.primary).to_hex() << "\n";
  ss << "secondary: " << pipeline(palette.secondary).to_hex() << "\n";
  ss << "tertiary: " << pipeline(palette.tertiary).to_hex() << "\n";
  ss << "error: " << pipeline(palette.error).to_hex() << "\n";
  ss << "surface: " << pipeline(palette.surface).to_hex() << "\n";
  ss << "surfaceVariant: " << pipeline(palette.surface_variant).to_hex() << "\n";
  ss << "onPrimary: " << pipeline(palette.on_primary).to_hex() << "\n";
  ss << "onSecondary: " << pipeline(palette.on_secondary).to_hex() << "\n";
  ss << "onSurface: " << pipeline(palette.on_surface).to_hex() << "\n";
  ss << "outline: " << pipeline(palette.outline).to_hex() << "\n*/\n";
  return ss.str();
}

}  // namespace colorcpp::operations::flow::export_
