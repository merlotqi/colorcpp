# Wide-Color-Gamut Completion — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Complete the wide-color-gamut feature: add Rec.2020 color type, standalone chromatic adaptation module, ProPhoto gamut mapping, unblock Display P3 tests, and add missing examples.

**Architecture:** Follow existing patterns from Adobe RGB / Display P3 / ProPhoto RGB for the Rec.2020 addition (core type + conversion functions + graph registration + CSS wiring). Chromatic adaptation is a new standalone algorithm module consumed by the refactored ProPhoto RGB conversion. ProPhoto gamut mapping extends the existing multi-colorspace gamut module.

**Tech Stack:** C++17, header-only, Google Test, CMake

## Global Constraints

- C++17, header-only library — all new code goes in `include/colorcpp/` as inline/template headers
- Follow existing naming conventions: `snake_case` files, `CamelCase` types, `namespace::sub_namespace` pattern
- New color type follows the Display P3 / Adobe RGB layered pattern (gamma + linear variants)
- Conversion graph edges use `COLORCPP_REGISTER_CONVERSION_BIDIR` macros in `index.hpp`
- Tests use Google Test with `EXPECT_FLOAT_EQ` / `EXPECT_NEAR` / `EXPECT_THROW`
- All examples print ANSI swatches to stdout

---

## File Structure Map

```
include/colorcpp/
  core/
    rec2020.hpp                        [NEW] Rec.2020 core type definitions
    core.hpp                           [EDIT] Add #include rec2020.hpp
  operations/conversion/
    functions/
      rec2020.hpp                      [NEW] Gamma + matrix + sRGB shortcut
      prophoto_rgb.hpp                 [EDIT] Split combined matrices, use explicit Bradford
      index.hpp                        [EDIT] Register rec2020 edges
    color_space_registry.hpp           [EDIT] Add Rec.2020 color_traits
  algorithms/
    chromatic_adaptation.hpp           [NEW] Bradford, Von Kries, white point constants
    algorithms.hpp                     [EDIT] Add #include chromatic_adaptation.hpp
    gamut/
      multi_colorspace.hpp             [EDIT] Add prophoto_rgb to gamut_type
      details.hpp                      [EDIT] Add prophoto gamma helpers if needed
  io/css/
    color_function.hpp                 [EDIT] Wire rec2020 parser to core type
    relative_color.hpp                 [EDIT] Add parse overload for rec2020_rgbaf_t

tests/
  core/
    test_display_p3.cpp                [EDIT] Uncomment 7 blocked tests
    test_rec2020.cpp                   [NEW] Rec.2020 test suite
  algorithms/
    test_chromatic_adaptation.cpp      [NEW] Chromatic adaptation test suite
    test_gamut.cpp                     [EDIT] Add ProPhoto RGB gamut cases

examples/
  adobe_rgb_example.cpp                [NEW] Adobe RGB usage example
  prophoto_rgb_example.cpp             [NEW] ProPhoto RGB usage example
  rec2020_example.cpp                  [NEW] Rec.2020 usage example
  chromatic_adaptation_example.cpp     [NEW] Chromatic adaptation example
```

---

### Task 1: Rec.2020 Core Type

**Files:**
- Create: `include/colorcpp/core/rec2020.hpp`
- Create: `tests/core/test_rec2020.cpp` (construction + boundary tests only)

**Interfaces:**
- Produces: `colorcpp::core::rec2020_rgbf_t`, `rec2020_rgbaf_t`, `linear_rec2020_rgbf_t`, `linear_rec2020_rgbaf_t` — all aliases for `basic_rec2020<Model>`
- Produces: model tags `rec2020::model::rec2020`, `rec2020::model::rec2020a`, `rec2020::model::linear_rec2020f`, `rec2020::model::linear_rec2020af`
- Produces: channel tags `rec2020::channel::r_tag`, `g_tag`, `b_tag`, `a_tag`
- Produces: stream operators `operator<<` / `operator>>` for `basic_rec2020<Model>`

- [ ] **Step 1: Create the Rec.2020 core header**

Write `include/colorcpp/core/rec2020.hpp` following the exact pattern of Adobe RGB (`adobe_rgb.hpp`) — same structure, different namespace names and prefix strings:

```cpp
/**
 * @file rec2020.hpp
 * @brief Rec.2020 (BT.2020) color space (D65 white point, BT.709-style piecewise transfer).
 *
 * ITU-R BT.2020 (Rec.2020) is the UHDTV standard, covering approximately 75% of
 * the CIE 1931 visible gamut. It uses the same D65 white point as sRGB but with
 * significantly wider primaries.
 *
 * Transfer function: BT.709-style piecewise (α=1.0993, β=0.0181).
 *
 * @see https://www.itu.int/rec/R-REC-BT.2020
 */

#pragma once

#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>
#include <colorcpp/core/linear_rgb.hpp>

namespace colorcpp::core::rec2020 {

namespace channel {

struct r_tag {};
struct g_tag {};
struct b_tag {};
struct a_tag {};

using f32_red = traits::basic_channel<r_tag, float, 0, 1, 1>;
using f32_green = traits::basic_channel<g_tag, float, 0, 1, 1>;
using f32_blue = traits::basic_channel<b_tag, float, 0, 1, 1>;
using f32_alpha = traits::basic_channel<a_tag, float, 0, 1, 1>;

}  // namespace channel

namespace model {

struct rec2020 {};
struct rec2020a {};
struct linear_rec2020f {};
struct linear_rec2020af {};

}  // namespace model

}  // namespace colorcpp::core::rec2020

namespace colorcpp::traits {

template <>
struct model_traits<core::rec2020::model::rec2020> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue>;
  static constexpr std::string_view prefix = "rec2020";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::rec2020::model::rec2020a> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue, core::rec2020::channel::f32_alpha>;
  static constexpr std::string_view prefix = "rec2020a";
  static constexpr std::size_t channel_size = 4;
};

template <>
struct model_traits<core::rec2020::model::linear_rec2020f> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue>;
  static constexpr std::string_view prefix = "linear-rec2020";
  static constexpr std::size_t channel_size = 3;
};

template <>
struct model_traits<core::rec2020::model::linear_rec2020af> {
  using channels_type = std::tuple<core::rec2020::channel::f32_red, core::rec2020::channel::f32_green,
                                   core::rec2020::channel::f32_blue, core::rec2020::channel::f32_alpha>;
  static constexpr std::string_view prefix = "linear-rec2020a";
  static constexpr std::size_t channel_size = 4;
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief Rec.2020 / Rec.2020A with @c r(), @c g(), @c b(), and @c a() when alpha is present.
 * @tparam Model rec2020::model::rec2020, rec2020a, linear_rec2020f, or linear_rec2020af.
 */
template <typename Model>
struct basic_rec2020 : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 private:
  template <typename Tag>
  constexpr auto& channel() {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return data[idx];
  }

  template <typename Tag>
  constexpr const auto& channel() const {
    constexpr std::size_t idx = traits::channel_index_v<Model, Tag>;
    return data[idx];
  }

 public:
  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::r_tag>>>
  constexpr auto& r() { return channel<rec2020::channel::r_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::r_tag>>>
  constexpr const auto& r() const { return channel<rec2020::channel::r_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::g_tag>>>
  constexpr auto& g() { return channel<rec2020::channel::g_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::g_tag>>>
  constexpr const auto& g() const { return channel<rec2020::channel::g_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::b_tag>>>
  constexpr auto& b() { return channel<rec2020::channel::b_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::b_tag>>>
  constexpr const auto& b() const { return channel<rec2020::channel::b_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::a_tag>>>
  constexpr auto& a() { return channel<rec2020::channel::a_tag>(); }

  template <typename M = Model, typename = std::enable_if_t<traits::has_channel_tag_v<M, rec2020::channel::a_tag>>>
  constexpr const auto& a() const { return channel<rec2020::channel::a_tag>(); }
};

/** @brief Rec.2020 with float channels. */
using rec2020_rgbf_t = basic_rec2020<rec2020::model::rec2020>;
/** @brief Rec.2020A with float channels. */
using rec2020_rgbaf_t = basic_rec2020<rec2020::model::rec2020a>;
/** @brief Linear Rec.2020 with float channels. */
using linear_rec2020_rgbf_t = basic_rec2020<rec2020::model::linear_rec2020f>;
/** @brief Linear Rec.2020A with float channels. */
using linear_rec2020_rgbaf_t = basic_rec2020<rec2020::model::linear_rec2020af>;

}  // namespace colorcpp::core

// I/O operators for basic_rec2020
template <typename Model>
std::ostream& operator<<(std::ostream& os, const colorcpp::core::basic_rec2020<Model>& c) {
  return colorcpp::core::io::operator<<(os, static_cast<const colorcpp::core::basic_color<Model>&>(c));
}

template <typename Model>
std::istream& operator>>(std::istream& is, colorcpp::core::basic_rec2020<Model>& c) {
  return colorcpp::core::io::operator>>(is, static_cast<colorcpp::core::basic_color<Model>&>(c));
}
```

- [ ] **Step 2: Add `#include <colorcpp/core/rec2020.hpp>` to the core aggregate header**

Edit `include/colorcpp/core/core.hpp` — add the include in alphabetical position (between `prophoto_rgb.hpp` and `rgb.hpp`):

```cpp
#include <colorcpp/core/rec2020.hpp>
```

(Insert after the line `#include <colorcpp/core/prophoto_rgb.hpp>`)

- [ ] **Step 3: Write construction/boundary tests**

Create `tests/core/test_rec2020.cpp` with construction, boundary, and type trait tests (no conversion tests yet — those come after conversion functions exist):

```cpp
/**
 * @file test_rec2020.cpp
 * @brief Tests for Rec.2020 (BT.2020) color model and conversion.
 *
 * Rec.2020 uses BT.709-style piecewise transfer function and D65 white point.
 */

#include <gtest/gtest.h>

#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/io/css.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::core::test {

using namespace colorcpp::operations::conversion;

// --- Construction ---

TEST(Rec2020Test, DefaultConstruction) {
  rec2020_rgbf_t c;
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(Rec2020Test, ParameterizedConstruction) {
  rec2020_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(Rec2020ATest, ParameterizedWithAlpha) {
  rec2020_rgbaf_t c(0.2f, 0.4f, 0.6f, 0.8f);
  EXPECT_FLOAT_EQ(c.r(), 0.2f);
  EXPECT_FLOAT_EQ(c.g(), 0.4f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
  EXPECT_FLOAT_EQ(c.a(), 0.8f);
}

// --- Boundary values ---

TEST(Rec2020Test, BoundaryValuesMin) {
  rec2020_rgbf_t c(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(c.r(), 0.0f);
  EXPECT_FLOAT_EQ(c.g(), 0.0f);
  EXPECT_FLOAT_EQ(c.b(), 0.0f);
}

TEST(Rec2020Test, BoundaryValuesMax) {
  rec2020_rgbf_t c(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(c.r(), 1.0f);
  EXPECT_FLOAT_EQ(c.g(), 1.0f);
  EXPECT_FLOAT_EQ(c.b(), 1.0f);
}

TEST(Rec2020Test, OutOfRangeThrows) {
  EXPECT_THROW(rec2020_rgbf_t(-0.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(1.1f, 0.5f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, -0.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 1.1f, 0.5f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 0.5f, -0.1f), std::out_of_range);
  EXPECT_THROW(rec2020_rgbf_t(0.5f, 0.5f, 1.1f), std::out_of_range);
}

// --- Member and indexed access ---

TEST(Rec2020Test, MutableMemberAccess) {
  rec2020_rgbf_t c(0.1f, 0.2f, 0.3f);
  c.r() = 0.4f;
  c.g() = 0.5f;
  c.b() = 0.6f;
  EXPECT_FLOAT_EQ(c.r(), 0.4f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.6f);
}

TEST(Rec2020Test, IndexAccess) {
  rec2020_rgbf_t c(0.2f, 0.4f, 0.6f);
  EXPECT_FLOAT_EQ(c.get_index<0>(), 0.2f);
  EXPECT_FLOAT_EQ(c.get_index<1>(), 0.4f);
  EXPECT_FLOAT_EQ(c.get_index<2>(), 0.6f);
}

TEST(Rec2020Test, ConstCorrectness) {
  const rec2020_rgbf_t c(0.3f, 0.5f, 0.7f);
  EXPECT_FLOAT_EQ(c.r(), 0.3f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.7f);
}

// --- Copy and move semantics ---

TEST(Rec2020Test, CopyConstruction) {
  rec2020_rgbf_t original(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t copy(original);
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(Rec2020Test, CopyAssignment) {
  rec2020_rgbf_t original(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t copy;
  copy = original;
  EXPECT_FLOAT_EQ(copy.r(), 0.2f);
  EXPECT_FLOAT_EQ(copy.g(), 0.4f);
  EXPECT_FLOAT_EQ(copy.b(), 0.6f);
}

TEST(Rec2020Test, Equality) {
  rec2020_rgbf_t a(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t b(0.2f, 0.4f, 0.6f);
  rec2020_rgbf_t c(0.3f, 0.4f, 0.6f);
  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

// --- Linear variants ---

TEST(LinearRec2020Test, Construction) {
  linear_rec2020_rgbf_t c(0.1f, 0.5f, 0.9f);
  EXPECT_FLOAT_EQ(c.r(), 0.1f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.9f);
}

TEST(LinearRec2020ATest, ConstructionWithAlpha) {
  linear_rec2020_rgbaf_t c(0.1f, 0.5f, 0.9f, 0.3f);
  EXPECT_FLOAT_EQ(c.r(), 0.1f);
  EXPECT_FLOAT_EQ(c.g(), 0.5f);
  EXPECT_FLOAT_EQ(c.b(), 0.9f);
  EXPECT_FLOAT_EQ(c.a(), 0.3f);
}

}  // namespace colorcpp::core::test
```

- [ ] **Step 4: Build and run core type tests**

```bash
cd build && cmake .. -DCOLORCPP_BUILD_TESTS=ON && cmake --build . --target test_rec2020 -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_rec2020 --output-on-failure
```

Expected: 15 tests pass (construction, boundary, access, copy, linear variants).

- [ ] **Step 5: Commit**

```bash
git add include/colorcpp/core/rec2020.hpp include/colorcpp/core/core.hpp tests/core/test_rec2020.cpp
git commit -m "feat(core): add Rec.2020 (BT.2020) core color type

Add rec2020_rgbf_t, rec2020_rgbaf_t, linear_rec2020_rgbf_t, and
linear_rec2020_rgbaf_t types following the Adobe RGB / Display P3 pattern.
Includes construction and boundary tests.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 2: Rec.2020 Conversion Functions

**Files:**
- Create: `include/colorcpp/operations/conversion/functions/rec2020.hpp`
- Modify: `tests/core/test_rec2020.cpp` (add conversion tests)

**Interfaces:**
- Consumes: `rec2020_rgbf_t`, `linear_rec2020_rgbf_t` etc. from Task 1
- Produces: `details::rec2020_to_linear_rec2020<To, From>()`, `details::linear_rec2020_to_rec2020<To, From>()`, `details::linear_rec2020_to_xyz<To, From>()`, `details::xyz_to_linear_rec2020<To, From>()`, `details::rec2020_to_srgb<To, From>()`, `details::srgb_to_rec2020<To, From>()`

- [ ] **Step 1: Write conversion tests (TDD — will fail to compile)**

Append to `tests/core/test_rec2020.cpp`:

```cpp
// --- Conversion tests ---

TEST(Rec2020ConversionTest, BlackConversion) {
  auto r2020 = color_cast<rec2020_rgbf_t>(rgbf_t{0.0f, 0.0f, 0.0f});
  EXPECT_NEAR(r2020.r(), 0.0f, 1e-4f);
  EXPECT_NEAR(r2020.g(), 0.0f, 1e-4f);
  EXPECT_NEAR(r2020.b(), 0.0f, 1e-4f);
}

TEST(Rec2020ConversionTest, WhiteConversion) {
  auto r2020 = color_cast<rec2020_rgbf_t>(rgbf_t{1.0f, 1.0f, 1.0f});
  EXPECT_NEAR(r2020.r(), 1.0f, 1e-4f);
  EXPECT_NEAR(r2020.g(), 1.0f, 1e-4f);
  EXPECT_NEAR(r2020.b(), 1.0f, 1e-4f);
}

TEST(Rec2020ConversionTest, AlphaPreserved) {
  rec2020_rgbaf_t original(0.5f, 0.3f, 0.7f, 0.6f);
  auto rgb = color_cast<rgbaf_t>(original);
  EXPECT_NEAR(rgb.a(), 0.6f, 1e-4f);
}

TEST(Rec2020ConversionTest, FloatRoundTrip) {
  rec2020_rgbf_t orig(0.6f, 0.2f, 0.8f);
  auto back = color_cast<rec2020_rgbf_t>(color_cast<rgbf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(Rec2020ConversionTest, AlphaRoundTrip) {
  rec2020_rgbaf_t orig(0.3f, 0.5f, 0.7f, 0.4f);
  auto back = color_cast<rec2020_rgbaf_t>(color_cast<rgbaf_t>(orig));
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
  EXPECT_NEAR(back.a(), orig.a(), 1e-4f);
}

// Linear conversion
TEST(Rec2020LinearTest, GammaDirection) {
  rec2020_rgbf_t gamma(0.5f, 0.5f, 0.5f);
  auto linear = color_cast<linear_rec2020_rgbf_t>(gamma);
  // Non-linear gamma — linear values are lower for mid-tones
  EXPECT_LT(linear.r(), 0.5f);
  EXPECT_LT(linear.g(), 0.5f);
  EXPECT_LT(linear.b(), 0.5f);
}

TEST(Rec2020LinearTest, LinearRoundTrip) {
  rec2020_rgbf_t orig(0.3f, 0.6f, 0.9f);
  auto linear = color_cast<linear_rec2020_rgbf_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(linear);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

// Cross-space conversions
TEST(Rec2020ConversionTest, DisplayP3CrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto dp3 = color_cast<display_p3f_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(dp3);
  EXPECT_NEAR(back.r(), orig.r(), 1e-2f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-2f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-2f);
}

TEST(Rec2020ConversionTest, AdobeRGBCrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto argb = color_cast<adobe_rgbf_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(argb);
  EXPECT_NEAR(back.r(), orig.r(), 1e-2f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-2f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-2f);
}

TEST(Rec2020ConversionTest, XYZCrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto xyz = color_cast<xyz_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(xyz);
  EXPECT_NEAR(back.r(), orig.r(), 1e-4f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-4f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-4f);
}

TEST(Rec2020ConversionTest, OKLabCrossConvert) {
  rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto oklab = color_cast<oklab_t>(orig);
  auto back = color_cast<rec2020_rgbf_t>(oklab);
  EXPECT_NEAR(back.r(), orig.r(), 1e-2f);
  EXPECT_NEAR(back.g(), orig.g(), 1e-2f);
  EXPECT_NEAR(back.b(), orig.b(), 1e-2f);
}

// CSS parsing
TEST(Rec2020CSSTest, ParseColorFunction) {
  auto c = parse_css_color<rec2020_rgbaf_t>("color(rec2020 0.5 0.3 0.7 / 0.8)");
  ASSERT_TRUE(c.has_value());
  EXPECT_NEAR(c->r(), 0.5f, 1e-4f);
  EXPECT_NEAR(c->g(), 0.3f, 1e-4f);
  EXPECT_NEAR(c->b(), 0.7f, 1e-4f);
  EXPECT_NEAR(c->a(), 0.8f, 1e-4f);
}

TEST(Rec2020CSSTest, ParseColorFunctionRoundTrip) {
  auto c = parse_css_color<rec2020_rgbaf_t>("color(rec2020 0.64 0.33 0.21)");
  ASSERT_TRUE(c.has_value());
  auto back = parse_css_color<rec2020_rgbaf_t>(to_css_color_string(*c));
  ASSERT_TRUE(back.has_value());
  EXPECT_NEAR(back->r(), c->r(), 1e-4f);
  EXPECT_NEAR(back->g(), c->g(), 1e-4f);
  EXPECT_NEAR(back->b(), c->b(), 1e-4f);
}
```

- [ ] **Step 2: Create the conversion functions header**

Write `include/colorcpp/operations/conversion/functions/rec2020.hpp`:

```cpp
/**
 * @file rec2020.hpp
 * @brief Rec.2020 (BT.2020) conversion functions.
 *
 * Rec.2020 uses BT.709-style piecewise transfer function.
 * White point: D65 (same as sRGB/XYZ hub, no chromatic adaptation needed).
 *
 * Transfer function parameters:
 *   α = 1.09929682680944
 *   β = 0.018053968510807
 *   Linear segment threshold (encoded) = β × 4.5 ≈ 0.081242858
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/rec2020.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion/detail.hpp>

namespace colorcpp::operations::conversion::details {

/**
 * @brief Rec.2020 transfer function constants (BT.709-style piecewise).
 */
namespace rec2020_tf {

constexpr float alpha = 1.09929682680944f;
constexpr float beta  = 0.018053968510807f;
constexpr float beta_times_4_5 = beta * 4.5f;   // ≈ 0.081242858
constexpr float exponent = 0.45f;                 // 1/2.222...
constexpr float inv_exponent = 1.0f / 0.45f;      // ≈ 2.222...

/**
 * @brief Linearize a Rec.2020 encoded channel value.
 */
constexpr float to_linear(float v) noexcept {
  if (v < beta_times_4_5)
    return v / 4.5f;
  else
    return std::pow((v + alpha - 1.0f) / alpha, inv_exponent);
}

/**
 * @brief Encode a linear channel value to Rec.2020 gamma.
 */
constexpr float from_linear(float v) noexcept {
  if (v < beta)
    return v * 4.5f;
  else
    return alpha * std::pow(v, exponent) - (alpha - 1.0f);
}

}  // namespace rec2020_tf

/**
 * @brief Convert Rec.2020 gamma-encoded to Linear Rec.2020.
 */
template <typename To, typename From>
constexpr To rec2020_to_linear_rec2020(const From& src) {
  float r = rec2020_tf::to_linear(to_unit<From, 0>(src.template get_index<0>()));
  float g = rec2020_tf::to_linear(to_unit<From, 1>(src.template get_index<1>()));
  float b = rec2020_tf::to_linear(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear Rec.2020 to gamma-encoded Rec.2020.
 */
template <typename To, typename From>
constexpr To linear_rec2020_to_rec2020(const From& src) {
  auto encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return rec2020_tf::from_linear(v);
  };
  float r = encode(to_unit<From, 0>(src.template get_index<0>()));
  float g = encode(to_unit<From, 1>(src.template get_index<1>()));
  float b = encode(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert Linear Rec.2020 to CIE XYZ (D65).
 *
 * Standard BT.2020 primaries matrix (ITU-R BT.2020-2, D65 white point).
 */
template <typename To, typename From>
constexpr To linear_rec2020_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  float x = 0.6369580483012914f * r + 0.14461690358620838f * g + 0.16888097516417213f * b;
  float y = 0.2627002120112671f * r  + 0.6779980715188708f * g  + 0.05930171646986196f * b;
  float z = 0.0000000000000000f * r  + 0.028072693049087428f * g + 1.0609850577107909f * b;

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(x), from_unit<To, 1>(y), from_unit<To, 2>(z), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(x), from_unit<To, 1>(y), from_unit<To, 2>(z));
  }
}

/**
 * @brief Convert CIE XYZ (D65) to Linear Rec.2020.
 *
 * Inverse BT.2020 matrix (ITU-R BT.2020-2).
 */
template <typename To, typename From>
constexpr To xyz_to_linear_rec2020(const From& src) {
  float x = to_unit<From, 0>(src.template get_index<0>());
  float y = to_unit<From, 1>(src.template get_index<1>());
  float z = to_unit<From, 2>(src.template get_index<2>());

  float r =  1.7166511879712679f  * x - 0.35567078377639240f * y - 0.25336628137365992f * z;
  float g = -0.6666843518324889f  * x + 1.6164812366349388f  * y + 0.015768545813911114f * z;
  float b =  0.01763985744531078f * x - 0.04277061325780865f * y + 0.942103121235474f    * z;

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
  }
}

/**
 * @brief Convert Rec.2020 directly to sRGB (4-hop shortcut).
 *
 * Optimized direct conversion: Rec.2020 linearize → matrix → sRGB gamma encode.
 * Avoids intermediate XYZ object allocation.
 */
template <typename To, typename From>
constexpr To rec2020_to_srgb(const From& src) {
  // Step 1: Linearize Rec.2020 gamma
  float r_lin = rec2020_tf::to_linear(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = rec2020_tf::to_linear(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = rec2020_tf::to_linear(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Step 2: Matrix: Linear Rec.2020 → Linear sRGB (computed as M_xyz_to_srgb × M_rec2020_to_xyz)
  float r_srgb_lin =  1.6605f * r_lin - 0.5876f * g_lin - 0.0728f * b_lin;
  float g_srgb_lin = -0.1246f * r_lin + 1.1329f * g_lin - 0.0083f * b_lin;
  float b_srgb_lin = -0.0182f * r_lin - 0.1006f * g_lin + 1.1187f * b_lin;

  // Step 3: Gamma encode for sRGB
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v <= 0.0031308f) ? (v * 12.92f) : (1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f);
  };
  float r = gamma_encode(r_srgb_lin);
  float g = gamma_encode(g_srgb_lin);
  float b = gamma_encode(b_srgb_lin);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

/**
 * @brief Convert sRGB directly to Rec.2020 (4-hop shortcut).
 *
 * Optimized direct conversion: sRGB linearize → inverse matrix → Rec.2020 gamma encode.
 */
template <typename To, typename From>
constexpr To srgb_to_rec2020(const From& src) {
  // Step 1: Linearize sRGB
  auto linearize = [](float v) noexcept {
    return (v <= 0.04045f) ? (v / 12.92f) : std::pow((v + 0.055f) / 1.055f, 2.4f);
  };
  float r_lin = linearize(to_unit<From, 0>(src.template get_index<0>()));
  float g_lin = linearize(to_unit<From, 1>(src.template get_index<1>()));
  float b_lin = linearize(to_unit<From, 2>(src.template get_index<2>()));
  float a = get_src_alpha(src);

  // Step 2: Matrix: Linear sRGB → Linear Rec.2020 (inverse of above)
  float r_r2020_lin =  0.6274f * r_lin + 0.3293f * g_lin + 0.0433f * b_lin;
  float g_r2020_lin =  0.0691f * r_lin + 0.9195f * g_lin + 0.0114f * b_lin;
  float b_r2020_lin =  0.0164f * r_lin + 0.0880f * g_lin + 0.8956f * b_lin;

  // Step 3: Gamma encode Rec.2020
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return rec2020_tf::from_linear(v);
  };
  float r = gamma_encode(r_r2020_lin);
  float g = gamma_encode(g_r2020_lin);
  float b = gamma_encode(b_r2020_lin);

  if constexpr (To::channels >= 4)
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  else
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
}

}  // namespace colorcpp::operations::conversion::details
```

- [ ] **Step 3: Build — verify compilation fails (no graph registration yet)**

```bash
cd build && cmake --build . --target test_rec2020 -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) 2>&1 | tail -5
```

Expected: compilation error — "no conversion path from rgbf_t to rec2020_rgbf_t" (registration not done yet).

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/operations/conversion/functions/rec2020.hpp tests/core/test_rec2020.cpp
git commit -m "feat(conversion): add Rec.2020 conversion functions (tests fail - unregistered)

Add gamma encode/decode, XYZ matrices, and sRGB shortcut. Tests added but
will fail to compile until graph edges are registered in next task.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 3: Register Rec.2020 in Conversion Graph

**Files:**
- Modify: `include/colorcpp/operations/conversion/functions/index.hpp`
- Modify: `include/colorcpp/operations/conversion/color_space_registry.hpp`

**Interfaces:**
- Consumes: conversion functions from Task 2
- Produces: registered edges so `color_cast<rec2020_rgbf_t>(rgbaf_t{...})` compiles and works

- [ ] **Step 1: Add Rec.2020 include and adobe_rgb/prophoto_rgb includes to index.hpp**

Edit `include/colorcpp/operations/conversion/functions/index.hpp` — add the function file include (alphabetically between `prophoto_rgb.hpp` and `rgbf_oklab_reg.hpp`):

```cpp
#include <colorcpp/operations/conversion/functions/rec2020.hpp>
```

And add the core type includes for Adobe RGB and ProPhoto RGB (needed for their registrations which are currently missing from this file):

After `#include <colorcpp/core/oklab.hpp>` add:
```cpp
#include <colorcpp/core/adobe_rgb.hpp>
#include <colorcpp/core/prophoto_rgb.hpp>
```

- [ ] **Step 2: Register Rec.2020 edges in index.hpp**

Append inside the `namespace colorcpp::operations::conversion {` block, before the closing `}`:

```cpp
// Adobe RGB ↔ Linear Adobe RGB
COLORCPP_REGISTER_CONVERSION_BIDIR(core::adobe_rgbf_t, core::linear_adobe_rgbf_t,
                                   details::adobe_rgb_to_linear_adobe_rgb<core::linear_adobe_rgbf_t>,
                                   details::linear_adobe_rgb_to_adobe_rgb<core::adobe_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::adobe_rgbaf_t, core::linear_adobe_rgbaf_t,
                                   details::adobe_rgb_to_linear_adobe_rgb<core::linear_adobe_rgbaf_t>,
                                   details::linear_adobe_rgb_to_adobe_rgb<core::adobe_rgbaf_t>)

// Linear Adobe RGB ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_adobe_rgbf_t, core::xyz_t,
                                   details::linear_adobe_rgb_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_adobe_rgb<core::linear_adobe_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_adobe_rgbaf_t, core::xyz_t,
                                   details::linear_adobe_rgb_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_adobe_rgb<core::linear_adobe_rgbaf_t>)

// Adobe RGB ↔ sRGB (direct short link)
COLORCPP_REGISTER_CONVERSION_BIDIR_WEIGHTED(core::adobe_rgbf_t, core::rgbf_t,
                                            details::adobe_rgb_to_srgb<core::rgbf_t>,
                                            details::srgb_to_adobe_rgb<core::adobe_rgbf_t>,
                                            route_cost::shortcut_4_hop, route_cost::shortcut_4_hop)

COLORCPP_REGISTER_CONVERSION_BIDIR_WEIGHTED(core::adobe_rgbaf_t, core::rgbaf_t,
                                            details::adobe_rgb_to_srgb<core::rgbaf_t>,
                                            details::srgb_to_adobe_rgb<core::adobe_rgbaf_t>,
                                            route_cost::shortcut_4_hop, route_cost::shortcut_4_hop)

// ProPhoto RGB ↔ Linear ProPhoto RGB
COLORCPP_REGISTER_CONVERSION_BIDIR(core::prophoto_rgbf_t, core::linear_prophoto_rgbf_t,
                                   details::prophoto_rgb_to_linear_prophoto_rgb<core::linear_prophoto_rgbf_t>,
                                   details::linear_prophoto_rgb_to_prophoto_rgb<core::prophoto_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::prophoto_rgbaf_t, core::linear_prophoto_rgbaf_t,
                                   details::prophoto_rgb_to_linear_prophoto_rgb<core::linear_prophoto_rgbaf_t>,
                                   details::linear_prophoto_rgb_to_prophoto_rgb<core::prophoto_rgbaf_t>)

// Linear ProPhoto RGB ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_prophoto_rgbf_t, core::xyz_t,
                                   details::linear_prophoto_rgb_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_prophoto_rgb<core::linear_prophoto_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_prophoto_rgbaf_t, core::xyz_t,
                                   details::linear_prophoto_rgb_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_prophoto_rgb<core::linear_prophoto_rgbaf_t>)

// Rec.2020 ↔ Linear Rec.2020
COLORCPP_REGISTER_CONVERSION_BIDIR(core::rec2020_rgbf_t, core::linear_rec2020_rgbf_t,
                                   details::rec2020_to_linear_rec2020<core::linear_rec2020_rgbf_t>,
                                   details::linear_rec2020_to_rec2020<core::rec2020_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::rec2020_rgbaf_t, core::linear_rec2020_rgbaf_t,
                                   details::rec2020_to_linear_rec2020<core::linear_rec2020_rgbaf_t>,
                                   details::linear_rec2020_to_rec2020<core::rec2020_rgbaf_t>)

// Linear Rec.2020 ↔ XYZ
COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rec2020_rgbf_t, core::xyz_t,
                                   details::linear_rec2020_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_rec2020<core::linear_rec2020_rgbf_t>)

COLORCPP_REGISTER_CONVERSION_BIDIR(core::linear_rec2020_rgbaf_t, core::xyz_t,
                                   details::linear_rec2020_to_xyz<core::xyz_t>,
                                   details::xyz_to_linear_rec2020<core::linear_rec2020_rgbaf_t>)

// Rec.2020 ↔ sRGB (direct short link)
COLORCPP_REGISTER_CONVERSION_BIDIR_WEIGHTED(core::rec2020_rgbf_t, core::rgbf_t,
                                            details::rec2020_to_srgb<core::rgbf_t>,
                                            details::srgb_to_rec2020<core::rec2020_rgbf_t>,
                                            route_cost::shortcut_4_hop, route_cost::shortcut_4_hop)

COLORCPP_REGISTER_CONVERSION_BIDIR_WEIGHTED(core::rec2020_rgbaf_t, core::rgbaf_t,
                                            details::rec2020_to_srgb<core::rgbaf_t>,
                                            details::srgb_to_rec2020<core::rec2020_rgbaf_t>,
                                            route_cost::shortcut_4_hop, route_cost::shortcut_4_hop)
```

Note: The Adobe RGB and ProPhoto RGB registrations are missing from the current `index.hpp` (they exist only in the `feat/wide-color-gamut` branch's version). After merging `main`, they should already be present — verify with `grep "adobe_rgb\|prophoto" include/colorcpp/operations/conversion/functions/index.hpp`. If present, skip those registrations.

- [ ] **Step 3: Add Rec.2020 color_traits to color_space_registry.hpp**

Edit `include/colorcpp/operations/conversion/color_space_registry.hpp` — add after the ProPhoto RGB section:

```cpp
// Rec.2020 uses Linear Rec.2020 as hub
template <>
struct color_traits<core::rec2020::model::rec2020> {
  using hub_type = core::linear_rec2020_rgbf_t;
};

template <>
struct color_traits<core::rec2020::model::rec2020a> {
  using hub_type = core::linear_rec2020_rgbaf_t;
};

// Linear Rec.2020 uses XYZ as hub
template <>
struct color_traits<core::rec2020::model::linear_rec2020f> {
  using hub_type = core::xyz_t;
};

template <>
struct color_traits<core::rec2020::model::linear_rec2020af> {
  using hub_type = core::xyz_t;
};
```

Also add `#include <colorcpp/core/rec2020.hpp>` at the top of the file with the other core includes.

- [ ] **Step 4: Build and run all Rec.2020 tests**

```bash
cd build && cmake --build . --target test_rec2020 -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_rec2020 --output-on-failure
```

Expected: All 28+ tests pass, including conversion and CSS tests.

- [ ] **Step 5: Commit**

```bash
git add include/colorcpp/operations/conversion/functions/index.hpp include/colorcpp/operations/conversion/color_space_registry.hpp
git commit -m "feat(conversion): register Rec.2020 edges in conversion graph

Register Rec.2020, Adobe RGB, and ProPhoto RGB edges in index.hpp.
Add Rec.2020 color_traits to color_space_registry.hpp.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 4: Chromatic Adaptation Module

**Files:**
- Create: `include/colorcpp/algorithms/chromatic_adaptation.hpp`
- Modify: `include/colorcpp/algorithms/algorithms.hpp`
- Create: `tests/algorithms/test_chromatic_adaptation.cpp`

**Interfaces:**
- Produces: `colorcpp::algorithms::chromatic_adaptation::bradford_adapt<ColorT>(color, src_white, dst_white) -> ColorT`
- Produces: `colorcpp::algorithms::chromatic_adaptation::von_kries_adapt<ColorT>(color, src_white, dst_white) -> ColorT`
- Produces: `WHITEPOINT_D65`, `WHITEPOINT_D50`, `WHITEPOINT_D55`, `WHITEPOINT_A`, `WHITEPOINT_C` — `constexpr xyz_t`
- Produces: Von Kries LMS matrices `M_VK`, `M_VK_inv` (internal constants)

- [ ] **Step 1: Write the chromatic adaptation tests**

Create `tests/algorithms/test_chromatic_adaptation.cpp`:

```cpp
/**
 * @file test_chromatic_adaptation.cpp
 * @brief Tests for chromatic adaptation (Bradford and Von Kries).
 */

#include <gtest/gtest.h>

#include <colorcpp/algorithms/chromatic_adaptation.hpp>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::test {

using namespace chromatic_adaptation;
using namespace operations::conversion;

// --- White point constants ---

TEST(WhitePointTest, D65Values) {
  EXPECT_NEAR(WHITEPOINT_D65.x(), 0.95047f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D65.y(), 1.00000f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D65.z(), 1.08883f, 1e-5f);
}

TEST(WhitePointTest, D50Values) {
  EXPECT_NEAR(WHITEPOINT_D50.x(), 0.96422f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D50.y(), 1.00000f, 1e-5f);
  EXPECT_NEAR(WHITEPOINT_D50.z(), 0.82521f, 1e-5f);
}

// --- Bradford adaptation ---

TEST(BradfordTest, SameWhitePointIsIdentity) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto result = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(BradfordTest, RoundTripD65D50D65) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto d50 = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  auto back = bradford_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-4f);
  EXPECT_NEAR(back.y(), color.y(), 1e-4f);
  EXPECT_NEAR(back.z(), color.z(), 1e-4f);
}

TEST(BradfordTest, D65ToD50KnownReference) {
  // Bruce Lindbloom reference: D65 white (0.95047, 1.0, 1.08883) → D50
  auto result = bradford_adapt(WHITEPOINT_D65, WHITEPOINT_D65, WHITEPOINT_D50);
  EXPECT_NEAR(result.x(), 0.96422f, 1e-4f);
  EXPECT_NEAR(result.y(), 1.00000f, 1e-4f);
  EXPECT_NEAR(result.z(), 0.82521f, 1e-4f);
}

TEST(BradfordTest, NonNegativeOutput) {
  xyz_t color(0.1f, 0.8f, 0.5f);
  auto result = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_A);
  EXPECT_GE(result.x(), -1e-5f);
  EXPECT_GE(result.y(), -1e-5f);
  EXPECT_GE(result.z(), -1e-5f);
}

TEST(BradfordTest, WorksWithRgbTypes) {
  rgbf_t color(0.5f, 0.3f, 0.7f);
  auto result = bradford_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  // Result should be a valid rgbf_t (within [0,1])
  EXPECT_GE(result.r(), 0.0f);
  EXPECT_LE(result.r(), 1.0f);
  EXPECT_GE(result.g(), 0.0f);
  EXPECT_LE(result.g(), 1.0f);
  EXPECT_GE(result.b(), 0.0f);
  EXPECT_LE(result.b(), 1.0f);
}

// --- Von Kries adaptation ---

TEST(VonKriesTest, SameWhitePointIsIdentity) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto result = von_kries_adapt(color, WHITEPOINT_D65, WHITEPOINT_D65);
  EXPECT_NEAR(result.x(), 0.5f, 1e-5f);
  EXPECT_NEAR(result.y(), 0.4f, 1e-5f);
  EXPECT_NEAR(result.z(), 0.3f, 1e-5f);
}

TEST(VonKriesTest, RoundTripD65D50D65) {
  xyz_t color(0.5f, 0.4f, 0.3f);
  auto d50 = von_kries_adapt(color, WHITEPOINT_D65, WHITEPOINT_D50);
  auto back = von_kries_adapt(d50, WHITEPOINT_D50, WHITEPOINT_D65);
  EXPECT_NEAR(back.x(), color.x(), 1e-3f);
  EXPECT_NEAR(back.y(), color.y(), 1e-3f);
  EXPECT_NEAR(back.z(), color.z(), 1e-3f);
}

}  // namespace colorcpp::algorithms::test
```

- [ ] **Step 2: Create the chromatic adaptation module**

Write `include/colorcpp/algorithms/chromatic_adaptation.hpp`:

```cpp
/**
 * @file chromatic_adaptation.hpp
 * @brief Chromatic adaptation transforms (Bradford, Von Kries) and standard white point constants.
 *
 * Chromatic adaptation converts colors from one white point (illuminant) to another.
 * This is essential when working with color spaces that use different reference whites
 * (e.g., ProPhoto RGB uses D50, most others use D65).
 *
 * Bradford is the recommended method for most use cases (used in ICC profiles).
 * Von Kries is simpler but less perceptually accurate.
 *
 * @see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/xyz.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::chromatic_adaptation {

// =============================================================================
// Standard White Points (CIE 1931 XYZ tristimulus, Y = 1.0)
// =============================================================================

/// D65 — CIE Standard Illuminant (noon daylight, correlated color temperature ~6504 K)
inline constexpr core::xyz_t WHITEPOINT_D65{0.95047f, 1.00000f, 1.08883f};

/// D50 — CIE Standard Illuminant (horizon daylight, CCT ~5003 K)
inline constexpr core::xyz_t WHITEPOINT_D50{0.96422f, 1.00000f, 0.82521f};

/// D55 — CIE Standard Illuminant (mid-morning/mid-afternoon daylight, CCT ~5503 K)
inline constexpr core::xyz_t WHITEPOINT_D55{0.95682f, 1.00000f, 0.92149f};

/// A — CIE Standard Illuminant (tungsten filament, CCT ~2856 K)
inline constexpr core::xyz_t WHITEPOINT_A{1.09850f, 1.00000f, 0.35585f};

/// C — CIE Standard Illuminant (filtered tungsten / average daylight, CCT ~6774 K)
inline constexpr core::xyz_t WHITEPOINT_C{0.98074f, 1.00000f, 1.18232f};

// =============================================================================
// Von Kries Chromatic Adaptation (diagonal scaling in LMS space)
// =============================================================================

namespace details {

// Von Kries LMS matrix (Hunt-Pointer-Estevez, normalized to D65)
inline constexpr float M_VK[9] = {
    0.40024f,  0.70760f, -0.08081f,
   -0.22630f,  1.16532f,  0.04570f,
    0.00000f,  0.00000f,  0.91822f,
};

// Inverse Von Kries LMS matrix
inline constexpr float M_VK_inv[9] = {
    1.8599364f, -1.1293816f,  0.2198974f,
    0.3611914f,  0.6388125f, -0.0000064f,
    0.0000000f,  0.0000000f,  1.0890636f,
};

}  // namespace details

/**
 * @brief Von Kries chromatic adaptation transform.
 *
 * Converts a color from @p src_white to @p dst_white using diagonal scaling
 * in Hunt-Pointer-Estevez LMS cone response space. Simpler and faster than
 * Bradford, but less perceptually uniform for large white point shifts.
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT von_kries_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  using namespace operations::conversion;

  auto src = color_cast<core::xyz_t>(color);

  // Compute LMS of source and destination whites
  float lms_src[3], lms_dst[3];
  for (int i = 0; i < 3; ++i) {
    lms_src[i] = details::M_VK[i * 3 + 0] * (&src_white.x())[0] +
                 details::M_VK[i * 3 + 1] * (&src_white.y())[0] +
                 details::M_VK[i * 3 + 2] * (&src_white.z())[0];
    lms_dst[i] = details::M_VK[i * 3 + 0] * (&dst_white.x())[0] +
                 details::M_VK[i * 3 + 1] * (&dst_white.y())[0] +
                 details::M_VK[i * 3 + 2] * (&dst_white.z())[0];
  }

  // Convert source color to LMS
  float lms[3];
  for (int i = 0; i < 3; ++i) {
    lms[i] = details::M_VK[i * 3 + 0] * src.x() +
             details::M_VK[i * 3 + 1] * src.y() +
             details::M_VK[i * 3 + 2] * src.z();
  }

  // Apply diagonal scaling
  for (int i = 0; i < 3; ++i) {
    if (lms_src[i] != 0.0f) {
      lms[i] *= lms_dst[i] / lms_src[i];
    }
  }

  // Convert back to XYZ
  float x = details::M_VK_inv[0] * lms[0] + details::M_VK_inv[1] * lms[1] + details::M_VK_inv[2] * lms[2];
  float y = details::M_VK_inv[3] * lms[0] + details::M_VK_inv[4] * lms[1] + details::M_VK_inv[5] * lms[2];
  float z = details::M_VK_inv[6] * lms[0] + details::M_VK_inv[7] * lms[1] + details::M_VK_inv[8] * lms[2];

  return color_cast<ColorT>(core::xyz_t{x, y, z});
}

// =============================================================================
// Bradford Chromatic Adaptation (recommended for most use cases)
// =============================================================================

namespace details {

// Bradford LMS matrix (from Bruce Lindbloom)
inline constexpr float M_BFD[9] = {
     0.8951000f,  0.2664000f, -0.1614000f,
    -0.7502000f,  1.7135000f,  0.0367000f,
     0.0389000f, -0.0685000f,  1.0296000f,
};

// Inverse Bradford LMS matrix
inline constexpr float M_BFD_inv[9] = {
     0.9869929f, -0.1470543f,  0.1599627f,
     0.4323053f,  0.5183603f,  0.0492912f,
    -0.0085287f,  0.0400428f,  0.9684867f,
};

}  // namespace details

/**
 * @brief Bradford chromatic adaptation transform.
 *
 * Converts a color from @p src_white to @p dst_white using the Bradford
 * LMS cone response matrix. This is the recommended method for most use
 * cases — it is used in ICC profiles and provides perceptually accurate
 * results across a wide range of white point shifts.
 *
 * @tparam ColorT Any color type supported by conversion::color_cast to/from xyz_t.
 * @param color Input color under src_white illuminant.
 * @param src_white XYZ tristimulus of the source white point.
 * @param dst_white XYZ tristimulus of the destination white point.
 * @return Color adapted to dst_white illuminant.
 */
template <typename ColorT>
ColorT bradford_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white) {
  using namespace operations::conversion;

  auto src = color_cast<core::xyz_t>(color);

  // Compute LMS of source and destination whites
  float lms_src[3], lms_dst[3];
  for (int i = 0; i < 3; ++i) {
    lms_src[i] = details::M_BFD[i * 3 + 0] * (&src_white.x())[0] +
                 details::M_BFD[i * 3 + 1] * (&src_white.y())[0] +
                 details::M_BFD[i * 3 + 2] * (&src_white.z())[0];
    lms_dst[i] = details::M_BFD[i * 3 + 0] * (&dst_white.x())[0] +
                 details::M_BFD[i * 3 + 1] * (&dst_white.y())[0] +
                 details::M_BFD[i * 3 + 2] * (&dst_white.z())[0];
  }

  // Convert source color to LMS
  float lms[3];
  for (int i = 0; i < 3; ++i) {
    lms[i] = details::M_BFD[i * 3 + 0] * src.x() +
             details::M_BFD[i * 3 + 1] * src.y() +
             details::M_BFD[i * 3 + 2] * src.z();
  }

  // Apply diagonal scaling
  for (int i = 0; i < 3; ++i) {
    if (lms_src[i] != 0.0f) {
      lms[i] *= lms_dst[i] / lms_src[i];
    }
  }

  // Convert back to XYZ
  float x = details::M_BFD_inv[0] * lms[0] + details::M_BFD_inv[1] * lms[1] + details::M_BFD_inv[2] * lms[2];
  float y = details::M_BFD_inv[3] * lms[0] + details::M_BFD_inv[4] * lms[1] + details::M_BFD_inv[5] * lms[2];
  float z = details::M_BFD_inv[6] * lms[0] + details::M_BFD_inv[7] * lms[1] + details::M_BFD_inv[8] * lms[2];

  return color_cast<ColorT>(core::xyz_t{x, y, z});
}

}  // namespace colorcpp::algorithms::chromatic_adaptation
```

- [ ] **Step 3: Add include to algorithms aggregate header**

Edit `include/colorcpp/algorithms/algorithms.hpp` — add after the `#include <colorcpp/algorithms/accessibility.hpp>` line:

```cpp
#include <colorcpp/algorithms/chromatic_adaptation.hpp>
```

- [ ] **Step 4: Build and run chromatic adaptation tests**

```bash
cd build && cmake .. -DCOLORCPP_BUILD_TESTS=ON && cmake --build . --target test_chromatic_adaptation -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_chromatic_adaptation --output-on-failure
```

Expected: 8 tests pass.

- [ ] **Step 5: Commit**

```bash
git add include/colorcpp/algorithms/chromatic_adaptation.hpp include/colorcpp/algorithms/algorithms.hpp tests/algorithms/test_chromatic_adaptation.cpp
git commit -m "feat(algorithms): add chromatic adaptation module (Bradford, Von Kries)

Add standalone chromatic_adaptation module with Bradford and Von Kries
transforms, plus standard white point constants (D65, D50, D55, A, C).
Tests verify round-trip fidelity and known reference values.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 5: Refactor ProPhoto RGB Conversion

**Files:**
- Modify: `include/colorcpp/operations/conversion/functions/prophoto_rgb.hpp`

**Interfaces:**
- Consumes: `bradford_adapt()` from Task 4
- Produces: Same external interface (graph edges unchanged), but internal implementation uses explicit Bradford adaptation instead of combined matrices

- [ ] **Step 1: Verify current ProPhoto tests pass**

```bash
cd build && cmake --build . --target test_prophoto_rgb -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_prophoto_rgb --output-on-failure
```

Expected: All ProPhoto tests pass (baseline).

- [ ] **Step 2: Replace combined matrices with stepwise conversion**

Edit `include/colorcpp/operations/conversion/functions/prophoto_rgb.hpp` — replace the `linear_prophoto_rgb_to_xyz` and `xyz_to_linear_prophoto_rgb` functions:

Replace lines 106-166 (the two combined-matrix functions) with:

```cpp
/**
 * @brief Convert Linear ProPhoto RGB (D50) to CIE XYZ (D65).
 *
 * This is a two-step conversion:
 *   1. Linear ProPhoto RGB → XYZ(D50) via ProPhoto primaries matrix
 *   2. XYZ(D50) → XYZ(D65) via Bradford chromatic adaptation
 *
 * Reference:
 *   http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
 */
template <typename To, typename From>
constexpr To linear_prophoto_rgb_to_xyz(const From& src) {
  float r = to_unit<From, 0>(src.template get_index<0>());
  float g = to_unit<From, 1>(src.template get_index<1>());
  float b = to_unit<From, 2>(src.template get_index<2>());

  // Step 1: Linear ProPhoto RGB → XYZ(D50) via pure primaries matrix
  // M_prophoto_to_xyz(D50) from Bruce Lindbloom
  float x_d50 = 0.7977605f * r + 0.1351858f * g + 0.0313493f * b;
  float y_d50 = 0.2880711f * r + 0.7118432f * g + 0.0000857f * b;
  float z_d50 = 0.0000000f * r + 0.0000000f * g + 0.8251046f * b;

  // Step 2: Bradford chromatic adaptation D50 → D65
  core::xyz_t xyz_d50{x_d50, y_d50, z_d50};
  auto xyz_d65 = algorithms::chromatic_adaptation::bradford_adapt(
      xyz_d50, algorithms::chromatic_adaptation::WHITEPOINT_D50,
      algorithms::chromatic_adaptation::WHITEPOINT_D65);

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(xyz_d65.x()), from_unit<To, 1>(xyz_d65.y()),
                       from_unit<To, 2>(xyz_d65.z()), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(xyz_d65.x()), from_unit<To, 1>(xyz_d65.y()),
                       from_unit<To, 2>(xyz_d65.z()));
  }
}

/**
 * @brief Convert CIE XYZ (D65) to Linear ProPhoto RGB (D50).
 *
 * This is a two-step conversion:
 *   1. XYZ(D65) → XYZ(D50) via inverse Bradford chromatic adaptation
 *   2. XYZ(D50) → Linear ProPhoto RGB via inverse ProPhoto primaries matrix
 */
template <typename To, typename From>
constexpr To xyz_to_linear_prophoto_rgb(const From& src) {
  float x = to_unit<From, 0>(src.template get_index<0>());
  float y = to_unit<From, 1>(src.template get_index<1>());
  float z = to_unit<From, 2>(src.template get_index<2>());

  // Step 1: Bradford chromatic adaptation D65 → D50
  core::xyz_t xyz_d65{x, y, z};
  auto xyz_d50 = algorithms::chromatic_adaptation::bradford_adapt(
      xyz_d65, algorithms::chromatic_adaptation::WHITEPOINT_D65,
      algorithms::chromatic_adaptation::WHITEPOINT_D50);

  // Step 2: XYZ(D50) → Linear ProPhoto RGB via inverse primaries matrix
  // M_xyz_to_prophoto(D50) from Bruce Lindbloom
  float r =  1.3459433f * xyz_d50.x() - 0.2556075f * xyz_d50.y() - 0.0511118f * xyz_d50.z();
  float g = -0.5445989f * xyz_d50.x() + 1.5081673f * xyz_d50.y() + 0.0205351f * xyz_d50.z();
  float b =  0.0000000f * xyz_d50.x() + 0.0000000f * xyz_d50.y() + 1.2119587f * xyz_d50.z();

  if constexpr (To::channels >= 4) {
    float a = get_src_alpha(src);
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b), from_unit<To, 3>(a));
  } else {
    return pack_to<To>(from_unit<To, 0>(r), from_unit<To, 1>(g), from_unit<To, 2>(b));
  }
}
```

Also add the include at the top of the file after `#include <colorcpp/core/prophoto_rgb.hpp>`:

```cpp
#include <colorcpp/algorithms/chromatic_adaptation.hpp>
```

- [ ] **Step 3: Run ProPhoto tests to verify round-trip still works**

```bash
cd build && cmake --build . --target test_prophoto_rgb -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_prophoto_rgb --output-on-failure
```

Expected: All ProPhoto tests pass with refactored conversion.

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/operations/conversion/functions/prophoto_rgb.hpp
git commit -m "refactor(conversion): use explicit Bradford adapt in ProPhoto RGB

Replace combined D50/D65 matrices with stepwise conversion:
pure ProPhoto primaries matrix + explicit bradford_adapt() call.
Improves transparency and reusability of the chromatic adaptation logic.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 6: ProPhoto RGB Gamut Mapping

**Files:**
- Modify: `include/colorcpp/algorithms/gamut/multi_colorspace.hpp`
- Modify: `include/colorcpp/algorithms/gamut/details.hpp`
- Modify: `tests/algorithms/test_gamut.cpp`

**Interfaces:**
- Produces: `gamut_type::prophoto_rgb`, `is_in_prophoto_gamut<ColorT>(c) -> bool`, `linrgb_in_prophoto_gamut(r,g,b) -> bool`

- [ ] **Step 1: Add ProPhoto gamma helpers to gamut/details.hpp**

Append to `include/colorcpp/algorithms/gamut/details.hpp` (after the existing `linrgb_to_adobergb` function):

```cpp
// ProPhoto RGB → linear RGB (D50 white point via conversion)
inline void prophoto_to_linrgb(float r, float g, float b, float& lr, float& lg, float& lb) noexcept {
  // ProPhoto uses gamma 1.8 with linear segment below 1/512
  auto gamma_decode = [](float v) noexcept {
    return (v < (1.0f / 32.0f)) ? (v / 16.0f) : std::pow(v, 1.8f);
  };
  lr = gamma_decode(r);
  lg = gamma_decode(g);
  lb = gamma_decode(b);
}

// linear RGB → ProPhoto RGB
inline void linrgb_to_prophoto(float lr, float lg, float lb, float& r, float& g, float& b) noexcept {
  auto gamma_encode = [](float v) noexcept {
    v = std::clamp(v, 0.0f, 1.0f);
    return (v < (1.0f / 512.0f)) ? (v * 16.0f) : std::pow(v, 1.0f / 1.8f);
  };
  r = gamma_encode(lr);
  g = gamma_encode(lg);
  b = gamma_encode(lb);
}
```

- [ ] **Step 2: Add prophoto_rgb to gamut_type and add helper functions**

Edit `include/colorcpp/algorithms/gamut/multi_colorspace.hpp`:

Change the enum:
```cpp
enum class gamut_type {
  srgb,         ///< Standard RGB (sRGB)
  display_p3,   ///< Display P3 (wide gamut)
  adobe_rgb,    ///< Adobe RGB (wide gamut)
  prophoto_rgb, ///< ProPhoto RGB (widest gamut)
};
```

Add after `linrgb_in_adobergb_gamut()`:
```cpp
// Check if linear RGB is in ProPhoto RGB gamut
inline bool linrgb_in_prophoto_gamut(float r, float g, float b, float eps = 1e-4f) noexcept {
  float pprgb_r, pprgb_g, pprgb_b;
  linrgb_to_prophoto(r, g, b, pprgb_r, pprgb_g, pprgb_b);
  return pprgb_r >= -eps && pprgb_r <= 1.0f + eps &&
         pprgb_g >= -eps && pprgb_g <= 1.0f + eps &&
         pprgb_b >= -eps && pprgb_b <= 1.0f + eps;
}
```

Add `case gamut_type::prophoto_rgb:` to the switch in `linrgb_in_gamut()`:
```cpp
    case gamut_type::prophoto_rgb:
      return linrgb_in_prophoto_gamut(r, g, b, eps);
```

Add after `is_in_adobergb_gamut()`:
```cpp
/**
 * @brief Check if a color is in ProPhoto RGB gamut.
 */
template <typename Color>
bool is_in_prophoto_gamut(const Color& c) {
  return is_in_gamut(c, gamut_type::prophoto_rgb);
}
```

- [ ] **Step 3: Add ProPhoto gamut tests**

Append to `tests/algorithms/test_gamut.cpp`:

```cpp
// --- ProPhoto RGB gamut ---

TEST(MultiColorspaceGamutTest, ProPhotoInGamut) {
  // Colors near sRGB primaries should all be within ProPhoto's massive gamut
  EXPECT_TRUE(is_in_gamut(rgbf_t{1.0f, 0.0f, 0.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{0.0f, 1.0f, 0.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{0.0f, 0.0f, 1.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{0.0f, 0.0f, 0.0f}, gamut_type::prophoto_rgb));
  EXPECT_TRUE(is_in_gamut(rgbf_t{1.0f, 1.0f, 1.0f}, gamut_type::prophoto_rgb));
}

TEST(MultiColorspaceGamutTest, ProPhotoConvenienceFunction) {
  EXPECT_TRUE(is_in_prophoto_gamut(rgbf_t{0.5f, 0.5f, 0.5f}));
}

TEST(MultiColorspaceGamutTest, GamutClipToProPhoto) {
  // Clipping a valid sRGB color to ProPhoto should return it (ProPhoto is larger)
  auto result = gamut_clip_to_gamut(rgbf_t{0.3f, 0.6f, 0.9f}, gamut_type::prophoto_rgb);
  EXPECT_NEAR(result.template get_index<0>(), 0.3f, 1e-2f);
  EXPECT_NEAR(result.template get_index<1>(), 0.6f, 1e-2f);
  EXPECT_NEAR(result.template get_index<2>(), 0.9f, 1e-2f);
}

TEST(MultiColorspaceGamutTest, ProPhotoClippedResultIsInGamut) {
  // Generate a highly saturated color, clip to ProPhoto, verify in-gamut
  auto saturated = color_cast<rgbf_t>(oklch_t{1.0f, 0.5f, 180.0f});
  auto clipped = gamut_clip_to_gamut(saturated, gamut_type::prophoto_rgb);
  EXPECT_TRUE(is_in_gamut(clipped, gamut_type::prophoto_rgb));
}
```

- [ ] **Step 4: Build and run gamut tests**

```bash
cd build && cmake --build . --target test_gamut -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_gamut --output-on-failure
```

Expected: All gamut tests pass (existing + 4 new ProPhoto tests).

- [ ] **Step 5: Commit**

```bash
git add include/colorcpp/algorithms/gamut/multi_colorspace.hpp include/colorcpp/algorithms/gamut/details.hpp tests/algorithms/test_gamut.cpp
git commit -m "feat(gamut): add ProPhoto RGB to multi-colorspace gamut mapping

Add prophoto_rgb to gamut_type enum with gamma helpers, gamut check,
and gamut clipping support. Tests verify in-gamut detection and clipping.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 7: CSS I/O Adaptation for Rec.2020

**Files:**
- Modify: `include/colorcpp/io/css/color_function.hpp`
- Modify: `include/colorcpp/io/css/relative_color.hpp`

- [ ] **Step 1: Refactor rec2020_to_rgbaf to use core type**

Edit `include/colorcpp/io/css/color_function.hpp` — replace the `rec2020_to_rgbaf` function (lines 139-142) to convert through the core type:

```cpp
inline core::rgbaf_t rec2020_to_rgbaf(float r, float g, float b, float a) {
  auto r2020 = core::rec2020_rgbaf_t{r, g, b, a};
  return color_cast<core::rgbaf_t>(r2020);
}
```

Add include at top: `#include <colorcpp/core/rec2020.hpp>`

- [ ] **Step 2: Add parse_css_color overload for rec2020_rgbaf_t**

Edit `include/colorcpp/io/css/relative_color.hpp` — the rec2020 handling in `color_to_xyz` already works via the generic `color()` path. Add a dedicated pseudo-parser:

After the existing Display P3 section pattern, add:

```cpp
// Rec.2020 specialization — parsed by generic color() infrastructure,
// but this overload enables parse_css_color<rec2020_rgbaf_t>().
template <>
inline std::optional<core::rec2020_rgbaf_t> parse_css_color<core::rec2020_rgbaf_t>(const std::string_view input) {
  auto parsed = parse_color_function(input);
  if (!parsed) return std::nullopt;
  // The generic parser returns rgbaf_t; convert via the graph
  return color_cast<core::rec2020_rgbaf_t>(*parsed);
}
```

(Note: if `parse_color_function` or equivalent doesn't exist, instead wire through the existing `parse_css_color<core::rgbaf_t>` and convert. Check the existing pattern in `css/display_p3.hpp` for the exact approach.)

- [ ] **Step 3: Verify CSS tests still pass**

```bash
cd build && cmake --build . --target test_css -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_css --output-on-failure
```

- [ ] **Step 4: Run Rec.2020 tests to verify CSS parsing works**

```bash
cd build && cmake --build . --target test_rec2020 -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_rec2020 --output-on-failure
```

- [ ] **Step 5: Commit**

```bash
git add include/colorcpp/io/css/color_function.hpp include/colorcpp/io/css/relative_color.hpp
git commit -m "feat(io): wire Rec.2020 CSS parsing to core type

Refactor rec2020_to_rgbaf to use the new rec2020_rgbaf_t core type.
Add parse_css_color overload for direct Rec.2020 parsing.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 8: Uncomment Display P3 Tests

**Files:**
- Modify: `tests/core/test_display_p3.cpp`

- [ ] **Step 1: Uncomment the 7 blocked conversion tests**

Edit `tests/core/test_display_p3.cpp` — remove the `/*` on line 243 and `*/` on line 304, and remove the "Temporarily disabled" comment on line 242.

- [ ] **Step 2: Build and run Display P3 tests**

```bash
cd build && cmake --build . --target test_display_p3 -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest -R test_display_p3 --output-on-failure
```

Expected: All Display P3 tests pass (type + conversion).

If any fail: debug the conversion implementation. The most likely issue is precision tolerance — if so, adjust `1e-4f` to `1e-3f` in the test expectations.

- [ ] **Step 3: Commit**

```bash
git add tests/core/test_display_p3.cpp
git commit -m "test(display-p3): uncomment and enable conversion tests

The 7 blocked tests (BlackConversion, WhiteConversion, AlphaPreserved,
FloatRoundTrip, AlphaRoundTrip, LinearConversion, LinearRoundTrip) were
prematurely disabled. Conversion code exists and is registered.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 9: Run Full Test Suite

**Files:** None (verification only)

- [ ] **Step 1: Build all tests and run full suite**

```bash
cd build && cmake .. -DCOLORCPP_BUILD_TESTS=ON && cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest --output-on-failure
```

Expected: All tests pass (existing + new). Zero regressions.

- [ ] **Step 2: Commit (if any fixes needed)**

Only commit if test failures required fixes.

---

### Task 10: Adobe RGB Example

**Files:**
- Create: `examples/adobe_rgb_example.cpp`

- [ ] **Step 1: Write the example**

Write `examples/adobe_rgb_example.cpp`:

```cpp
/**
 * @file adobe_rgb_example.cpp
 * @brief Demonstrates Adobe RGB (A98-RGB) wide-gamut color space usage.
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make adobe_rgb_example
 */

#include <iostream>
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

int main() {
  std::cout << "=== Adobe RGB (A98-RGB) Wide-Gamut Color Space ===\n\n";

  // 1. Construct Adobe RGB colors
  std::cout << "1. Construction\n";
  core::adobe_rgbf_t pure_red(1.0f, 0.0f, 0.0f);
  core::adobe_rgbf_t pure_green(0.0f, 1.0f, 0.0f);
  core::adobe_rgbf_t pure_blue(0.0f, 0.0f, 1.0f);
  std::cout << "   Red:   " << pure_red << "\n";
  std::cout << "   Green: " << pure_green << "\n";
  std::cout << "   Blue:  " << pure_blue << "\n\n";

  // 2. Convert sRGB → Adobe RGB → sRGB (show gamut differences)
  std::cout << "2. sRGB ↔ Adobe RGB round-trip\n";
  core::rgbf_t srgb_green(0.0f, 1.0f, 0.0f);
  auto adobe_green = conversion::color_cast<core::adobe_rgbf_t>(srgb_green);
  auto back_to_srgb = conversion::color_cast<core::rgbf_t>(adobe_green);
  std::cout << "   sRGB green:        " << srgb_green << "\n";
  std::cout << "   → Adobe RGB:       " << adobe_green << "\n";
  std::cout << "   → back to sRGB:    " << back_to_srgb << "\n\n";

  // 3. Gamma encoding: show linear values
  std::cout << "3. Gamma encoding (Adobe RGB → Linear Adobe RGB)\n";
  auto linear_adobe = conversion::color_cast<core::linear_adobe_rgbf_t>(adobe_green);
  std::cout << "   Adobe RGB (gamma): " << adobe_green << "\n";
  std::cout << "   Linear Adobe RGB:   " << linear_adobe << "\n";
  std::cout << "   (Pure gamma 2.2 — mid-tones are darker in linear)\n\n";

  // 4. Cross-space chain: Adobe RGB → Display P3 → Rec.2020 → OKLab
  std::cout << "4. Cross-space chain: Adobe RGB → Display P3 → Rec.2020 → OKLab\n";
  core::adobe_rgbf_t test_color(0.7f, 0.2f, 0.4f);
  auto dp3 = conversion::color_cast<core::display_p3f_t>(test_color);
  auto r2020 = conversion::color_cast<core::rec2020_rgbf_t>(dp3);
  auto oklab = conversion::color_cast<core::oklab_t>(r2020);
  std::cout << "   Input (Adobe RGB): " << test_color << "\n";
  std::cout << "   → Display P3:      " << dp3 << "\n";
  std::cout << "   → Rec.2020:        " << r2020 << "\n";
  std::cout << "   → OKLab:           " << oklab << "\n\n";

  // 5. CSS parsing
  std::cout << "5. CSS color(a98-rgb ...) parsing\n";
  auto css = io::css::parse_css_color<core::adobe_rgbaf_t>("color(a98-rgb 0.64 0.33 0.21)");
  if (css) {
    std::cout << "   Parsed: " << *css << "\n";
    std::cout << "   CSS string: " << io::css::to_css_color_string(*css) << "\n";
  }
  std::cout << "\n";

  // 6. ANSI swatch
  std::cout << "6. Color swatches\n";
  io::ansi::print_swatch("Adobe Red  ", conversion::color_cast<core::rgb8_t>(core::adobe_rgbf_t{0.64f, 0.1f, 0.1f}));
  io::ansi::print_swatch("Adobe Green", conversion::color_cast<core::rgb8_t>(core::adobe_rgbf_t{0.1f, 0.64f, 0.1f}));
  io::ansi::print_swatch("Adobe Blue ", conversion::color_cast<core::rgb8_t>(core::adobe_rgbf_t{0.1f, 0.1f, 0.64f}));
  std::cout << "\n";

  std::cout << "Done.\n";
  return 0;
}
```

- [ ] **Step 2: Build and run the example**

```bash
cd build && cmake .. -DCOLORCPP_BUILD_EXAMPLES=ON && cmake --build . --target adobe_rgb_example -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ./examples/adobe_rgb_example
```

Expected: Output shows color conversions with no crashes.

- [ ] **Step 3: Commit**

```bash
git add examples/adobe_rgb_example.cpp
git commit -m "docs(examples): add Adobe RGB usage example

Demonstrates construction, sRGB round-trip, gamma encoding,
cross-space chain, CSS parsing, and ANSI swatch output.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 11: ProPhoto RGB Example

**Files:**
- Create: `examples/prophoto_rgb_example.cpp`

- [ ] **Step 1: Write the example**

```cpp
/**
 * @file prophoto_rgb_example.cpp
 * @brief Demonstrates ProPhoto RGB (ROMM RGB) wide-gamut color space usage.
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make prophoto_rgb_example
 */

#include <iostream>
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

int main() {
  std::cout << "=== ProPhoto RGB (ROMM RGB) Wide-Gamut Color Space ===\n\n";

  // 1. Construct ProPhoto RGB colors
  std::cout << "1. Construction\n";
  core::prophoto_rgbf_t red(0.8f, 0.1f, 0.1f);
  core::prophoto_rgbf_t green(0.1f, 0.8f, 0.1f);
  core::prophoto_rgbf_t blue(0.1f, 0.1f, 0.8f);
  std::cout << "   Red:   " << red << "\n";
  std::cout << "   Green: " << green << "\n";
  std::cout << "   Blue:  " << blue << "\n\n";

  // 2. sRGB → ProPhoto → sRGB (gamut expansion)
  std::cout << "2. sRGB ↔ ProPhoto RGB round-trip\n";
  core::rgbf_t srgb_red(1.0f, 0.0f, 0.0f);
  auto pprgb = conversion::color_cast<core::prophoto_rgbf_t>(srgb_red);
  auto back = conversion::color_cast<core::rgbf_t>(pprgb);
  std::cout << "   sRGB red:          " << srgb_red << "\n";
  std::cout << "   → ProPhoto RGB:    " << pprgb << "\n";
  std::cout << "   → back to sRGB:    " << back << "\n\n";

  // 3. D50 white point / chromatic adaptation
  std::cout << "3. Chromatic adaptation (D50 ↔ D65)\n";
  core::xyz_t white_d65 = algorithms::chromatic_adaptation::WHITEPOINT_D65;
  core::xyz_t white_d50 = algorithms::chromatic_adaptation::WHITEPOINT_D50;
  std::cout << "   D65 white: " << white_d65 << "\n";
  std::cout << "   D50 white: " << white_d50 << "\n";
  auto d65_to_d50 = algorithms::chromatic_adaptation::bradford_adapt(
      white_d65, white_d65, white_d50);
  std::cout << "   Bradford(D65→D50): " << d65_to_d50 << "\n\n";

  // 4. Gamut check
  std::cout << "4. Gamut membership\n";
  core::rgbf_t in_gamut(0.5f, 0.5f, 0.5f);
  std::cout << "   Gray 0.5 in ProPhoto? " << std::boolalpha
            << algorithms::gamut::is_in_prophoto_gamut(in_gamut) << "\n";
  // ProPhoto is huge — most sRGB colors are well inside it
  std::cout << "   sRGB red in ProPhoto? "
            << algorithms::gamut::is_in_prophoto_gamut(srgb_red) << "\n\n";

  // 5. CSS parsing
  std::cout << "5. CSS color(prophoto-rgb ...) parsing\n";
  auto css = io::css::parse_css_color<core::prophoto_rgbaf_t>(
      "color(prophoto-rgb 0.5 0.3 0.7 / 0.9)");
  if (css) {
    std::cout << "   Parsed: " << *css << "\n";
    std::cout << "   CSS string: " << io::css::to_css_color_string(*css) << "\n";
  }
  std::cout << "\n";

  // 6. Gamma encoding
  std::cout << "6. Gamma encoding (Gamma 1.8 with linear segment)\n";
  auto linear_pp = conversion::color_cast<core::linear_prophoto_rgbf_t>(
      core::prophoto_rgbf_t{0.5f, 0.5f, 0.5f});
  std::cout << "   ProPhoto (gamma):  " << core::prophoto_rgbf_t{0.5f, 0.5f, 0.5f} << "\n";
  std::cout << "   Linear ProPhoto:   " << linear_pp << "\n";
  std::cout << "   (Gamma 1.8 is gentler than sRGB's ~2.2)\n\n";

  std::cout << "Done.\n";
  return 0;
}
```

- [ ] **Step 2: Build and run**

```bash
cd build && cmake --build . --target prophoto_rgb_example -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ./examples/prophoto_rgb_example
```

- [ ] **Step 3: Commit**

```bash
git add examples/prophoto_rgb_example.cpp
git commit -m "docs(examples): add ProPhoto RGB usage example

Demonstrates construction, sRGB round-trip, chromatic adaptation,
gamut checking, CSS parsing, and gamma encoding.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 12: Rec.2020 Example

**Files:**
- Create: `examples/rec2020_example.cpp`

- [ ] **Step 1: Write the example**

```cpp
/**
 * @file rec2020_example.cpp
 * @brief Demonstrates Rec.2020 (BT.2020) wide-gamut color space usage.
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make rec2020_example
 */

#include <iostream>
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

int main() {
  std::cout << "=== Rec.2020 (BT.2020) Wide-Gamut Color Space ===\n\n";

  // 1. Construct Rec.2020 colors
  std::cout << "1. Construction\n";
  core::rec2020_rgbf_t red(0.8f, 0.1f, 0.1f);
  core::rec2020_rgbf_t green(0.1f, 0.8f, 0.1f);
  core::rec2020_rgbf_t blue(0.1f, 0.1f, 0.8f);
  std::cout << "   Red:   " << red << "\n";
  std::cout << "   Green: " << green << "\n";
  std::cout << "   Blue:  " << blue << "\n\n";

  // 2. Gamut hierarchy: sRGB ⊂ Display P3 ⊂ Adobe RGB ⊂ Rec.2020 ⊂ ProPhoto
  std::cout << "2. Gamut hierarchy comparison\n";
  core::rgbf_t test_color(0.0f, 1.0f, 0.0f);  // sRGB pure green

  auto dp3 = conversion::color_cast<core::display_p3f_t>(test_color);
  auto argb = conversion::color_cast<core::adobe_rgbf_t>(test_color);
  auto r2020 = conversion::color_cast<core::rec2020_rgbf_t>(test_color);
  auto pprgb = conversion::color_cast<core::prophoto_rgbf_t>(test_color);

  std::cout << "   sRGB green:\n";
  std::cout << "     → Display P3:  " << dp3 << "\n";
  std::cout << "     → Adobe RGB:   " << argb << "\n";
  std::cout << "     → Rec.2020:    " << r2020 << "\n";
  std::cout << "     → ProPhoto:    " << pprgb << "\n";
  std::cout << "   (Notice how the same sRGB green maps to different values\n";
  std::cout << "    in each space due to different primaries)\n\n";

  // 3. Round-trip fidelity
  std::cout << "3. Rec.2020 round-trip fidelity\n";
  core::rec2020_rgbf_t orig(0.5f, 0.3f, 0.7f);
  auto via_srgb = conversion::color_cast<core::rec2020_rgbf_t>(
      conversion::color_cast<core::rgbf_t>(orig));
  std::cout << "   Original:  " << orig << "\n";
  std::cout << "   Via sRGB:   " << via_srgb << "\n\n";

  // 4. Transfer function
  std::cout << "4. BT.709-style piecewise transfer function\n";
  core::rec2020_rgbf_t mid(0.5f, 0.5f, 0.5f);
  auto linear = conversion::color_cast<core::linear_rec2020_rgbf_t>(mid);
  std::cout << "   Rec.2020 (gamma): " << mid << "\n";
  std::cout << "   Linear Rec.2020:   " << linear << "\n";
  std::cout << "   (α=1.0993, β=0.0181 — slightly different from sRGB)\n\n";

  // 5. CSS parsing
  std::cout << "5. CSS color(rec2020 ...) parsing\n";
  auto css = io::css::parse_css_color<core::rec2020_rgbaf_t>(
      "color(rec2020 0.64 0.33 0.21)");
  if (css) {
    std::cout << "   Parsed: " << *css << "\n";
    std::cout << "   CSS string: " << io::css::to_css_color_string(*css) << "\n";
  }
  std::cout << "\n";

  // 6. Rec.2020 is widely used in UHD/HDR
  std::cout << "6. Rec.2020 fact\n";
  std::cout << "   Rec.2020 covers ~75% of CIE 1931 visible gamut.\n";
  std::cout << "   It is the standard color space for 4K/8K UHDTV.\n\n";

  std::cout << "Done.\n";
  return 0;
}
```

- [ ] **Step 2: Build and run**

```bash
cd build && cmake --build . --target rec2020_example -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ./examples/rec2020_example
```

- [ ] **Step 3: Commit**

```bash
git add examples/rec2020_example.cpp
git commit -m "docs(examples): add Rec.2020 usage example

Demonstrates construction, gamut hierarchy comparison, round-trip,
transfer function, and CSS parsing for the BT.2020 color space.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 13: Chromatic Adaptation Example

**Files:**
- Create: `examples/chromatic_adaptation_example.cpp`

- [ ] **Step 1: Write the example**

```cpp
/**
 * @file chromatic_adaptation_example.cpp
 * @brief Demonstrates chromatic adaptation (white point conversion).
 *
 * Build: cmake -DCOLORCPP_BUILD_EXAMPLES=ON && make chromatic_adaptation_example
 */

#include <iostream>
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

int main() {
  std::cout << "=== Chromatic Adaptation (White Point Conversion) ===\n\n";

  using namespace algorithms::chromatic_adaptation;

  // 1. White point constants
  std::cout << "1. Standard white points (CIE XYZ, Y=1)\n";
  std::cout << "   D65 (noon daylight):     " << WHITEPOINT_D65 << "\n";
  std::cout << "   D50 (horizon daylight):  " << WHITEPOINT_D50 << "\n";
  std::cout << "   A   (tungsten):          " << WHITEPOINT_A << "\n";
  std::cout << "   D55 (mid-day):           " << WHITEPOINT_D55 << "\n\n";

  // 2. Bradford D65 → D50 adaptation
  std::cout << "2. Bradford adaptation: D65 → D50\n";
  core::xyz_t color_d65(0.5f, 0.4f, 0.3f);
  auto color_d50 = bradford_adapt(color_d65, WHITEPOINT_D65, WHITEPOINT_D50);
  std::cout << "   Color under D65: " << color_d65 << "\n";
  std::cout << "   → under D50:     " << color_d50 << "\n\n";

  // 3. Round-trip verification
  std::cout << "3. Round-trip: D65 → D50 → D65\n";
  auto back_to_d65 = bradford_adapt(color_d50, WHITEPOINT_D50, WHITEPOINT_D65);
  std::cout << "   Original:  " << color_d65 << "\n";
  std::cout << "   After D65→D50→D65: " << back_to_d65 << "\n";
  std::cout << "   (Should match original within floating-point precision)\n\n";

  // 4. Tungsten (A) simulation — warm color shift
  std::cout << "4. Daylight → Tungsten (D65 → A) — warm shift\n";
  core::rgbf_t white(1.0f, 1.0f, 1.0f);
  auto warm_white = bradford_adapt(white, WHITEPOINT_D65, WHITEPOINT_A);
  std::cout << "   White under D65:    " << white << "\n";
  std::cout << "   → under Tungsten A: " << warm_white << "\n";
  std::cout << "   (Expect warm/yellowish shift)\n\n";

  // 5. Von Kries comparison
  std::cout << "5. Bradford vs Von Kries (D65 → A)\n";
  core::xyz_t test(0.3f, 0.5f, 0.2f);
  auto brad = bradford_adapt(test, WHITEPOINT_D65, WHITEPOINT_A);
  auto vk = von_kries_adapt(test, WHITEPOINT_D65, WHITEPOINT_A);
  std::cout << "   Original:        " << test << "\n";
  std::cout << "   Bradford(D65→A): " << brad << "\n";
  std::cout << "   Von Kries(D65→A): " << vk << "\n";
  std::cout << "   (Bradford is more perceptually accurate)\n\n";

  // 6. ANSI swatches
  std::cout << "6. Visual comparison\n";
  auto white_d65_rgb = conversion::color_cast<core::rgb8_t>(core::rgbf_t{1.0f, 1.0f, 1.0f});
  io::ansi::print_swatch("D65 white  ", white_d65_rgb);

  auto tungsten_rgb = conversion::color_cast<core::rgb8_t>(
      bradford_adapt(core::rgbf_t{1.0f, 1.0f, 1.0f}, WHITEPOINT_D65, WHITEPOINT_A));
  io::ansi::print_swatch("Tungsten A ", tungsten_rgb);
  std::cout << "\n";

  std::cout << "Done.\n";
  return 0;
}
```

- [ ] **Step 2: Build and run**

```bash
cd build && cmake --build . --target chromatic_adaptation_example -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ./examples/chromatic_adaptation_example
```

- [ ] **Step 3: Commit**

```bash
git add examples/chromatic_adaptation_example.cpp
git commit -m "docs(examples): add chromatic adaptation example

Demonstrates Bradford and Von Kries adaption, white point constants,
round-trip verification, tungsten simulation, and ANSI swatch output.

Co-Authored-By: Claude <noreply@anthropic.com>"
```

---

### Task 14: Final Verification

**Files:** None

- [ ] **Step 1: Build and run full test suite**

```bash
cd build && cmake .. -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON && cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu) && ctest --output-on-failure
```

Expected: All tests pass. Zero regressions.

- [ ] **Step 2: Verify examples run without crash**

```bash
for exe in adobe_rgb_example prophoto_rgb_example rec2020_example chromatic_adaptation_example; do
  echo "--- $exe ---"
  ./examples/$exe
  echo ""
done
```

Expected: All 4 examples run and produce output without crashes.

- [ ] **Step 3: Commit (if any fixes)**

```bash
git add -A
git commit -m "chore: final verification — all tests pass, all examples run"
```

---

## Implementation Order Dependency Graph

```
Task 1 (Rec.2020 core type)
  └→ Task 2 (Rec.2020 conversion functions)
       └→ Task 3 (Register in graph)
            └→ Task 7 (CSS adaptation)

Task 4 (Chromatic adaptation)
  └→ Task 5 (Refactor ProPhoto)

Task 6 (ProPhoto gamut)

Task 8 (Uncomment Display P3 tests) — independent, can run anytime

Task 9 (Full test suite) — after Tasks 1-8

Tasks 10-13 (Examples) — after Task 9
Task 14 (Final verification) — after Tasks 10-13
```

**Parallel opportunities:** Tasks 6 and 8 are independent of Tasks 2-5 and can run in parallel with them. Task 4 can run in parallel with Tasks 1-3.
