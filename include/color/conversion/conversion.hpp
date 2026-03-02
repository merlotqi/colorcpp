#include <type_traits>

#include "cmyk_to_hsl.hpp"
#include "cmyk_to_hsv.hpp"
#include "cmyk_to_rgb.hpp"
#include "hsl_to_cmyk.hpp"
#include "hsl_to_hsv.hpp"
#include "hsl_to_rgb.hpp"
#include "hsv_to_cmyk.hpp"
#include "hsv_to_hsl.hpp"
#include "hsv_to_rgb.hpp"
#include "rgb_to_cmyk.hpp"
#include "rgb_to_hsl.hpp"
#include "rgb_to_hsv.hpp"

namespace color::conversion {

namespace details {

template <typename TargetType, typename SourceType>
struct converter {
  static constexpr TargetType apply(const SourceType& src) {
    if constexpr (std::is_same<TargetType, SourceType>::value) {
      return src;
    } else {
      return convert(src);
    }
  }
};

}  // namespace details

template <typename TargetType, typename SourceType>
constexpr TargetType convert(const SourceType& src) {
  return details::converter<TargetType, SourceType>::apply(src);
}

template <typename T, intptr_t Scale, typename SourceType>
constexpr auto to_hsv(const SourceType& src) {
  using target_t = core::basic_hsv<T, Scale>;
  return convert<target_t>(src);
}

template <typename T, intptr_t Scale, typename SourceType>
constexpr auto to_hsl(const SourceType& src) {
  using target_t = core::basic_hsl<T, Scale>;
  return convert<target_t>(src);
}

template <typename T, intptr_t Scale, typename SourceType>
constexpr auto to_rgb(const SourceType& src) {
  using target_t = core::basic_rgb<T, Scale>;
  return convert<target_t>(src);
}

}  // namespace color::conversion
