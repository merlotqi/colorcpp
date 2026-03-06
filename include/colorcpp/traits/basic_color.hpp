#pragma once

#include <array>
#include <cstddef>
#include <stdint.h>
#include "../traits/model_traits.hpp"
#include "../traits/concepts.hpp"

namespace color::core {

template <typename Model, traits::channel_value T>
struct basic_color {
  using model_type = Model;
  using value_type = T;

  static constexpr std::size_t channels = color::traits::model_traits<Model>::channels;

  using storage_type = std::array<T, channels>;

  storage_type data{};

  // ------------------------------------------------
  // constructors
  // ------------------------------------------------

  constexpr basic_color() = default;

  template <typename... Args>
    requires(sizeof...(Args) == channels)
  constexpr basic_color(Args... args) : data{static_cast<T>(args)...} {}

  // ------------------------------------------------
  // channel access
  // ------------------------------------------------

  constexpr T& operator[](std::size_t i) noexcept { return data[i]; }

  constexpr const T& operator[](std::size_t i) const noexcept { return data[i]; }

  // ------------------------------------------------
  // named accessors (optional but useful)
  // ------------------------------------------------

  constexpr T& c0() noexcept { return data[0]; }
  constexpr T& c1() noexcept { return data[1]; }
  constexpr T& c2() noexcept { return data[2]; }

  constexpr const T& c0() const noexcept { return data[0]; }
  constexpr const T& c1() const noexcept { return data[1]; }
  constexpr const T& c2() const noexcept { return data[2]; }

  // RGBA convenience
  constexpr T& c3() noexcept
    requires(channels > 3)
  {
    return data[3];
  }

  constexpr const T& c3() const noexcept
    requires(channels > 3)
  {
    return data[3];
  }

  // ------------------------------------------------
  // raw pointer access (SIMD / interop)
  // ------------------------------------------------

  constexpr T* data_ptr() noexcept { return data.data(); }

  constexpr const T* data_ptr() const noexcept { return data.data(); }

  // ------------------------------------------------
  // equality
  // ------------------------------------------------

  constexpr bool operator==(const basic_color&) const = default;
};

}  // namespace color::core


