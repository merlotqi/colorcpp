/**
 * @file context.hpp
 * @brief Ambient context for CSS color parsing features that require external values.
 */

#pragma once

#include <colorcpp/core/rgb.hpp>
#include <functional>
#include <optional>

namespace colorcpp::io::css {

/**
 * @brief Context values for CSS colors that depend on the environment.
 *
 * Fields are optional on purpose: parsing a keyword such as @c currentColor or
 * a system color only succeeds when the corresponding context value is provided.
 */
struct parse_css_color_context {
  bool dark_theme = false;
  std::function<std::optional<core::rgbaf_t>(std::string_view)> variable_resolver{};
  std::optional<core::rgbaf_t> current_color = std::nullopt;

  std::optional<core::rgbaf_t> accent_color = std::nullopt;
  std::optional<core::rgbaf_t> accent_color_text = std::nullopt;
  std::optional<core::rgbaf_t> active_text = std::nullopt;
  std::optional<core::rgbaf_t> button_border = std::nullopt;
  std::optional<core::rgbaf_t> button_face = std::nullopt;
  std::optional<core::rgbaf_t> button_text = std::nullopt;
  std::optional<core::rgbaf_t> canvas = std::nullopt;
  std::optional<core::rgbaf_t> canvas_text = std::nullopt;
  std::optional<core::rgbaf_t> field = std::nullopt;
  std::optional<core::rgbaf_t> field_text = std::nullopt;
  std::optional<core::rgbaf_t> gray_text = std::nullopt;
  std::optional<core::rgbaf_t> highlight = std::nullopt;
  std::optional<core::rgbaf_t> highlight_text = std::nullopt;
  std::optional<core::rgbaf_t> link_text = std::nullopt;
  std::optional<core::rgbaf_t> mark = std::nullopt;
  std::optional<core::rgbaf_t> mark_text = std::nullopt;
  std::optional<core::rgbaf_t> selected_item = std::nullopt;
  std::optional<core::rgbaf_t> selected_item_text = std::nullopt;
  std::optional<core::rgbaf_t> visited_text = std::nullopt;
};

}  // namespace colorcpp::io::css
