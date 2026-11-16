/**
 * @file parse_templates.hpp
 * @brief @c parse_css_color template API — now lives in @ref parse_public.hpp.
 *
 * The template suite moved down so parse_color_mix_item_space (color-mix()
 * item resolution, issue #110) can see it through parse_detail.hpp's forward
 * declaration. This header remains as an include shim for existing users.
 */

#pragma once

#include <colorcpp/io/css/parse_public.hpp>
