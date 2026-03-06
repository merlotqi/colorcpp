#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/traits/concepts.hpp>
#include <ratio>

namespace color::core {

template <typename T, typename Scale = std::ratio<1>>
struct basic_xyz : color_base<T, Scale, category::xyz> {
  static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");

  static constexpr T full_range = static_cast<T>(Scale::den) / static_cast<T>(Scale::num);

  T x, y, z;

  constexpr basic_xyz() : x(0), y(0), z(0) {}
  constexpr basic_xyz(T x_val, T y_val, T z_val);

  template <long long X_raw, long long Y_raw, long long Z_raw>
  static constexpr basic_xyz make() {
    constexpr T static_x = static_cast<T>(X_raw);
    constexpr T static_y = static_cast<T>(Y_raw);
    constexpr T static_z = static_cast<T>(Z_raw);

    static_assert(is_valid_val(static_x, static_y, static_z), "RGBA value out of range!");

    return {static_x, static_y, static_z};
  }

  static constexpr bool is_valid_val(T xv, T yv, T zv) {
    auto in_range = [](T v) { return v >= T(0) && v <= full_range; };
    return in_range(xv) && in_range(yv) && in_range(zv);
  }

  constexpr bool is_valid() const { return is_valid_val(x, y, z); }
};

using xyz_t = basic_xyz<float, std::ratio<1, 100>>;

}  // namespace color::core
