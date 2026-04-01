/**
 * @file named_colors.hpp
 * @brief CSS named colors parsing (140+ predefined color names).
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace colorcpp::io::css {

/**
 * @brief Get the RGBA value for a CSS named color.
 * @param name The color name (case-insensitive)
 * @return The RGBA value if found, std::nullopt otherwise
 */
inline std::optional<core::rgba8_t> get_named_color(std::string_view name) {
  // CSS Color Level 4 named colors (140+ colors)
  static const std::unordered_map<std::string_view, core::rgba8_t> named_colors = {
      // Pink colors
      {"pink", core::rgba8_t{255, 192, 203, 255}},
      {"lightpink", core::rgba8_t{255, 182, 193, 255}},
      {"hotpink", core::rgba8_t{255, 105, 180, 255}},
      {"deeppink", core::rgba8_t{255, 20, 147, 255}},
      {"mediumvioletred", core::rgba8_t{199, 21, 133, 255}},
      {"palevioletred", core::rgba8_t{219, 112, 147, 255}},

      // Red colors
      {"lightsalmon", core::rgba8_t{255, 160, 122, 255}},
      {"salmon", core::rgba8_t{250, 128, 114, 255}},
      {"darksalmon", core::rgba8_t{233, 150, 122, 255}},
      {"lightcoral", core::rgba8_t{240, 128, 128, 255}},
      {"indianred", core::rgba8_t{205, 92, 92, 255}},
      {"crimson", core::rgba8_t{220, 20, 60, 255}},
      {"firebrick", core::rgba8_t{178, 34, 34, 255}},
      {"red", core::rgba8_t{255, 0, 0, 255}},
      {"darkred", core::rgba8_t{139, 0, 0, 255}},

      // Orange colors
      {"coral", core::rgba8_t{255, 127, 80, 255}},
      {"tomato", core::rgba8_t{255, 99, 71, 255}},
      {"orangered", core::rgba8_t{255, 69, 0, 255}},
      {"gold", core::rgba8_t{255, 215, 0, 255}},
      {"orange", core::rgba8_t{255, 165, 0, 255}},
      {"darkorange", core::rgba8_t{255, 140, 0, 255}},

      // Yellow colors
      {"lightyellow", core::rgba8_t{255, 255, 224, 255}},
      {"lemonchiffon", core::rgba8_t{255, 250, 205, 255}},
      {"lightgoldenrodyellow", core::rgba8_t{250, 250, 210, 255}},
      {"papayawhip", core::rgba8_t{255, 239, 213, 255}},
      {"moccasin", core::rgba8_t{255, 228, 181, 255}},
      {"peachpuff", core::rgba8_t{255, 218, 185, 255}},
      {"palegoldenrod", core::rgba8_t{238, 232, 170, 255}},
      {"khaki", core::rgba8_t{240, 230, 140, 255}},
      {"darkkhaki", core::rgba8_t{189, 183, 107, 255}},
      {"yellow", core::rgba8_t{255, 255, 0, 255}},

      // Purple colors
      {"lavender", core::rgba8_t{230, 230, 250, 255}},
      {"thistle", core::rgba8_t{216, 191, 216, 255}},
      {"plum", core::rgba8_t{221, 160, 221, 255}},
      {"violet", core::rgba8_t{238, 130, 238, 255}},
      {"orchid", core::rgba8_t{218, 112, 214, 255}},
      {"fuchsia", core::rgba8_t{255, 0, 255, 255}},
      {"magenta", core::rgba8_t{255, 0, 255, 255}},
      {"mediumorchid", core::rgba8_t{186, 85, 211, 255}},
      {"mediumpurple", core::rgba8_t{147, 112, 219, 255}},
      {"blueviolet", core::rgba8_t{138, 43, 226, 255}},
      {"darkviolet", core::rgba8_t{148, 0, 211, 255}},
      {"darkorchid", core::rgba8_t{153, 50, 204, 255}},
      {"darkmagenta", core::rgba8_t{139, 0, 139, 255}},
      {"purple", core::rgba8_t{128, 0, 128, 255}},
      {"indigo", core::rgba8_t{75, 0, 130, 255}},
      {"darkslateblue", core::rgba8_t{72, 61, 139, 255}},
      {"slateblue", core::rgba8_t{106, 90, 205, 255}},
      {"mediumslateblue", core::rgba8_t{123, 104, 238, 255}},

      // Green colors
      {"greenyellow", core::rgba8_t{173, 255, 47, 255}},
      {"chartreuse", core::rgba8_t{127, 255, 0, 255}},
      {"lawngreen", core::rgba8_t{124, 252, 0, 255}},
      {"lime", core::rgba8_t{0, 255, 0, 255}},
      {"limegreen", core::rgba8_t{50, 205, 50, 255}},
      {"palegreen", core::rgba8_t{152, 251, 152, 255}},
      {"lightgreen", core::rgba8_t{144, 238, 144, 255}},
      {"mediumspringgreen", core::rgba8_t{0, 250, 154, 255}},
      {"springgreen", core::rgba8_t{0, 255, 127, 255}},
      {"mediumseagreen", core::rgba8_t{60, 179, 113, 255}},
      {"seagreen", core::rgba8_t{46, 139, 87, 255}},
      {"forestgreen", core::rgba8_t{34, 139, 34, 255}},
      {"green", core::rgba8_t{0, 128, 0, 255}},
      {"darkgreen", core::rgba8_t{0, 100, 0, 255}},
      {"yellowgreen", core::rgba8_t{154, 205, 50, 255}},
      {"olivedrab", core::rgba8_t{107, 142, 35, 255}},
      {"olive", core::rgba8_t{128, 128, 0, 255}},
      {"darkolivegreen", core::rgba8_t{85, 107, 47, 255}},
      {"mediumaquamarine", core::rgba8_t{102, 205, 170, 255}},
      {"darkseagreen", core::rgba8_t{143, 188, 143, 255}},
      {"lightseagreen", core::rgba8_t{32, 178, 170, 255}},
      {"darkcyan", core::rgba8_t{0, 139, 139, 255}},
      {"teal", core::rgba8_t{0, 128, 128, 255}},

      // Blue colors
      {"aqua", core::rgba8_t{0, 255, 255, 255}},
      {"cyan", core::rgba8_t{0, 255, 255, 255}},
      {"lightcyan", core::rgba8_t{224, 255, 255, 255}},
      {"paleturquoise", core::rgba8_t{175, 238, 238, 255}},
      {"aquamarine", core::rgba8_t{127, 255, 212, 255}},
      {"turquoise", core::rgba8_t{64, 224, 208, 255}},
      {"mediumturquoise", core::rgba8_t{72, 209, 204, 255}},
      {"darkturquoise", core::rgba8_t{0, 206, 209, 255}},
      {"cadetblue", core::rgba8_t{95, 158, 160, 255}},
      {"steelblue", core::rgba8_t{70, 130, 180, 255}},
      {"lightsteelblue", core::rgba8_t{176, 196, 222, 255}},
      {"powderblue", core::rgba8_t{176, 224, 230, 255}},
      {"lightblue", core::rgba8_t{173, 216, 230, 255}},
      {"skyblue", core::rgba8_t{135, 206, 235, 255}},
      {"lightskyblue", core::rgba8_t{135, 206, 250, 255}},
      {"deepskyblue", core::rgba8_t{0, 191, 255, 255}},
      {"dodgerblue", core::rgba8_t{30, 144, 255, 255}},
      {"cornflowerblue", core::rgba8_t{100, 149, 237, 255}},
      {"mediumslateblue", core::rgba8_t{123, 104, 238, 255}},
      {"royalblue", core::rgba8_t{65, 105, 225, 255}},
      {"blue", core::rgba8_t{0, 0, 255, 255}},
      {"mediumblue", core::rgba8_t{0, 0, 205, 255}},
      {"darkblue", core::rgba8_t{0, 0, 139, 255}},
      {"navy", core::rgba8_t{0, 0, 128, 255}},
      {"midnightblue", core::rgba8_t{25, 25, 112, 255}},

      // Brown colors
      {"cornsilk", core::rgba8_t{255, 248, 220, 255}},
      {"blanchedalmond", core::rgba8_t{255, 235, 205, 255}},
      {"bisque", core::rgba8_t{255, 228, 196, 255}},
      {"navajowhite", core::rgba8_t{255, 222, 173, 255}},
      {"wheat", core::rgba8_t{245, 222, 179, 255}},
      {"burlywood", core::rgba8_t{222, 184, 135, 255}},
      {"tan", core::rgba8_t{210, 180, 140, 255}},
      {"rosybrown", core::rgba8_t{188, 143, 143, 255}},
      {"sandybrown", core::rgba8_t{244, 164, 96, 255}},
      {"goldenrod", core::rgba8_t{218, 165, 32, 255}},
      {"darkgoldenrod", core::rgba8_t{184, 134, 11, 255}},
      {"peru", core::rgba8_t{205, 133, 63, 255}},
      {"chocolate", core::rgba8_t{210, 105, 30, 255}},
      {"saddlebrown", core::rgba8_t{139, 69, 19, 255}},
      {"sienna", core::rgba8_t{160, 82, 45, 255}},
      {"brown", core::rgba8_t{165, 42, 42, 255}},
      {"maroon", core::rgba8_t{128, 0, 0, 255}},

      // White colors
      {"white", core::rgba8_t{255, 255, 255, 255}},
      {"snow", core::rgba8_t{255, 250, 250, 255}},
      {"honeydew", core::rgba8_t{240, 255, 240, 255}},
      {"mintcream", core::rgba8_t{245, 255, 250, 255}},
      {"azure", core::rgba8_t{240, 255, 255, 255}},
      {"aliceblue", core::rgba8_t{240, 248, 255, 255}},
      {"ghostwhite", core::rgba8_t{248, 248, 255, 255}},
      {"whitesmoke", core::rgba8_t{245, 245, 245, 255}},
      {"seashell", core::rgba8_t{255, 245, 238, 255}},
      {"beige", core::rgba8_t{245, 245, 220, 255}},
      {"oldlace", core::rgba8_t{253, 245, 230, 255}},
      {"floralwhite", core::rgba8_t{255, 250, 240, 255}},
      {"ivory", core::rgba8_t{255, 255, 240, 255}},
      {"antiquewhite", core::rgba8_t{250, 235, 215, 255}},
      {"linen", core::rgba8_t{250, 240, 230, 255}},
      {"lavenderblush", core::rgba8_t{255, 240, 245, 255}},
      {"mistyrose", core::rgba8_t{255, 228, 225, 255}},

      // Gray colors
      {"gainsboro", core::rgba8_t{220, 220, 220, 255}},
      {"lightgray", core::rgba8_t{211, 211, 211, 255}},
      {"silver", core::rgba8_t{192, 192, 192, 255}},
      {"darkgray", core::rgba8_t{169, 169, 169, 255}},
      {"gray", core::rgba8_t{128, 128, 128, 255}},
      {"dimgray", core::rgba8_t{105, 105, 105, 255}},
      {"lightslategray", core::rgba8_t{119, 136, 153, 255}},
      {"slategray", core::rgba8_t{112, 128, 144, 255}},
      {"darkslategray", core::rgba8_t{47, 79, 79, 255}},
      {"black", core::rgba8_t{0, 0, 0, 255}},
  };

  // Case-insensitive search
  // Convert to lowercase for comparison
  std::string lower_name;
  lower_name.reserve(name.size());
  for (char c : name) {
    lower_name.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  }

  auto it = named_colors.find(lower_name);
  if (it != named_colors.end()) {
    return it->second;
  }
  return std::nullopt;
}

/**
 * @brief Check if a string is a valid CSS named color.
 * @param name The color name (case-insensitive)
 * @return true if it's a valid named color, false otherwise
 */
inline bool is_named_color(std::string_view name) { return get_named_color(name).has_value(); }

namespace named_literal {

/**
 * @brief String literal: CSS named color → rgba8_t.
 * @example "red"_color → rgba8_t{255, 0, 0, 255}
 * @example "coral"_color → rgba8_t{255, 127, 80, 255}
 * @throws std::invalid_argument if the name is not a valid CSS color.
 */
inline auto operator"" _color(const char* str, size_t len) {
  auto result = get_named_color(std::string_view(str, len));
  if (!result) {
    throw std::invalid_argument(std::string("colorcpp: unknown named color '") + std::string(str, len) + "'");
  }
  return *result;
}
}  // namespace named_literal

}  // namespace colorcpp::io::css
