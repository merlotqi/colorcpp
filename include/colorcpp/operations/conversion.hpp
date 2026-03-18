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

// Forward declaration required for two-hop dispatch inside color_cast_impl.
template <typename To, typename From>
constexpr To color_cast(const From& src);

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

// Clamp v to [0, 1] before scaling: floating-point arithmetic in color math can produce
// values like 1.0000001 that would exceed channel::max and throw in the constructor.
template <typename Color, std::size_t I>
constexpr typename Color::value_type from_unit(float v) {
  using channel_t = std::tuple_element_t<I, typename Color::channels_tuple>;
  v = std::clamp(v, 0.0f, 1.0f);
  float val = v * static_cast<float>(channel_t::max - channel_t::min) + static_cast<float>(channel_t::min);
  return static_cast<typename Color::value_type>(val);
}

// Returns the alpha channel as a unit [0,1] value.
// Only called on types where channel 3 is known to be alpha (RGB, HSL, HSV).
// CMYK must NOT use this helper — its channel 3 is K (black), not alpha.
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

// model_category maps a model tag to a category flag.
// To add a new model, specialize model_category<NewModelTag> in the new model's header
// and set the appropriate flag. No changes to this file are required.
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

// --- Conversion functions ---
// Each function works in normalised unit [0,1] space internally.
// Alpha is handled conditionally: only emitted when To has >= 4 channels.

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
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
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
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(l));
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
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
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
  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(h), from_unit<To, 1>(s), from_unit<To, 2>(v));
}

// CMYK channels are C, M, Y, K — channel 3 is K (black), NOT alpha.
// Alpha is not a CMYK concept; conversions to/from CMYK always use opaque alpha.
template <typename To, typename From>
constexpr To cmyk_to_rgb(const From& src) {
  float c = to_unit<From, 0>(src.template get_index<0>());
  float m = to_unit<From, 1>(src.template get_index<1>());
  float y = to_unit<From, 2>(src.template get_index<2>());
  float k = to_unit<From, 3>(src.template get_index<3>());

  float r = (1.0f - c) * (1.0f - k);
  float g = (1.0f - m) * (1.0f - k);
  float b = (1.0f - y) * (1.0f - k);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(1.0f));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

template <typename To, typename From>
constexpr To rgb_to_cmyk(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float k = 1.0f - std::max({r, g, b});
  float denom = 1.0f - k;
  float c = (denom > 0.0f) ? (1.0f - r - k) / denom : 0.0f;
  float m = (denom > 0.0f) ? (1.0f - g - k) / denom : 0.0f;
  float y = (denom > 0.0f) ? (1.0f - b - k) / denom : 0.0f;

  return pack_to<To>(from_unit<To, 0>(c), from_unit<To, 1>(m), from_unit<To, 2>(y), from_unit<To, 3>(k));
}

template <typename To, typename From, std::size_t... Is>
constexpr To same_model_cast_impl(const From& src, std::index_sequence<Is...>) {
  return pack_to<To>((Is < From::channels ? from_unit<To, Is>(to_unit<From, Is>(src.template get_index<Is>()))
                                          : from_unit<To, Is>(1.0f))...);
}

}  // namespace details


template <typename To, typename From, typename = void>
struct color_cast_impl {
  static constexpr To convert(const From& src) {
    using FromTag = details::extract_model_t<From>;
    using ToTag = details::extract_model_t<To>;
    using FromCat = details::model_category<FromTag>;
    using ToCat = details::model_category<ToTag>;

    // Same type, or same colour space with different precision/alpha variant
    if constexpr (std::is_same_v<FromTag, ToTag> || (FromCat::is_rgb && ToCat::is_rgb) ||
                  (FromCat::is_hsl && ToCat::is_hsl) || (FromCat::is_hsv && ToCat::is_hsv) ||
                  (FromCat::is_cmyk && ToCat::is_cmyk)) {
      return details::same_model_cast_impl<To>(src, std::make_index_sequence<To::channels>{});

      // Direct conversions: HSL ↔ RGB
    } else if constexpr (FromCat::is_hsl && ToCat::is_rgb) {
      return details::hsl_to_rgb<To>(src);
    } else if constexpr (FromCat::is_rgb && ToCat::is_hsl) {
      return details::rgb_to_hsl<To>(src);

      // Direct conversions: HSV ↔ RGB
    } else if constexpr (FromCat::is_hsv && ToCat::is_rgb) {
      return details::hsv_to_rgb<To>(src);
    } else if constexpr (FromCat::is_rgb && ToCat::is_hsv) {
      return details::rgb_to_hsv<To>(src);

      // Direct conversions: CMYK ↔ RGB
    } else if constexpr (FromCat::is_cmyk && ToCat::is_rgb) {
      return details::cmyk_to_rgb<To>(src);
    } else if constexpr (FromCat::is_rgb && ToCat::is_cmyk) {
      return details::rgb_to_cmyk<To>(src);

      // Two-hop via RGBaf: handles all other cross-category pairs
      // (HSL↔CMYK, HSV↔CMYK, new-model↔existing, etc.)
      // Only attempted when neither side is RGB to prevent infinite recursion.
    } else if constexpr (!FromCat::is_rgb && !ToCat::is_rgb) {
      auto mid = color_cast<core::rgbaf_t>(src);
      return color_cast<To>(mid);

    } else {
      static_assert(sizeof(To) == 0,
                    "colorcpp: unsupported conversion path. "
                    "Specialize color_cast_impl<To,From> to add support.");
      return To{};
    }
  }
};

template <typename To, typename From>
constexpr To color_cast(const From& src) {
  return color_cast_impl<To, From>::convert(src);
}

}  // namespace colorcpp::operations::conversion
