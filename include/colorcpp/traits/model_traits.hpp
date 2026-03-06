#pragma once

#include <cstddef>

#include "channel_tags.hpp"
#include "model_tags.hpp"

namespace color::traits {

// primary template
template <typename Model>
struct model_traits;

// ------------------------------------------------
// RGB
// ------------------------------------------------

template <>
struct model_traits<color::model::rgb> {
  static constexpr std::size_t channels = 3;

  using channel0 = color::channel::r;
  using channel1 = color::channel::g;
  using channel2 = color::channel::b;
};

// ------------------------------------------------
// RGBA
// ------------------------------------------------

template <>
struct model_traits<color::model::rgba> {
  static constexpr std::size_t channels = 4;

  using channel0 = color::channel::r;
  using channel1 = color::channel::g;
  using channel2 = color::channel::b;
  using channel3 = color::channel::a;
};

// ------------------------------------------------
// HSL
// ------------------------------------------------

template <>
struct model_traits<color::model::hsl> {
  static constexpr std::size_t channels = 3;

  using channel0 = color::channel::h;
  using channel1 = color::channel::s;
  using channel2 = color::channel::l;
};

// ------------------------------------------------
// LAB
// ------------------------------------------------

template <>
struct model_traits<color::model::lab> {
  static constexpr std::size_t channels = 3;

  using channel0 = color::channel::lab_l;
  using channel1 = color::channel::lab_a;
  using channel2 = color::channel::lab_b;
};

// ------------------------------------------------
// XYZ
// ------------------------------------------------

template <>
struct model_traits<color::model::xyz> {
  static constexpr std::size_t channels = 3;

  using channel0 = color::channel::x;
  using channel1 = color::channel::y;
  using channel2 = color::channel::z;
};

}  // namespace color::traits
