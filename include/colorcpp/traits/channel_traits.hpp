#pragma once

#include "channel_tags.hpp"

namespace color::traits {

// primary template
template <typename ChannelTag>
struct channel_traits;

// ------------------------------------------------
// RGB
// ------------------------------------------------

template <>
struct channel_traits<color::channel::r> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

template <>
struct channel_traits<color::channel::g> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

template <>
struct channel_traits<color::channel::b> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

template <>
struct channel_traits<color::channel::a> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

// ------------------------------------------------
// HSL
// ------------------------------------------------

template <>
struct channel_traits<color::channel::h> {
  static constexpr float min = 0.0f;
  static constexpr float max = 360.0f;
};

template <>
struct channel_traits<color::channel::s> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

template <>
struct channel_traits<color::channel::l> {
  static constexpr float min = 0.0f;
  static constexpr float max = 1.0f;
};

// ------------------------------------------------
// LAB
// ------------------------------------------------

template <>
struct channel_traits<color::channel::lab_l> {
  static constexpr float min = 0.0f;
  static constexpr float max = 100.0f;
};

template <>
struct channel_traits<color::channel::lab_a> {
  static constexpr float min = -128.0f;
  static constexpr float max = 127.0f;
};

template <>
struct channel_traits<color::channel::lab_b> {
  static constexpr float min = -128.0f;
  static constexpr float max = 127.0f;
};

// ------------------------------------------------
// XYZ
// ------------------------------------------------

template <>
struct channel_traits<color::channel::x> {
  static constexpr float min = 0.0f;
  static constexpr float max = 95.047f;
};

template <>
struct channel_traits<color::channel::y> {
  static constexpr float min = 0.0f;
  static constexpr float max = 100.0f;
};

template <>
struct channel_traits<color::channel::z> {
  static constexpr float min = 0.0f;
  static constexpr float max = 108.883f;
};

}  // namespace color::traits
