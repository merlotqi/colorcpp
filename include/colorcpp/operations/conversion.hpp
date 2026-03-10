#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/core/hsv.hpp>
#include <colorcpp/core/rgb.hpp>
#include <tuple>
#include <type_traits>
#include <utility>

namespace colorcpp::operations::conversion {

namespace details {

template <typename T>
struct extract_model;

template <template <typename...> class Color, typename Model, typename... Args>
struct extract_model<Color<Model, Args...>> {
  using type = Model;
};

template <typename T>
using extract_model_t = typename extract_model<T>::type;

template <typename Color, std::size_t I>
constexpr float to_unit(typename Color::value_type v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  return static_cast<float>(v - channel_t::min) / static_cast<float>(channel_t::max - channel_t::min);
}

template <typename Color, std::size_t I>
constexpr typename Color::value_type from_unit(float v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  float val = v * static_cast<float>(channel_t::max - channel_t::min) + static_cast<float>(channel_t::min);
  return static_cast<typename Color::value_type>(val);
}

template <typename From>
constexpr float get_src_alpha(const From& src) {
  if constexpr (From::channels >= 4)
    return to_unit<From, 3>(src.template get_index<3>());
  else
    return 1.0f;
}

template <typename To, std::size_t... Is, typename... Args>
constexpr To pack_to_impl(std::index_sequence<Is...>, Args&&... args) {
  auto tp = std::forward_as_tuple(std::forward<Args>(args)...);
  return To{std::get<Is>(std::move(tp))...};
}

template <typename To, typename... Args>
constexpr To pack_to(Args&&... args) {
  if constexpr (sizeof...(args) > To::channels) {
    return pack_to_impl<To>(std::make_index_sequence<To::channels>{}, std::forward<Args>(args)...);
  } else {
    return To{std::forward<Args>(args)...};
  }
}

template <typename ModelTag>
struct model_category {
  static constexpr bool is_rgb =
      std::is_same_v<ModelTag, core::rgb::model::rgb8> || std::is_same_v<ModelTag, core::rgb::model::rgba8> ||
      std::is_same_v<ModelTag, core::rgb::model::rgb_float> || std::is_same_v<ModelTag, core::rgb::model::rgba_float>;

  static constexpr bool is_hsl =
      std::is_same_v<ModelTag, core::hsl::model::hsl> || std::is_same_v<ModelTag, core::hsl::model::hsla>;

  static constexpr bool is_hsv =
      std::is_same_v<ModelTag, core::hsv::model::hsv> || std::is_same_v<ModelTag, core::hsv::model::hsva>;

  static constexpr bool is_cmyk =
      std::is_same_v<ModelTag, core::cmyk::model::cmyk_u8> || std::is_same_v<ModelTag, core::cmyk::model::cmyk_float>;
};

template <typename To, typename From>
constexpr To hsl_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float l = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  auto hue2rgb = [](float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
  };

  float r{0.0f}, g{0.0f}, b{0.0f};
  if (s == 0.0f) {
    r = g = b = l;
  } else {
    float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
    float p = 2.0f * l - q;
    r = hue2rgb(p, q, h + 1.0f / 3.0f);
    g = hue2rgb(p, q, h);
    b = hue2rgb(p, q, h - 1.0f / 3.0f);
  }
  return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
}

template <typename To, typename From>
constexpr To rgb_to_hsl(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float h = 0.0f, s = 0.0f, l = (max + min) / 2.0f;

  if (max != min) {
    float d = max - min;
    s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }
  return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l), from_unit<To, 3>(a));
}

template <typename To, typename From>
constexpr To hsv_to_rgb(const From& src) {
  float h = to_unit<From, 0>(src.template get_index<0>());
  float s = to_unit<From, 1>(src.template get_index<1>());
  float v = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  int i = static_cast<int>(h * 6.0f);
  float f = h * 6.0f - static_cast<float>(i);
  float p = v * (1.0f - s);
  float q = v * (1.0f - f * s);
  float t = v * (1.0f - (1.0f - f) * s);

  float r = 0, g = 0, b = 0;
  switch (i % 6) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
  }
  return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
}

template <typename To, typename From>
constexpr To rgb_to_hsv(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());
  float a = get_src_alpha(src);

  float max = std::max({r, g, b}), min = std::min({r, g, b});
  float d = max - min;
  float h = 0.0f, s = (max == 0.0f ? 0.0f : d / max), v = max;

  if (max != min) {
    if (max == r)
      h = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (max == g)
      h = (b - r) / d + 2.0f;
    else
      h = (r - g) / d + 4.0f;
    h /= 6.0f;
  }
  return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v), from_unit<To, 3>(a));
}

template <typename To, typename From, std::size_t... Is>
constexpr To same_model_cast_impl(const From& src, std::index_sequence<Is...>) {
  return pack_to<To>((Is < From::channels ? from_unit<To, Is>(to_unit<From, Is>(src.template get_index<Is>()))
                                          : from_unit<To, Is>(1.0f))...);
}

}  // namespace details

template <typename To, typename From>
constexpr To color_cast(const From& src) {
  using FromTag = details::extract_model_t<From>;
  using ToTag = details::extract_model_t<To>;

  using FromCat = details::model_category<FromTag>;
  using ToCat = details::model_category<ToTag>;

  if constexpr (std::is_same_v<FromTag, ToTag> || (FromCat::is_rgb && ToCat::is_rgb)) {
    return details::same_model_cast_impl<To>(src, std::make_index_sequence<To::channels>{});
  } else {
    if constexpr (FromCat::is_hsl && ToCat::is_rgb)
      return details::hsl_to_rgb<To>(src);
    else if constexpr (FromCat::is_rgb && ToCat::is_hsl)
      return details::rgb_to_hsl<To>(src);
    else if constexpr (FromCat::is_hsv && ToCat::is_rgb)
      return details::hsv_to_rgb<To>(src);
    else if constexpr (FromCat::is_rgb && ToCat::is_hsv)
      return details::rgb_to_hsv<To>(src);

    else if constexpr (FromCat::is_hsl && ToCat::is_hsv) {
      auto mid = details::hsl_to_rgb<core::rgbaf_t>(src);
      return details::rgb_to_hsv<To>(mid);
    } else if constexpr (FromCat::is_hsv && ToCat::is_hsl) {
      auto mid = details::hsv_to_rgb<core::rgbaf_t>(src);
      return details::rgb_to_hsl<To>(mid);
    } else {
      static_assert(sizeof(To) == 0, "colorcpp: Unsupported conversion path.");
      return To{};
    }
  }
}

}  // namespace colorcpp::operations::conversion
