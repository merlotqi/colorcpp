#pragma once

#include <cmath>
#include <colorcpp/operations/conversion.hpp>
#include <colorcpp/operations/interpolate.hpp>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace colorcpp::operations::palette {

template <typename Color>
class palette_set {
 public:
  using color_type = Color;

  palette_set() = default;
  palette_set(std::initializer_list<Color> colors) : colors_(colors) {}
  palette_set(const std::vector<Color>& colors) : colors_(colors) {}

  void add(const Color& c) { colors_.push_back(c); }
  const std::vector<Color>& colors() const { return colors_; }

  size_t size() const { return colors_.size(); }
  bool empty() const { return colors_.empty(); }

  Color operator[](size_t index) const {
    if (colors_.empty()) return Color{};
    return colors_[index % colors_.size()];
  }

  Color at(size_t index) const {
    if (index >= colors_.size()) throw std::out_of_range("colorcpp: palette index out of range");
    return colors_[index];
  }

  auto begin() { return colors_.begin(); }
  auto end() { return colors_.end(); }
  auto begin() const { return colors_.begin(); }
  auto end() const { return colors_.end(); }
  auto cbegin() const { return colors_.cbegin(); }
  auto cend() const { return colors_.cend(); }

 private:
  std::vector<Color> colors_;
};

namespace details {
// h and degrees are both in [0, 360] degree space (matching f32_hue channel range)
inline float rotate_hue(float h, float degrees) {
  float res = std::fmod(h + degrees, 360.0f);
  if (res < 0.0f) res += 360.0f;
  return res;
}

template <typename HSLColor>
constexpr HSLColor modify_hue(const HSLColor& src, float new_h) {
  if constexpr (HSLColor::channels == 4) {
    return HSLColor{new_h, src.template get_index<1>(), src.template get_index<2>(), src.template get_index<3>()};
  } else {
    return HSLColor{new_h, src.template get_index<1>(), src.template get_index<2>()};
  }
}

}  // namespace details

template <typename Color>
auto linear_scale(const Color& start, const Color& end, size_t count) {
  palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(interpolate::lerp(start, end, t));
  }
  return p;
}

template <typename Color>
auto visual_scale(const Color& start, const Color& end, size_t count) {
  palette_set<Color> p;
  if (count == 0) return p;
  if (count == 1) {
    p.add(start);
    return p;
  }

  for (size_t i = 0; i < count; ++i) {
    float t = static_cast<float>(i) / (count - 1);
    p.add(interpolate::lerp_hsl(start, end, t));
  }
  return p;
}

struct generate {
  template <typename Color>
  static auto complementary(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float next_h = details::rotate_hue(h.template get_index<0>(), 180.0f);
    auto comp_h = details::modify_hue(h, next_h);

    return palette_set<Color>{base, color_cast<Color>(comp_h)};
  }

  template <typename Color>
  static auto analogous(const Color& base, float angle = 30.0f) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), -angle);
    float h2 = details::rotate_hue(h.template get_index<0>(), angle);

    return palette_set<Color>{color_cast<Color>(details::modify_hue(h, h1)), base,
                              color_cast<Color>(details::modify_hue(h, h2))};
  }

  template <typename Color>
  static auto triadic(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 120.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 240.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2))};
  }

  template <typename Color>
  static auto split_complementary(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 150.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 210.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2))};
  }

  template <typename Color>
  static auto tetradic(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 60.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 180.0f);
    float h3 = details::rotate_hue(h.template get_index<0>(), 240.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2)),
                              color_cast<Color>(details::modify_hue(h, h3))};
  }

  template <typename Color>
  static auto square(const Color& base) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    float h1 = details::rotate_hue(h.template get_index<0>(), 90.0f);
    float h2 = details::rotate_hue(h.template get_index<0>(), 180.0f);
    float h3 = details::rotate_hue(h.template get_index<0>(), 270.0f);

    return palette_set<Color>{base, color_cast<Color>(details::modify_hue(h, h1)),
                              color_cast<Color>(details::modify_hue(h, h2)),
                              color_cast<Color>(details::modify_hue(h, h3))};
  }

  template <typename Color>
  static auto monochromatic(const Color& base, size_t count = 5) {
    using namespace conversion;
    auto h = color_cast<core::hsla_float_t>(base);

    core::hsla_float_t low{h.template get_index<0>(), h.template get_index<1>(), 20.0f, h.template get_index<3>()};
    core::hsla_float_t high{h.template get_index<0>(), h.template get_index<1>(), 90.0f, h.template get_index<3>()};

    return visual_scale(color_cast<Color>(low), color_cast<Color>(high), count);
  }
};

}  // namespace colorcpp::operations::palette
