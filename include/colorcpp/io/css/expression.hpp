/**
 * @file expression.hpp
 * @brief Channel-expression AST, parser, and evaluator.
 *
 * Shared machinery for relative-color channel expressions (rgb(from ...),
 * hsl(from ...), ...) and absolute-color math functions (calc() — issue #114).
 * Split out of @ref relative_color.hpp: low-level value parsers
 * (@ref details::Cursor) must reach this code, but relative_color.hpp sits
 * above them in the include graph, so keeping it there would be a cycle.
 * This header depends only on @ref details.hpp and @ref context.hpp.
 */

#pragma once

#include <array>
#include <cctype>
#include <cmath>
#include <colorcpp/io/css/context.hpp>
#include <colorcpp/io/css/details.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace colorcpp::io::css {

struct channel_expression {
  enum class op {
    literal,
    percentage,
    identifier,
    variable_reference,
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

namespace relative_css_detail {

inline std::string ascii_lower_copy(std::string_view s) {
  std::string out;
  out.reserve(s.size());
  for (char ch : s) out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
  return out;
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

  // Channel-list mode: a binary +/- must be followed by whitespace (the CSS
  // calc() convention) so "25% -50%" parses as two channels — unary channel
  // minus vs. subtraction. calc() and parenthesized groups switch it off
  // internally to keep lenient tight arithmetic like calc(h+180).
  void set_channel_list_mode(bool on) { channel_list_mode_ = on; }

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
      if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) {
        if (channel_list_mode_ && (pos_ + 1 >= text_.size() || !details::is_space(text_[pos_ + 1]))) {
          pos_ = checkpoint;
          break;  // no whitespace after the sign: next channel, not an operator
        }
        const bool is_add = text_[pos_] == '+';
        ++pos_;
        auto rhs = parse_multiplicative();
        if (!rhs) return nullptr;
        lhs = make_binary_expr(is_add ? channel_expression::op::add : channel_expression::op::sub, lhs, rhs);
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
    // Every operand passes through here — including one per calc()/paren
    // nesting level and one per +/- operator — so a single budget bounds both
    // parser recursion depth and the AST depth the recursive evaluator walks.
    // Without it, ~20000 nesting levels or a ~50000-term additive chain
    // overflow the stack. 1024 is far above any real-world CSS expression.
    if (budget_-- <= 0) return nullptr;
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
      const bool saved = channel_list_mode_;
      channel_list_mode_ = false;  // calc() keeps lenient tight arithmetic (calc(h+180))
      auto inner = parse_expression();
      channel_list_mode_ = saved;
      if (!inner) return nullptr;
      if (!consume_char(')')) return nullptr;
      return inner;
    }

    pos_ = save;
    if (consume_char('(')) {
      const bool saved = channel_list_mode_;
      channel_list_mode_ = false;
      auto inner = parse_expression();
      channel_list_mode_ = saved;
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
      // Angle units attach directly to the number (no whitespace) and resolve
      // to degrees: deg, grad, rad, turn (CSS Color 4 §4).
      static constexpr std::array<std::pair<std::string_view, float>, 4> kAngleUnits = {
          std::pair<std::string_view, float>{"deg", 1.0f},
          {"grad", 0.9f},
          {"rad", 57.29577951308232f},
          {"turn", 360.0f},
      };
      for (const auto& unit : kAngleUnits) {
        const std::string_view suffix = unit.first;
        if (pos_ + suffix.size() > text_.size()) continue;
        bool match = true;
        for (size_t i = 0; i < suffix.size(); ++i) {
          if (std::tolower(static_cast<unsigned char>(text_[pos_ + i])) !=
              std::tolower(static_cast<unsigned char>(suffix[i]))) {
            match = false;
            break;
          }
        }
        if (!match) continue;
        const size_t end = pos_ + suffix.size();
        if (end < text_.size() &&
            (std::isalnum(static_cast<unsigned char>(text_[end])) || text_[end] == '-' || text_[end] == '_')) {
          continue;  // part of a longer identifier (e.g. "dega"), not a unit
        }
        pos_ = end;
        return make_literal_expr(static_cast<float>(*number) * unit.second, channel_expression::op::literal);
      }
      return make_literal_expr(static_cast<float>(*number), channel_expression::op::literal);
    }

    pos_ = save;
    if (consume_ci("var")) {
      if (!consume_char('(')) return nullptr;
      const size_t name_start = pos_;
      while (pos_ < text_.size() && text_[pos_] != ')') ++pos_;
      if (pos_ >= text_.size()) return nullptr;
      std::string_view var_name(text_.data() + name_start, pos_ - name_start);
      details::trim(var_name);
      ++pos_;  // consume ')'
      auto expr = std::make_shared<channel_expression>();
      expr->operation = channel_expression::op::variable_reference;
      expr->identifier = std::move(var_name);
      return expr;
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
  bool channel_list_mode_ = false;
  // Shared operand/nesting budget; see parse_unary.
  int budget_ = 1024;
};

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
                                                const channel_environment& env, float percentage_scale,
                                                const parse_css_color_context& context) {
  if (!expr) return std::nullopt;

  switch (expr->operation) {
    case channel_expression::op::literal:
      return expr->literal;
    case channel_expression::op::percentage:
      return expr->literal * percentage_scale / 100.0f;
    case channel_expression::op::identifier:
      return env.get(expr->identifier);
    case channel_expression::op::variable_reference: {
      if (context.numeric_variable_resolver) {
        auto numeric_value = context.numeric_variable_resolver(expr->identifier);
        if (numeric_value) return *numeric_value;
      }
      if (context.variable_resolver) {
        auto color_value = context.variable_resolver(expr->identifier);
        if (color_value) return color_value->r();
      }
      return std::nullopt;
    }
    case channel_expression::op::add: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale, context);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale, context);
      if (!lhs || !rhs) return std::nullopt;
      return *lhs + *rhs;
    }
    case channel_expression::op::sub: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale, context);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale, context);
      if (!lhs || !rhs) return std::nullopt;
      return *lhs - *rhs;
    }
    case channel_expression::op::mul: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale, context);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale, context);
      if (!lhs || !rhs) return std::nullopt;
      return *lhs * *rhs;
    }
    case channel_expression::op::div: {
      auto lhs = evaluate_expression(expr->left, env, percentage_scale, context);
      auto rhs = evaluate_expression(expr->right, env, percentage_scale, context);
      if (!lhs || !rhs || std::fabs(*rhs) <= 1e-8f) return std::nullopt;
      return *lhs / *rhs;
    }
    case channel_expression::op::unary_minus: {
      auto value = evaluate_expression(expr->left, env, percentage_scale, context);
      if (!value) return std::nullopt;
      return -*value;
    }
  }

  return std::nullopt;
}

}  // namespace relative_css_detail
}  // namespace colorcpp::io::css
