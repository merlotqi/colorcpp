/**
 * @file css.hpp
 * @brief CSS Color Level 4 parsing (aggregate header).
 *
 * Implementation is split under @c io/css/: @ref parse_detail.hpp (atomic dispatch, @c color-mix),
 * @ref relative_color.hpp (@c rgb(from ...) / @c color(from ...) AST + evaluation),
 * @ref parse_public.hpp (top-level parsers), @ref parse_templates.hpp (@c parse_css_color&lt;T&gt;).
 *
 * Single-color forms use @ref css_parse_detail::try_atomic_rgba8_cursor /
 * @ref css_parse_detail::try_atomic_rgbaf_cursor (one pass, full-string @c eof).
 */

#pragma once

#include <colorcpp/io/css/parse_detail.hpp>
#include <colorcpp/io/css/parse_public.hpp>
#include <colorcpp/io/css/parse_templates.hpp>
#include <colorcpp/io/css/relative_color.hpp>
