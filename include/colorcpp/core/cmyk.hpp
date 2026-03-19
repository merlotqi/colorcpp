#pragma once

#include <colorcpp/core/color_base.hpp>

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
  static constexpr std::size_t channel_size = 4;
};

template <>
struct model_traits<core::cmyk::model::cmyk_float> {
  using channels_type = std::tuple<core::cmyk::channel::float_cyan, core::cmyk::channel::float_magenta,
                                   core::cmyk::channel::float_yellow, core::cmyk::channel::float_key>;

  static constexpr std::string_view prefix = "cmyk";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

template <typename Model>
struct basic_cmyk : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 private:
  template <typename Tag>
  constexpr auto& channel() {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return std::get<idx>(data);
  }

  template <typename Tag>
  constexpr const auto& channel() const {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return std::get<idx>(data);
  }

 public:
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::c_tag>>>
  constexpr auto& c() {
    return channel<cmyk::channel::c_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::c_tag>>>
  constexpr const auto& c() const {
    return channel<cmyk::channel::c_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::m_tag>>>
  constexpr auto& m() {
    return channel<cmyk::channel::m_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::m_tag>>>
  constexpr const auto& m() const {
    return channel<cmyk::channel::m_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::y_tag>>>
  constexpr auto& y() {
    return channel<cmyk::channel::y_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::y_tag>>>
  constexpr const auto& y() const {
    return channel<cmyk::channel::y_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::k_tag>>>
  constexpr auto& k() {
    return channel<cmyk::channel::k_tag>();
  }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, cmyk::channel::k_tag>>>
  constexpr const auto& k() const {
    return channel<cmyk::channel::k_tag>();
  }
};

using cmyk8_t = basic_cmyk<cmyk::model::cmyk_u8>;
using cmyk_float_t = basic_cmyk<cmyk::model::cmyk_float>;

}  // namespace colorcpp::core
