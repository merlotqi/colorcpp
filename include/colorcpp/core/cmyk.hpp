/**
 * @file cmyk.hpp
 * @brief CMYK for print: either 8-bit-style 0–100 integers or float channels in [0, 1].
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::cmyk {

namespace channel {

struct c_tag {};
struct m_tag {};
struct y_tag {};
struct k_tag {};

using u8_cyan = traits::basic_channel<c_tag, uint8_t, 0, 100, 1>;
using u8_magenta = traits::basic_channel<m_tag, uint8_t, 0, 100, 1>;
using u8_yellow = traits::basic_channel<y_tag, uint8_t, 0, 100, 1>;
using u8_key = traits::basic_channel<k_tag, uint8_t, 0, 100, 1>;

using float_cyan = traits::basic_channel<c_tag, float, 0, 1>;
using float_magenta = traits::basic_channel<m_tag, float, 0, 1>;
using float_yellow = traits::basic_channel<y_tag, float, 0, 1>;
using float_key = traits::basic_channel<k_tag, float, 0, 1>;

}  // namespace channel

namespace model {

struct cmyk_u8 {};
struct cmyk_float {};

}  // namespace model

}  // namespace colorcpp::core::cmyk

namespace colorcpp::traits {

template <>
struct model_traits<core::cmyk::model::cmyk_u8> {
  using channels_type = std::tuple<core::cmyk::channel::u8_cyan, core::cmyk::channel::u8_magenta,
                                   core::cmyk::channel::u8_yellow, core::cmyk::channel::u8_key>;

  static constexpr std::string_view prefix = "cmyk";
};

template <>
struct model_traits<core::cmyk::model::cmyk_float> {
  using channels_type = std::tuple<core::cmyk::channel::float_cyan, core::cmyk::channel::float_magenta,
                                   core::cmyk::channel::float_yellow, core::cmyk::channel::float_key>;

  static constexpr std::string_view prefix = "cmyk";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief CMYK with @c c(), @c m(), @c y(), @c k().
 * @tparam Model cmyk_u8 (0–100) or cmyk_float ([0, 1]).
 */
template <typename Model>
struct basic_cmyk : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(cmyk::channel::c_tag, c)

  COLORCPP_CHANNEL_ACCESSOR(cmyk::channel::m_tag, m)

  COLORCPP_CHANNEL_ACCESSOR(cmyk::channel::y_tag, y)

  COLORCPP_CHANNEL_ACCESSOR(cmyk::channel::k_tag, k)
};

/** @brief CMYK with integer channels 0–100. */
using cmyk8_t = basic_cmyk<cmyk::model::cmyk_u8>;
/** @brief CMYK with float channels in [0, 1]. */
using cmyk_float_t = basic_cmyk<cmyk::model::cmyk_float>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_cmyk)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_cmyk)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_cmyk)
