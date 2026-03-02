#pragma once

namespace color {
namespace maths {

template <typename T>
constexpr T abs(T v) noexcept {
  return v < 0 ? -v : v;
}

constexpr double fmod(double x, double y) noexcept {
  if (y == 0.0) return 0.0;
  return x - static_cast<long long>(x / y) * y;
}

template <typename Ret = int, typename T>
constexpr Ret round(T v) noexcept {
  return static_cast<Ret>(v >= 0 ? v + 0.5 : v - 0.5);
}

}  // namespace maths
}  // namespace color
