#pragma once

#include <type_traits>

namespace color::core {
namespace details {

template <typename T, typename Scale, typename Tag>
struct color_base_impl {
  using value_type = T;
  using scale_type = Scale;
  using color_tag = Tag;

  static_assert(std::is_arithmetic_v<T>, "Color value_type must be arithmetic");
};

}  // namespace details

template <typename T, typename Scale, typename Tag>
struct color_base : public details::color_base_impl<T, Scale, Tag> {
 protected:
  using base = details::color_base_impl<T, Scale, Tag>;
};

}  // namespace color::core
