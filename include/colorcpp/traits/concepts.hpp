#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

namespace color::traits {

// ------------------------------------------------
// channel value
// ------------------------------------------------

template <typename T>
concept channel_value = std::is_arithmetic_v<T>;


// ------------------------------------------------
// channel traits
// ------------------------------------------------

template <typename T>
concept channel_traits_like = requires {
  { T::min };
  { T::max };
};


// ------------------------------------------------
// model traits
// ------------------------------------------------

template <typename T>
concept model_traits_like = requires {
  { T::channels } -> std::convertible_to<std::size_t>;
};


// ------------------------------------------------
// channel type
// ------------------------------------------------

template <typename T>
concept channel = requires(T c) {
  typename T::value_type;
  typename T::tag;

  { c.value } -> std::convertible_to<typename T::value_type>;
};


// ------------------------------------------------
// color model tag
// ------------------------------------------------

template <typename T>
concept model_tag = requires {
  typename T::model_tag;
};


// ------------------------------------------------
// color type
// ------------------------------------------------

template <typename T>
concept color = requires {
  typename T::value_type;
  typename T::model_type;
};

} // namespace color::traits
