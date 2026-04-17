/**
 * @file relative_color.hpp
 * @brief Relative CSS color parsing and delayed evaluation for @c rgb(from ...) and @c color(from ...).
 */

#pragma once

#include <array>
#include <cctype>
#include <cmath>
#include <colorcpp/core/display_p3.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/io/css/color_function.hpp>
#include <colorcpp/io/css/context.hpp>
#include <colorcpp/io/css/details.hpp>
#include <colorcpp/io/css/named_colors.hpp>
#include <colorcpp/io/css/parse_detail.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace colorcpp::io::css {

enum class relative_color_function_kind {
  rgb,
  color,
};

enum class relative_color_space {
  srgb,
  srgb_linear,
  display_p3,
  display_p3_linear,
  a98_rgb,
  prophoto_rgb,
  rec2020,
  xyz_d50,
  xyz_d65,
};

struct channel_expression {
  enum class op {
    literal,
    percentage,
    identifier,
    add,
    sub,
    mul,
    div,
    unary_minus,
  };

  op operation = op::literal;
  float literal = 0.0f;
  std::string identifier;
  std::shared_ptr<channel_expression> left;
  std::shared_ptr<channel_expression> right;
};

struct relative_color {
  relative_color_function_kind function = relative_color_function_kind::rgb;
  std::string source_expression;
  std::optional<relative_color_space> target_space = std::nullopt;
  std::array<std::shared_ptr<channel_expression>, 4> channels{};
};

struct parsed_css_color {
  std::variant<core::rgbaf_t, relative_color> value;

  parsed_css_color() = default;
  parsed_css_color(const core::rgbaf_t& concrete) : value(concrete) {}
  parsed_css_color(core::rgbaf_t&& concrete) : value(std::move(concrete)) {}
  parsed_css_color(const relative_color& relative) : value(relative) {}
  parsed_css_color(relative_color&& relative) : value(std::move(relative)) {}

  bool is_concrete() const { return std::holds_alternative<core::rgbaf_t>(value); }
  bool is_relative() const { return std::holds_alternative<relative_color>(value); }
};

namespace relative_css_detail {

inline std::string ascii_lower_copy(std::string_view s) {
  std::string out;
  out.reserve(s.size());
  for (char ch : s) out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  return out;
}

inline bool is_context_color_keyword(std::string_view s) {
  return details::equals_ci(s, "currentcolor") || details::equals_ci(s, "accentcolor") ||
         details::equals_ci(s, "accentcolortext") || details::equals_ci(s, "activetext") ||
         details::equals_ci(s, "buttonborder") || details::equals_ci(s, "buttonface") ||
         details::equals_ci(s, "buttontext") || details::equals_ci(s, "canvas") ||
         details::equals_ci(s, "canvastext") || details::equals_ci(s, "field") || details::equals_ci(s, "fieldtext") ||
         details::equals_ci(s, "graytext") || details::equals_ci(s, "highlight") ||
         details::equals_ci(s, "highlighttext") || details::equals_ci(s, "linktext") || details::equals_ci(s, "mark") ||
         details::equals_ci(s, "marktext") || details::equals_ci(s, "selecteditem") ||
         details::equals_ci(s, "selecteditemtext") || details::equals_ci(s, "visitedtext");
}

inline bool looks_like_supported_function_name(std::string_view name) {
  return details::equals_ci(name, "rgb") || details::equals_ci(name, "rgba") || details::equals_ci(name, "hsl") ||
         details::equals_ci(name, "hsla") || details::equals_ci(name, "hwb") || details::equals_ci(name, "lab") ||
         details::equals_ci(name, "lch") || details::equals_ci(name, "oklab") || details::equals_ci(name, "oklch") ||
         details::equals_ci(name, "color") || details::equals_ci(name, "device-cmyk") ||
         details::equals_ci(name, "light-dark") || details::equals_ci(name, "color-mix") ||
         details::equals_ci(name, "var");
}

inline bool looks_like_function_call(std::string_view s) {
  details::trim(s);
  if (s.size() < 3 || s.back() != ')') return false;
  const size_t open = s.find('(');
  if (open == std::string_view::npos || open == 0) return false;
  std::string_view name = s.substr(0, open);
  details::trim(name);
  if (!looks_like_supported_function_name(name)) return false;

  int depth = 0;
  for (size_t i = open; i < s.size(); ++i) {
    const char ch = s[i];
    if (ch == '(') {
      ++depth;
    } else if (ch == ')') {
      --depth;
      if (depth == 0 && i + 1 != s.size()) return false;
      if (depth < 0) return false;
    }
  }
  return depth == 0;
}

inline bool looks_like_color_source_expression(std::string_view s) {
  details::trim(s);
  if (s.empty()) return false;
  if (s.front() == '#') return true;
  if (is_named_color(s) || is_context_color_keyword(s)) return true;
  return looks_like_function_call(s);
}

inline std::optional<std::string_view> try_extract_var_name(std::string_view s) {
  details::trim(s);
  if (s.size() < 6) return std::nullopt;
  details::Cursor c{s, 0};
  if (!c.consume_ci("var")) return std::nullopt;
  auto inner = details::consume_parenthesized_contents(c);
  if (!inner) return std::nullopt;
  c.skip_ws();
  if (!c.eof()) return std::nullopt;

  std::string_view payload = *inner;
  details::trim(payload);
  const size_t comma = details::find_top_level_comma(payload, 0);
  if (comma != std::string_view::npos) return std::nullopt;
  if (payload.size() < 3 || payload[0] != '-' || payload[1] != '-') return std::nullopt;
  return payload;
}

inline relative_color_space to_relative_space(details_color_fn::Space space) {
  switch (space) {
    case details_color_fn::Space::srgb:
      return relative_color_space::srgb;
    case details_color_fn::Space::srgb_linear:
      return relative_color_space::srgb_linear;
    case details_color_fn::Space::display_p3:
      return relative_color_space::display_p3;
    case details_color_fn::Space::display_p3_linear:
      return relative_color_space::display_p3_linear;
    case details_color_fn::Space::a98_rgb:
      return relative_color_space::a98_rgb;
    case details_color_fn::Space::prophoto_rgb:
      return relative_color_space::prophoto_rgb;
    case details_color_fn::Space::rec2020:
      return relative_color_space::rec2020;
    case details_color_fn::Space::xyz_d50:
      return relative_color_space::xyz_d50;
    case details_color_fn::Space::xyz_d65:
      return relative_color_space::xyz_d65;
  }

  return relative_color_space::srgb;
}

inline details_color_fn::Space to_color_function_space(relative_color_space space) {
  switch (space) {
    case relative_color_space::srgb:
      return details_color_fn::Space::srgb;
    case relative_color_space::srgb_linear:
      return details_color_fn::Space::srgb_linear;
    case relative_color_space::display_p3:
      return details_color_fn::Space::display_p3;
    case relative_color_space::display_p3_linear:
      return details_color_fn::Space::display_p3_linear;
    case relative_color_space::a98_rgb:
      return details_color_fn::Space::a98_rgb;
    case relative_color_space::prophoto_rgb:
      return details_color_fn::Space::prophoto_rgb;
    case relative_color_space::rec2020:
      return details_color_fn::Space::rec2020;
    case relative_color_space::xyz_d50:
      return details_color_fn::Space::xyz_d50;
    case relative_color_space::xyz_d65:
      return details_color_fn::Space::xyz_d65;
  }

  return details_color_fn::Space::srgb;
}

inline std::shared_ptr<channel_expression> make_literal_expr(float value, channel_expression::op operation) {
  auto expr = std::make_shared<channel_expression>();
  expr->operation = operation;
  expr->literal = value;
  return expr;
}

inline std::shared_ptr<channel_expression> make_identifier_expr(std::string ident) {
  auto expr = std::make_shared<channel_expression>();
  expr->operation = channel_expression::op::identifier;
  expr->identifier = std::move(ident);
  return expr;
}

inline std::shared_ptr<channel_expression> make_binary_expr(channel_expression::op operation,
                                                            std::shared_ptr<channel_expression> left,
                                                            std::shared_ptr<channel_expression> right) {
  auto expr = std::make_shared<channel_expression>();
  expr->operation = operation;
  expr->left = std::move(left);
  expr->right = std::move(right);
  return expr;
}

inline std::shared_ptr<channel_expression> make_unary_minus_expr(std::shared_ptr<channel_expression> operand) {
  auto expr = std::make_shared<channel_expression>();
  expr->operation = channel_expression::op::unary_minus;
  expr->left = std::move(operand);
  return expr;
}

class channel_expression_parser {
 public:
  explicit channel_expression_parser(std::string_view text, size_t start = 0) : text_(text), pos_(start) {}

  std::shared_ptr<channel_expression> parse_expression() { return parse_additive(); }

  size_t position() const { return pos_; }
  void set_position(size_t pos) { pos_ = pos; }

  void skip_ws() {
    while (pos_ < text_.size() && details::is_space(text_[pos_])) ++pos_;
  }

  bool eof() const { return pos_ >= text_.size(); }

  bool consume_char(char ch) {
    skip_ws();
    if (pos_ < text_.size() && text_[pos_] == ch) {
      ++pos_;
      return true;
    }
    return false;
  }

  bool consume_ci(std::string_view lit) {
    skip_ws();
    if (pos_ + lit.size() > text_.size()) return false;
    for (size_t i = 0; i < lit.size(); ++i) {
      if (std::tolower(static_cast<unsigned char>(text_[pos_ + i])) !=
          std::tolower(static_cast<unsigned char>(lit[i]))) {
        return false;
      }
    }
    pos_ += lit.size();
    return true;
  }

 private:
  std::shared_ptr<channel_expression> parse_additive() {
    auto lhs = parse_multiplicative();
    if (!lhs) return nullptr;
    while (true) {
      const size_t checkpoint = pos_;
      skip_ws();
      if (pos_ < text_.size() && text_[pos_] == '+') {
        ++pos_;
        auto rhs = parse_multiplicative();
        if (!rhs) return nullptr;
        lhs = make_binary_expr(channel_expression::op::add, lhs, rhs);
      } else if (pos_ < text_.size() && text_[pos_] == '-') {
        ++pos_;
        auto rhs = parse_multiplicative();
        if (!rhs) return nullptr;
        lhs = make_binary_expr(channel_expression::op::sub, lhs, rhs);
      } else {
        pos_ = checkpoint;
        break;
      }
    }
    return lhs;
  }

  std::shared_ptr<channel_expression> parse_multiplicative() {
    auto lhs = parse_unary();
    if (!lhs) return nullptr;
    while (true) {
      const size_t checkpoint = pos_;
      skip_ws();
      if (pos_ < text_.size() && text_[pos_] == '*') {
        ++pos_;
        auto rhs = parse_unary();
        if (!rhs) return nullptr;
        lhs = make_binary_expr(channel_expression::op::mul, lhs, rhs);
      } else if (pos_ < text_.size() && text_[pos_] == '/') {
        ++pos_;
        auto rhs = parse_unary();
        if (!rhs) return nullptr;
        lhs = make_binary_expr(channel_expression::op::div, lhs, rhs);
      } else {
        pos_ = checkpoint;
        break;
      }
    }
    return lhs;
  }

  std::shared_ptr<channel_expression> parse_unary() {
    skip_ws();
    if (consume_char('+')) return parse_unary();
    if (consume_char('-')) {
      auto operand = parse_unary();
      if (!operand) return nullptr;
      return make_unary_minus_expr(operand);
    }
    return parse_primary();
  }

  std::shared_ptr<channel_expression> parse_primary() {
    skip_ws();
    const size_t save = pos_;

    if (consume_ci("calc")) {
      if (!consume_char('(')) return nullptr;
      auto inner = parse_expression();
      if (!inner) return nullptr;
      if (!consume_char(')')) return nullptr;
      return inner;
    }

    pos_ = save;
    if (consume_char('(')) {
      auto inner = parse_expression();
      if (!inner) return nullptr;
      if (!consume_char(')')) return nullptr;
      return inner;
    }

    pos_ = save;
    if (consume_ci("none")) return make_literal_expr(0.0f, channel_expression::op::literal);

    pos_ = save;
    details::Cursor c{text_, pos_};
    if (auto number = c.parse_number()) {
      pos_ = c.i;
      if (pos_ < text_.size() && text_[pos_] == '%') {
        ++pos_;
        return make_literal_expr(static_cast<float>(*number), channel_expression::op::percentage);
      }
      return make_literal_expr(static_cast<float>(*number), channel_expression::op::literal);
    }

    pos_ = save;
    if (pos_ < text_.size() && (std::isalpha(static_cast<unsigned char>(text_[pos_])) || text_[pos_] == '_')) {
      const size_t start = pos_++;
      while (pos_ < text_.size()) {
        const char ch = text_[pos_];
        if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '-' || ch == '_') {
          ++pos_;
        } else {
          break;
        }
      }
      return make_identifier_expr(ascii_lower_copy(text_.substr(start, pos_ - start)));
    }

    return nullptr;
  }

  std::string_view text_;
  size_t pos_ = 0;
};

inline bool consume_required_ws(channel_expression_parser& parser) {
  const size_t before = parser.position();
  parser.skip_ws();
  return parser.position() > before;
}

inline std::optional<std::shared_ptr<channel_expression>> parse_full_expression(std::string_view text) {
  channel_expression_parser parser{text};
  auto expr = parser.parse_expression();
  if (!expr) return std::nullopt;
  parser.skip_ws();
  if (!parser.eof()) return std::nullopt;
  return expr;
}

inline size_t find_top_level_solidus(std::string_view s) {
  int depth = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    const char ch = s[i];
    if (ch == '(') {
      ++depth;
    } else if (ch == ')') {
      if (depth > 0) --depth;
    } else if (ch == '/' && depth == 0) {
      return i;
    }
  }
  return std::string_view::npos;
}

inline std::optional<std::array<std::shared_ptr<channel_expression>, 4>> parse_relative_rgb_tail(
    std::string_view tail) {
  channel_expression_parser parser{tail};
  std::array<std::shared_ptr<channel_expression>, 4> channels{};

  channels[0] = parser.parse_expression();
  if (!channels[0]) return std::nullopt;
  if (!consume_required_ws(parser)) return std::nullopt;

  channels[1] = parser.parse_expression();
  if (!channels[1]) return std::nullopt;
  if (!consume_required_ws(parser)) return std::nullopt;

  std::string_view remaining = tail.substr(parser.position());
  details::trim(remaining);
  const size_t solidus = find_top_level_solidus(remaining);
  std::string_view third = solidus == std::string_view::npos ? remaining : remaining.substr(0, solidus);
  details::trim(third);
  auto third_expr = parse_full_expression(third);
  if (!third_expr) return std::nullopt;
  channels[2] = *third_expr;

  if (solidus != std::string_view::npos) {
    std::string_view alpha = remaining.substr(solidus + 1);
    details::trim(alpha);
    auto alpha_expr = parse_full_expression(alpha);
    if (!alpha_expr) return std::nullopt;
    channels[3] = *alpha_expr;
  }
  return channels;
}

inline std::optional<relative_color> parse_relative_rgb_function(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  details::Cursor c{str, 0};
  const size_t save = c.i;
  const bool rgba = c.consume_ci("rgba");
  if (!rgba && !c.consume_ci("rgb")) return std::nullopt;
  auto inner_opt = details::consume_parenthesized_contents(c);
  if (!inner_opt) {
    c.i = save;
    return std::nullopt;
  }
  c.skip_ws();
  if (!c.eof()) {
    c.i = save;
    return std::nullopt;
  }

  std::string_view inner = *inner_opt;
  details::trim(inner);
  details::Cursor ic{inner, 0};
  if (!ic.consume_ci("from")) {
    c.i = save;
    return std::nullopt;
  }
  const size_t after_from = ic.i;
  ic.skip_ws();
  if (ic.i == after_from || ic.eof()) {
    c.i = save;
    return std::nullopt;
  }

  const size_t source_start = ic.i;
  int depth = 0;
  for (size_t i = source_start; i < inner.size(); ++i) {
    const char ch = inner[i];
    if (ch == '(') {
      ++depth;
    } else if (ch == ')') {
      if (depth > 0) --depth;
    } else if (depth == 0 && details::is_space(ch)) {
      std::string_view source = inner.substr(source_start, i - source_start);
      details::trim(source);
      size_t tail_start = i;
      while (tail_start < inner.size() && details::is_space(inner[tail_start])) ++tail_start;
      if (source.empty() || tail_start >= inner.size() || !looks_like_color_source_expression(source)) continue;

      auto channels = parse_relative_rgb_tail(inner.substr(tail_start));
      if (!channels) continue;

      relative_color out;
      out.function = relative_color_function_kind::rgb;
      out.source_expression = std::string(source);
      out.channels = std::move(*channels);
      return out;
    }
  }

  c.i = save;
  return std::nullopt;
}

inline std::optional<std::pair<relative_color_space, std::array<std::shared_ptr<channel_expression>, 4>>>
parse_relative_color_tail(std::string_view tail) {
  details::Cursor space_cursor{tail, 0};
  auto space = details_color_fn::parse_predefined_space(space_cursor);
  if (!space) return std::nullopt;

  channel_expression_parser parser{tail, space_cursor.i};
  if (!consume_required_ws(parser)) return std::nullopt;

  std::array<std::shared_ptr<channel_expression>, 4> channels{};
  channels[0] = parser.parse_expression();
  if (!channels[0]) return std::nullopt;
  if (!consume_required_ws(parser)) return std::nullopt;

  channels[1] = parser.parse_expression();
  if (!channels[1]) return std::nullopt;
  if (!consume_required_ws(parser)) return std::nullopt;

  std::string_view remaining = tail.substr(parser.position());
  details::trim(remaining);
  const size_t solidus = find_top_level_solidus(remaining);
  std::string_view third = solidus == std::string_view::npos ? remaining : remaining.substr(0, solidus);
  details::trim(third);
  auto third_expr = parse_full_expression(third);
  if (!third_expr) return std::nullopt;
  channels[2] = *third_expr;

  if (solidus != std::string_view::npos) {
    std::string_view alpha = remaining.substr(solidus + 1);
    details::trim(alpha);
    auto alpha_expr = parse_full_expression(alpha);
    if (!alpha_expr) return std::nullopt;
    channels[3] = *alpha_expr;
  }
  return std::pair<relative_color_space, std::array<std::shared_ptr<channel_expression>, 4>>{to_relative_space(*space),
                                                                                             std::move(channels)};
}

inline std::optional<relative_color> parse_relative_color_function(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;

  details::Cursor c{str, 0};
  const size_t save = c.i;
  if (!c.consume_ci("color")) return std::nullopt;
  auto inner_opt = details::consume_parenthesized_contents(c);
  if (!inner_opt) {
    c.i = save;
    return std::nullopt;
  }
  c.skip_ws();
  if (!c.eof()) {
    c.i = save;
    return std::nullopt;
  }

  std::string_view inner = *inner_opt;
  details::trim(inner);
  details::Cursor ic{inner, 0};
  if (!ic.consume_ci("from")) {
    c.i = save;
    return std::nullopt;
  }
  const size_t after_from = ic.i;
  ic.skip_ws();
  if (ic.i == after_from || ic.eof()) {
    c.i = save;
    return std::nullopt;
  }

  const size_t source_start = ic.i;
  int depth = 0;
  for (size_t i = source_start; i < inner.size(); ++i) {
    const char ch = inner[i];
    if (ch == '(') {
      ++depth;
    } else if (ch == ')') {
      if (depth > 0) --depth;
    } else if (depth == 0 && details::is_space(ch)) {
      std::string_view source = inner.substr(source_start, i - source_start);
      details::trim(source);
      size_t tail_start = i;
      while (tail_start < inner.size() && details::is_space(inner[tail_start])) ++tail_start;
      if (source.empty() || tail_start >= inner.size() || !looks_like_color_source_expression(source)) continue;

      auto tail = parse_relative_color_tail(inner.substr(tail_start));
      if (!tail) continue;

      relative_color out;
      out.function = relative_color_function_kind::color;
      out.source_expression = std::string(source);
      out.target_space = tail->first;
      out.channels = std::move(tail->second);
      return out;
    }
  }

  c.i = save;
  return std::nullopt;
}

inline std::optional<relative_color> parse_relative_css_color(std::string_view str) {
  if (auto rgb = parse_relative_rgb_function(str)) return rgb;
  if (auto color = parse_relative_color_function(str)) return color;
  return std::nullopt;
}

struct channel_binding {
  std::string_view name;
  float value = 0.0f;
};

struct channel_environment {
  std::array<channel_binding, 8> bindings{};
  size_t size = 0;

  void set(std::string_view name, float value) {
    if (size < bindings.size()) bindings[size++] = channel_binding{name, value};
  }

  std::optional<float> get(std::string_view name) const {
    for (size_t i = 0; i < size; ++i) {
      if (bindings[i].name == name) return bindings[i].value;
    }
    return std::nullopt;
  }
};

inline std::optional<float> evaluate_expression(const std::shared_ptr<channel_expression>& expr,
                                                const channel_environment& env, float percentage_scale) {
  if (!expr) return std::nullopt;

  switch (expr->operation) {
    case channel_expression::op::literal:
      return expr->literal;
    case channel_expression::op::percentage:
      return expr->literal * percentage_scale / 100.0f;
    case channel_expression::op::identifier:
      return env.get(expr->identifier);
    case channel_expression::op::add: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale);
      if (!lhs || !rhs) return std::nullopt;
      return *lhs + *rhs;
    }
    case channel_expression::op::sub: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale);
      if (!lhs || !rhs) return std::nullopt;
      return *lhs - *rhs;
    }
    case channel_expression::op::mul: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale);
      if (!lhs || !rhs) return std::nullopt;
      return *lhs * *rhs;
    }
    case channel_expression::op::div: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale);
      if (!lhs || !rhs || std::fabs(*rhs) <= 1e-8f) return std::nullopt;
      return *lhs / *rhs;
    }
    case channel_expression::op::unary_minus: {
      auto value = evaluate_expression(expr->left, env, percentage_scale);
      if (!value) return std::nullopt;
      return -*value;
    }
  }

  return std::nullopt;
}

template <typename Color>
inline std::optional<Color> evaluate_relative_color(const relative_color& parsed,
                                                    const parse_css_color_context& context);

inline std::optional<core::rgbaf_t> parse_concrete_css_source_rgbaf(std::string_view source_expression,
                                                                    const parse_css_color_context& context) {
  details::trim(source_expression);
  if (source_expression.empty()) return std::nullopt;

  details::Cursor c{source_expression, 0};
  if (auto parsed = css_parse_detail::parse_light_dark_rgbaf(c, context)) {
    c.skip_ws();
    if (c.eof()) return parsed;
  }

  c.i = 0;
  if (auto parsed = css_parse_detail::parse_color_mix_in_srgb_rgbaf(c, context)) {
    c.skip_ws();
    if (c.eof()) return parsed;
  }

  return css_parse_detail::parse_css_color_rgbaf_atomic_eof(source_expression, context);
}

template <typename Color>
inline std::optional<Color> resolve_source_color(std::string_view source_expression,
                                                 const parse_css_color_context& context) {
  if (auto var_name = try_extract_var_name(source_expression)) {
    if (!context.variable_resolver) return std::nullopt;
    auto resolved = context.variable_resolver(*var_name);
    if (!resolved) return std::nullopt;
    return operations::conversion::color_cast<Color>(*resolved);
  }

  if (auto relative = parse_relative_css_color(source_expression)) {
    return evaluate_relative_color<Color>(*relative, context);
  }

  if constexpr (std::is_same_v<Color, core::rgbaf_t>) {
    return parse_concrete_css_source_rgbaf(source_expression, context);
  } else {
    details::Cursor c{source_expression, 0};
    if (auto typed = parse_color_function_as<Color>(c)) {
      c.skip_ws();
      if (c.eof()) return typed;
    }

    auto concrete = parse_concrete_css_source_rgbaf(source_expression, context);
    if (!concrete) return std::nullopt;
    return operations::conversion::color_cast<Color>(*concrete);
  }
}

inline details_color_fn::rgb3f invert_mul3x3(const float m[9], float x, float y, float z) {
  const float det =
      m[0] * (m[4] * m[8] - m[5] * m[7]) - m[1] * (m[3] * m[8] - m[5] * m[6]) + m[2] * (m[3] * m[7] - m[4] * m[6]);
  if (std::fabs(det) <= 1e-8f) return {0.0f, 0.0f, 0.0f};
  const float inv_det = 1.0f / det;

  const float inv[9] = {
      (m[4] * m[8] - m[5] * m[7]) * inv_det, (m[2] * m[7] - m[1] * m[8]) * inv_det,
      (m[1] * m[5] - m[2] * m[4]) * inv_det, (m[5] * m[6] - m[3] * m[8]) * inv_det,
      (m[0] * m[8] - m[2] * m[6]) * inv_det, (m[2] * m[3] - m[0] * m[5]) * inv_det,
      (m[3] * m[7] - m[4] * m[6]) * inv_det, (m[1] * m[6] - m[0] * m[7]) * inv_det,
      (m[0] * m[4] - m[1] * m[3]) * inv_det,
  };
  return details_color_fn::mul3x3(inv, x, y, z);
}

inline float gamma_encode_a98(float v) { return details_color_fn::sign_pow(v, 256.0f / 563.0f); }

inline float gamma_encode_prophoto(float v) {
  const float av = std::fabs(v);
  if (av <= (1.0f / 512.0f)) return v * 16.0f;
  return std::copysign(std::pow(av, 1.0f / 1.8f), v);
}

inline float gamma_encode_rec2020(float v) {
  static constexpr float alpha = 1.0992968f;
  static constexpr float beta = 0.0180540f;
  const float av = std::fabs(v);
  if (av < beta) return v * 4.5f;
  return std::copysign(alpha * std::pow(av, 0.45f) - (alpha - 1.0f), v);
}

inline details_color_fn::rgb3f xyz_d65_to_d50(float x, float y, float z) {
  static constexpr float d50_to_d65[9] = {
      0.9554734f, -0.0230985f, 0.0632592f, -0.0283697f, 1.0099954f, 0.0210414f, 0.0123140f, -0.0205076f, 1.3303659f,
  };
  return invert_mul3x3(d50_to_d65, x, y, z);
}

inline details_color_fn::rgb3f xyz_d65_to_linear_a98(float x, float y, float z) {
  static constexpr float a98_to_xyz[9] = {
      0.5766690f, 0.1855582f, 0.1882286f, 0.2973450f, 0.6273636f, 0.0752915f, 0.0270314f, 0.0706889f, 0.9913375f,
  };
  return invert_mul3x3(a98_to_xyz, x, y, z);
}

inline details_color_fn::rgb3f xyz_d50_to_linear_prophoto(float x, float y, float z) {
  static constexpr float prophoto_to_xyz[9] = {
      0.7977605f, 0.1351858f, 0.0313493f, 0.2880711f, 0.7118432f, 0.0000857f, 0.0000000f, 0.0000000f, 0.8251046f,
  };
  return invert_mul3x3(prophoto_to_xyz, x, y, z);
}

inline details_color_fn::rgb3f xyz_d65_to_linear_rec2020(float x, float y, float z) {
  static constexpr float rec2020_to_xyz[9] = {
      0.6369580f, 0.1446169f, 0.1688810f, 0.2627002f, 0.6779981f, 0.0593017f, 0.0000000f, 0.0280727f, 1.0609851f,
  };
  return invert_mul3x3(rec2020_to_xyz, x, y, z);
}

struct source_channel_values {
  float c1 = 0.0f;
  float c2 = 0.0f;
  float c3 = 0.0f;
  float alpha = 1.0f;
};

inline std::optional<source_channel_values> resolve_relative_rgb_source(std::string_view source_expression,
                                                                        const parse_css_color_context& context) {
  auto source = resolve_source_color<core::rgbaf_t>(source_expression, context);
  if (!source) return std::nullopt;
  return source_channel_values{source->r() * 255.0f, source->g() * 255.0f, source->b() * 255.0f, source->a()};
}

inline std::optional<source_channel_values> resolve_relative_color_source(std::string_view source_expression,
                                                                          relative_color_space space,
                                                                          const parse_css_color_context& context) {
  auto alpha_source = resolve_source_color<core::rgbaf_t>(source_expression, context);
  if (!alpha_source) return std::nullopt;
  const float alpha = alpha_source->a();

  switch (space) {
    case relative_color_space::srgb:
      return source_channel_values{alpha_source->r(), alpha_source->g(), alpha_source->b(), alpha};
    case relative_color_space::srgb_linear: {
      auto source = resolve_source_color<core::linear_rgbf_t>(source_expression, context);
      if (!source) return std::nullopt;
      return source_channel_values{source->r(), source->g(), source->b(), alpha};
    }
    case relative_color_space::display_p3: {
      auto source = resolve_source_color<core::display_p3f_t>(source_expression, context);
      if (!source) return std::nullopt;
      return source_channel_values{source->r(), source->g(), source->b(), alpha};
    }
    case relative_color_space::display_p3_linear: {
      auto source = resolve_source_color<core::linear_display_p3f_t>(source_expression, context);
      if (!source) return std::nullopt;
      return source_channel_values{source->r(), source->g(), source->b(), alpha};
    }
    case relative_color_space::xyz_d65: {
      auto source = resolve_source_color<core::xyz_t>(source_expression, context);
      if (!source) return std::nullopt;
      return source_channel_values{source->x(), source->y(), source->z(), alpha};
    }
    case relative_color_space::xyz_d50: {
      auto source = resolve_source_color<core::xyz_t>(source_expression, context);
      if (!source) return std::nullopt;
      const auto xyz = xyz_d65_to_d50(source->x(), source->y(), source->z());
      return source_channel_values{xyz.x, xyz.y, xyz.z, alpha};
    }
    case relative_color_space::a98_rgb: {
      auto source = resolve_source_color<core::xyz_t>(source_expression, context);
      if (!source) return std::nullopt;
      const auto linear = xyz_d65_to_linear_a98(source->x(), source->y(), source->z());
      return source_channel_values{gamma_encode_a98(linear.x), gamma_encode_a98(linear.y), gamma_encode_a98(linear.z),
                                   alpha};
    }
    case relative_color_space::prophoto_rgb: {
      auto source = resolve_source_color<core::xyz_t>(source_expression, context);
      if (!source) return std::nullopt;
      const auto xyz_d50 = xyz_d65_to_d50(source->x(), source->y(), source->z());
      const auto linear = xyz_d50_to_linear_prophoto(xyz_d50.x, xyz_d50.y, xyz_d50.z);
      return source_channel_values{gamma_encode_prophoto(linear.x), gamma_encode_prophoto(linear.y),
                                   gamma_encode_prophoto(linear.z), alpha};
    }
    case relative_color_space::rec2020: {
      auto source = resolve_source_color<core::xyz_t>(source_expression, context);
      if (!source) return std::nullopt;
      const auto linear = xyz_d65_to_linear_rec2020(source->x(), source->y(), source->z());
      return source_channel_values{gamma_encode_rec2020(linear.x), gamma_encode_rec2020(linear.y),
                                   gamma_encode_rec2020(linear.z), alpha};
    }
  }

  return std::nullopt;
}

inline channel_environment make_rgb_environment(const source_channel_values& source) {
  channel_environment env;
  env.set("r", source.c1);
  env.set("g", source.c2);
  env.set("b", source.c3);
  env.set("a", source.alpha);
  env.set("alpha", source.alpha);
  return env;
}

inline channel_environment make_color_environment(relative_color_space space, const source_channel_values& source) {
  channel_environment env;
  if (space == relative_color_space::xyz_d50 || space == relative_color_space::xyz_d65) {
    env.set("x", source.c1);
    env.set("y", source.c2);
    env.set("z", source.c3);
  } else {
    env.set("r", source.c1);
    env.set("g", source.c2);
    env.set("b", source.c3);
  }
  env.set("a", source.alpha);
  env.set("alpha", source.alpha);
  return env;
}

template <typename Color>
inline std::optional<Color> evaluate_relative_color(const relative_color& parsed,
                                                    const parse_css_color_context& context) {
  if (parsed.function == relative_color_function_kind::rgb) {
    auto source = resolve_relative_rgb_source(parsed.source_expression, context);
    if (!source) return std::nullopt;

    const auto env = make_rgb_environment(*source);
    auto r = evaluate_expression(parsed.channels[0], env, 255.0f);
    auto g = evaluate_expression(parsed.channels[1], env, 255.0f);
    auto b = evaluate_expression(parsed.channels[2], env, 255.0f);
    if (!r || !g || !b) return std::nullopt;

    auto alpha =
        parsed.channels[3] ? evaluate_expression(parsed.channels[3], env, 1.0f) : std::optional<float>{source->alpha};
    if (!alpha) return std::nullopt;

    core::rgbaf_t out{
        std::clamp(*r / 255.0f, 0.0f, 1.0f),
        std::clamp(*g / 255.0f, 0.0f, 1.0f),
        std::clamp(*b / 255.0f, 0.0f, 1.0f),
        std::clamp(*alpha, 0.0f, 1.0f),
    };
    return operations::conversion::color_cast<Color>(out);
  }

  if (!parsed.target_space) return std::nullopt;
  auto source = resolve_relative_color_source(parsed.source_expression, *parsed.target_space, context);
  if (!source) return std::nullopt;

  const auto env = make_color_environment(*parsed.target_space, *source);
  auto c1 = evaluate_expression(parsed.channels[0], env, 1.0f);
  auto c2 = evaluate_expression(parsed.channels[1], env, 1.0f);
  auto c3 = evaluate_expression(parsed.channels[2], env, 1.0f);
  if (!c1 || !c2 || !c3) return std::nullopt;

  auto alpha =
      parsed.channels[3] ? evaluate_expression(parsed.channels[3], env, 1.0f) : std::optional<float>{source->alpha};
  if (!alpha) return std::nullopt;

  details_color_fn::parsed_color_function color_fn{
      to_color_function_space(*parsed.target_space), *c1, *c2, *c3, std::clamp(*alpha, 0.0f, 1.0f),
  };
  return details_color_fn::parsed_color_function_to_color<Color>(color_fn);
}

}  // namespace relative_css_detail

inline std::optional<parsed_css_color> parse_css_color_ast(std::string_view str) {
  details::trim(str);
  if (str.empty()) return std::nullopt;
  if (auto concrete = css_parse_detail::parse_css_color_rgbaf_atomic_eof(str)) return parsed_css_color{*concrete};
  if (auto relative = relative_css_detail::parse_relative_css_color(str)) return parsed_css_color{std::move(*relative)};
  return std::nullopt;
}

template <typename Color>
inline std::optional<Color> evaluate(const parsed_css_color& ast, const parse_css_color_context& context) {
  if (auto concrete = std::get_if<core::rgbaf_t>(&ast.value)) {
    return operations::conversion::color_cast<Color>(*concrete);
  }
  return relative_css_detail::evaluate_relative_color<Color>(std::get<relative_color>(ast.value), context);
}

template <typename Color>
inline std::optional<Color> evaluate(const parsed_css_color& ast,
                                     std::function<std::optional<core::rgbaf_t>(std::string_view)> variable_resolver) {
  parse_css_color_context context;
  context.variable_resolver = std::move(variable_resolver);
  return evaluate<Color>(ast, context);
}

}  // namespace colorcpp::io::css
