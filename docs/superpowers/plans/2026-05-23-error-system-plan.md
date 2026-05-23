# Error System Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace all raw `throw std::out_of_range` / `std::invalid_argument` with `report_error(color_errc, msg)` backed by a custom `std::error_category`.

**Architecture:** Three new headers in `include/colorcpp/error/`:
- `color_error.hpp` — `color_errc` enum, `std::is_error_code_enum` specialization, forward declaration of `color_error_category`, declaration of `make_error_code` and `report_error`. All in `namespace colorcpp` (not `colorcpp::error`) so child namespaces resolve these names without full qualification.
- `category.hpp` — `color_error_category` singleton (implements `name()` / `message()`), `make_error_code` definition, `report_error` definition.
- `error.hpp` — umbrella includes both.

Then 18 existing files replace `#include <stdexcept>` with `#include <colorcpp/error/error.hpp>` and swap `throw std::<ex>(...)` for `report_error(color_errc::xxx, ...)`.

**Tech Stack:** C++17, `<system_error>`, `<string>`, header-only.

---

### Task 1: Create `color_error.hpp` — enum + trait + forward declarations

**Files:**
- Create: `include/colorcpp/error/color_error.hpp`

- [ ] **Step 1: Create directory and write `color_error.hpp`**

```bash
mkdir -p include/colorcpp/error
```

```cpp
/**
 * @file color_error.hpp
 * @brief Error codes and forward declarations for the colorcpp error system.
 */
#pragma once

#include <system_error>

namespace colorcpp {

// Forward declaration (defined in category.hpp)
class color_error_category;

enum class color_errc {
  invalid_channel_value = 1,
  value_out_of_range,
  invalid_hex_format,
  unknown_named_color,
  insufficient_stops,
  invalid_stop_position,
  invalid_segment,
  invalid_scale_factor,
  invalid_quantize_levels,
  invalid_option_range,
  non_finite_value,
};

const color_error_category& color_category() noexcept;

std::error_code make_error_code(color_errc e) noexcept;

[[noreturn]] void report_error(color_errc code, const char* msg);

}  // namespace colorcpp

namespace std {

template <>
struct is_error_code_enum<colorcpp::color_errc> : true_type {};

}  // namespace std
```

- [ ] **Step 2: Commit**

```bash
git add include/colorcpp/error/color_error.hpp
git commit -m "feat: add color_errc enum with trait and forward declarations"
```

---

### Task 2: Create `category.hpp` — category singleton + implementations

**Files:**
- Create: `include/colorcpp/error/category.hpp`

- [ ] **Step 1: Write `category.hpp`**

```cpp
/**
 * @file category.hpp
 * @brief @ref colorcpp::color_error_category and error helper definitions.
 */
#pragma once

#include <colorcpp/error/color_error.hpp>
#include <string>

namespace colorcpp {

class color_error_category final : public std::error_category {
  friend const color_error_category& color_category() noexcept;
  color_error_category() noexcept = default;

 public:
  static const color_error_category& instance() noexcept {
    static const color_error_category s;
    return s;
  }

  const char* name() const noexcept override { return "colorcpp"; }

  std::string message(int ev) const override {
    switch (static_cast<color_errc>(ev)) {
      case color_errc::invalid_channel_value:   return "colorcpp: channel value out of range";
      case color_errc::value_out_of_range:      return "colorcpp: value out of range";
      case color_errc::invalid_hex_format:      return "colorcpp: invalid hex format";
      case color_errc::unknown_named_color:     return "colorcpp: unknown named color";
      case color_errc::insufficient_stops:      return "colorcpp: insufficient stops";
      case color_errc::invalid_stop_position:   return "colorcpp: stop position out of range";
      case color_errc::invalid_segment:         return "colorcpp: invalid segment";
      case color_errc::invalid_scale_factor:    return "colorcpp: scale factor must be positive";
      case color_errc::invalid_quantize_levels: return "colorcpp: quantize levels must be at least 1";
      case color_errc::invalid_option_range:    return "colorcpp: option range invalid";
      case color_errc::non_finite_value:        return "colorcpp: non-finite value";
      default:                                  return "colorcpp: unknown error";
    }
  }
};

inline const color_error_category& color_category() noexcept {
  return color_error_category::instance();
}

inline std::error_code make_error_code(color_errc e) noexcept {
  return {static_cast<int>(e), color_error_category::instance()};
}

[[noreturn]] inline void report_error(color_errc code, const char* msg) {
  throw std::system_error(make_error_code(code), msg);
}

}  // namespace colorcpp
```

- [ ] **Step 2: Commit**

```bash
git add include/colorcpp/error/category.hpp
git commit -m "feat: add color_error_category, make_error_code, and report_error"
```

---

### Task 3: Create umbrella header and register in `colorcpp.hpp`

**Files:**
- Create: `include/colorcpp/error/error.hpp`
- Modify: `include/colorcpp/colorcpp.hpp`

- [ ] **Step 1: Write `error.hpp`**

```cpp
/**
 * @file error.hpp
 * @brief Umbrella header for the colorcpp error module.
 */
#pragma once

#include <colorcpp/error/color_error.hpp>
#include <colorcpp/error/category.hpp>
```

- [ ] **Step 2: Add error include to `include/colorcpp/colorcpp.hpp`**

After the `// Include color io header` line and its include, add:

```cpp
// Include color error module
#include <colorcpp/error/error.hpp>
```

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/error/error.hpp include/colorcpp/colorcpp.hpp
git commit -m "feat: add error module umbrella and register in colorcpp.hpp"
```

---

### Task 4: Migrate `core/color_base.hpp` (2 throw sites → `invalid_channel_value`)

**Files:**
- Modify: `include/colorcpp/core/color_base.hpp`

- [ ] **Step 1: Replace `#include <stdexcept>` with `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace throw in `set()`**

Old:
```cpp
    if (!(v >= static_cast<T>(channel_t::min) && v <= static_cast<T>(channel_t::max))) {
      throw std::out_of_range("colorcpp: channel value out of range");
    }
```

New:
```cpp
    if (!(v >= static_cast<T>(channel_t::min) && v <= static_cast<T>(channel_t::max))) {
      report_error(color_errc::invalid_channel_value, "colorcpp: channel value out of range");
    }
```

- [ ] **Step 3: Replace throw in `check_range()`**

Old:
```cpp
    if (!valid) {
      throw std::out_of_range("colorcpp: channel value out of range");
    }
```

New:
```cpp
    if (!valid) {
      report_error(color_errc::invalid_channel_value, "colorcpp: channel value out of range");
    }
```

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/core/color_base.hpp
git commit -m "refactor: migrate color_base to color_errc"
```

---

### Task 5: Migrate `core/palette_set.hpp` (1 throw site → `value_out_of_range`)

**Files:**
- Modify: `include/colorcpp/core/palette_set.hpp`

- [ ] **Step 1: Replace `#include <stdexcept>` with `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace throw in `at()`**

Old:
```cpp
    if (index >= colors_.size()) throw std::out_of_range("colorcpp: palette index out of range");
```

New:
```cpp
    if (index >= colors_.size()) report_error(color_errc::value_out_of_range, "colorcpp: palette index out of range");
```

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/core/palette_set.hpp
git commit -m "refactor: migrate palette_set to color_errc"
```

---

### Task 6: Migrate `io/literals/details.hpp` (2 throw sites → `invalid_hex_format`)

**Files:**
- Modify: `include/colorcpp/io/literals/details.hpp`

- [ ] **Step 1: Replace `#include <stdexcept>` with `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace throw in `validate_hex_chars()`**

Old:
```cpp
      throw std::invalid_argument("colorcpp: invalid hex digit in _hex literal");
```

New:
```cpp
      report_error(color_errc::invalid_hex_format, "colorcpp: invalid hex digit in _hex literal");
```

- [ ] **Step 3: Replace throw in `parse_hex_string()`**

Old:
```cpp
    throw std::invalid_argument("colorcpp: invalid hex string length. Expected 3, 4, 6, or 8 characters.");
```

New:
```cpp
    report_error(color_errc::invalid_hex_format, "colorcpp: invalid hex string length. Expected 3, 4, 6, or 8 characters.");
```

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/io/literals/details.hpp
git commit -m "refactor: migrate hex literal parsing to color_errc"
```

---

### Task 7: Migrate `io/literals/rgb.hpp` (3 throw sites → `value_out_of_range`)

**Files:**
- Modify: `include/colorcpp/io/literals/rgb.hpp`

- [ ] **Step 1: Add `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace 3 throw statements**

Old (line 54):
```cpp
    throw std::out_of_range("colorcpp: _rgb value exceeds 0xFFFFFF (24-bit limit)");
```

New:
```cpp
    report_error(color_errc::value_out_of_range, "colorcpp: _rgb value exceeds 0xFFFFFF (24-bit limit)");
```

Old (line 63):
```cpp
    throw std::out_of_range("colorcpp: _rgba value exceeds 0xFFFFFFFF (32-bit limit)");
```

New:
```cpp
    report_error(color_errc::value_out_of_range, "colorcpp: _rgba value exceeds 0xFFFFFFFF (32-bit limit)");
```

Old (line 72):
```cpp
    throw std::out_of_range("colorcpp: _argb value exceeds 0xFFFFFFFF (32-bit limit)");
```

New:
```cpp
    report_error(color_errc::value_out_of_range, "colorcpp: _argb value exceeds 0xFFFFFFFF (32-bit limit)");
```

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/io/literals/rgb.hpp
git commit -m "refactor: migrate rgb literals to color_errc"
```

---

### Task 8: Migrate `io/css/named_colors.hpp` (1 throw site → `unknown_named_color`)

**Files:**
- Modify: `include/colorcpp/io/css/named_colors.hpp`

- [ ] **Step 1: Add `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace throw**

Old:
```cpp
    throw std::invalid_argument(std::string("colorcpp: unknown named color '") + std::string(str, len) + "'");
```

New:
```cpp
    report_error(color_errc::unknown_named_color,
                 (std::string("colorcpp: unknown named color '") + std::string(str, len) + "'").c_str());
```

`std::system_error` copies the `what()` string internally, so the temporary `std::string` is safe.

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/io/css/named_colors.hpp
git commit -m "refactor: migrate named_colors to color_errc"
```

---

### Task 9: Migrate `algorithms/gradient/details.hpp` (5 throw sites)

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/details.hpp`

- [ ] **Step 1: Replace `#include <stdexcept>` with `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Line 31 — `find_bracketing_stops` → `insufficient_stops`**

```cpp
    report_error(color_errc::insufficient_stops, "colorcpp: gradient has no stops");
```

- [ ] **Step 3: Line 126 — `sample_gradient` → `insufficient_stops`**

```cpp
    report_error(color_errc::insufficient_stops, "colorcpp: gradient has no stops");
```

- [ ] **Step 4: Line 187 — `validate_stops` → `insufficient_stops`**

```cpp
    report_error(color_errc::insufficient_stops, "colorcpp: gradient must have at least one stop");
```

- [ ] **Step 5: Line 193 — `validate_stops` → `invalid_stop_position`**

```cpp
      report_error(color_errc::invalid_stop_position, "colorcpp: stop position must be in [0, 1]");
```

- [ ] **Step 6: Line 199 — `validate_stops` → `non_finite_value`**

```cpp
      report_error(color_errc::non_finite_value, "colorcpp: stop color contains non-finite values (NaN or Inf)");
```

- [ ] **Step 7: Commit**

```bash
git add include/colorcpp/algorithms/gradient/details.hpp
git commit -m "refactor: migrate gradient details to color_errc"
```

---

### Task 10: Migrate `algorithms/gradient/stops.hpp` (6 throw sites)

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/stops.hpp`

- [ ] **Step 1: Add `#include <colorcpp/error/error.hpp>`, remove `#include <stdexcept>`**

- [ ] **Step 2: Replace all 6 throw statements**

Line 36 (constructor position):
```cpp
      report_error(color_errc::invalid_stop_position, "colorcpp: stop position must be in [0, 1]");
```

Line 83 (to_replace position):
```cpp
      report_error(color_errc::invalid_stop_position, "colorcpp: stop position must be in [0, 1]");
```

Line 96 (index access):
```cpp
      report_error(color_errc::value_out_of_range, "colorcpp: stop index out of range");
```

Line 124 (remove_stop):
```cpp
      report_error(color_errc::value_out_of_range, "colorcpp: stop index out of range");
```

Line 138 (replace_at index):
```cpp
      report_error(color_errc::value_out_of_range, "colorcpp: stop index out of range");
```

Line 141 (replace_at position):
```cpp
      report_error(color_errc::invalid_stop_position, "colorcpp: stop position must be in [0, 1]");
```

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/algorithms/gradient/stops.hpp
git commit -m "refactor: migrate gradient stops to color_errc"
```

---

### Task 11: Migrate gradient shape files (7 files, 18 throw sites)

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/angular.hpp`
- Modify: `include/colorcpp/algorithms/gradient/box.hpp`
- Modify: `include/colorcpp/algorithms/gradient/diamond.hpp`
- Modify: `include/colorcpp/algorithms/gradient/linear.hpp`
- Modify: `include/colorcpp/algorithms/gradient/radial.hpp`
- Modify: `include/colorcpp/algorithms/gradient/sequence.hpp`
- Modify: `include/colorcpp/algorithms/gradient/stepped.hpp`

- [ ] **Step 1: angular.hpp — add `#include <colorcpp/error/error.hpp>`, replace 2 throws**

Line 196 (scale factor):
```cpp
      report_error(color_errc::invalid_scale_factor, "colorcpp: scale factor must be positive");
```

Line 229 (quantize levels):
```cpp
      report_error(color_errc::invalid_quantize_levels, "colorcpp: quantize levels must be at least 1");
```

- [ ] **Step 2: box.hpp — add include, same 2 replacements (scale factor + quantize levels)**

- [ ] **Step 3: diamond.hpp — add include, same 2 replacements**

- [ ] **Step 4: linear.hpp — add include, same 2 replacements**

- [ ] **Step 5: radial.hpp — add include, same 2 replacements**

- [ ] **Step 6: sequence.hpp — add include, replace 4 throws**

Line 40:
```cpp
      report_error(color_errc::invalid_stop_position, "colorcpp: segment positions must be in [0, 1]");
```

Line 43:
```cpp
      report_error(color_errc::invalid_segment, "colorcpp: segment start must be less than end");
```

Line 167:
```cpp
      report_error(color_errc::insufficient_stops, "colorcpp: sequence gradient must have at least one segment");
```

Line 173:
```cpp
        report_error(color_errc::invalid_segment, "colorcpp: sequence segments must not overlap");
```

- [ ] **Step 7: stepped.hpp — add include, replace 2 throws**

Lines 33, 58:
```cpp
      report_error(color_errc::invalid_quantize_levels, "colorcpp: stepped gradient levels must be at least 1");
```

- [ ] **Step 8: Commit**

```bash
git add include/colorcpp/algorithms/gradient/angular.hpp \
        include/colorcpp/algorithms/gradient/box.hpp \
        include/colorcpp/algorithms/gradient/diamond.hpp \
        include/colorcpp/algorithms/gradient/linear.hpp \
        include/colorcpp/algorithms/gradient/radial.hpp \
        include/colorcpp/algorithms/gradient/sequence.hpp \
        include/colorcpp/algorithms/gradient/stepped.hpp
git commit -m "refactor: migrate gradient shapes to color_errc"
```

---

### Task 12: Migrate `operations/interpolate/color_lerp.hpp` (8 throw sites → `insufficient_stops`)

**Files:**
- Modify: `include/colorcpp/operations/interpolate/color_lerp.hpp`

- [ ] **Step 1: Add `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace all 8 throw statements**

Each line follows `if (stops.size() < 2) throw std::invalid_argument(...)`. Messages preserved verbatim:

Line 173: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp requires at least 2 colour stops");`
Line 191: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp_hsl requires at least 2 colour stops");`
Line 209: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp_oklab requires at least 2 colour stops");`
Line 227: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp_oklch requires at least 2 colour stops");`
Line 249: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp requires at least 2 colour stops");`
Line 274: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp_hsl requires at least 2 colour stops");`
Line 299: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp_oklab requires at least 2 colour stops");`
Line 324: `if (stops.size() < 2) report_error(color_errc::insufficient_stops, "colorcpp: multi_lerp_oklch requires at least 2 colour stops");`

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/operations/interpolate/color_lerp.hpp
git commit -m "refactor: migrate color_lerp to color_errc"
```

---

### Task 13: Migrate `operations/random/hsl.hpp` (2 throw sites → `invalid_option_range`)

**Files:**
- Modify: `include/colorcpp/operations/random/hsl.hpp`

- [ ] **Step 1: Add `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace 2 throws**

Line 92:
```cpp
      report_error(color_errc::invalid_option_range, "colorcpp: generator options min must not exceed max");
```

Line 94:
```cpp
      if (o.a_min > o.a_max) report_error(color_errc::invalid_option_range, "colorcpp: generator alpha min must not exceed max");
```

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/operations/random/hsl.hpp
git commit -m "refactor: migrate random/hsl to color_errc"
```

---

### Task 14: Migrate `operations/random/luminance.hpp` (2 throw sites → `invalid_option_range`)

**Files:**
- Modify: `include/colorcpp/operations/random/luminance.hpp`

- [ ] **Step 1: Add `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace 2 throws**

Line 86:
```cpp
    if (o.l_min > o.l_max) report_error(color_errc::invalid_option_range, "colorcpp: l_min must not exceed l_max");
```

Line 87:
```cpp
    if (o.chroma_max < 0.0f) report_error(color_errc::invalid_option_range, "colorcpp: chroma_max must be non-negative");
```

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/operations/random/luminance.hpp
git commit -m "refactor: migrate random/luminance to color_errc"
```

---

### Task 15: Migrate `operations/random/contrast.hpp` (3 throw sites → `invalid_option_range`)

**Files:**
- Modify: `include/colorcpp/operations/random/contrast.hpp`

- [ ] **Step 1: Add `#include <colorcpp/error/error.hpp>`**

- [ ] **Step 2: Replace 3 throws**

Line 117:
```cpp
    if (o.l_min > o.l_max) report_error(color_errc::invalid_option_range, "colorcpp: l_min must not exceed l_max");
```

Line 118:
```cpp
    if (o.chroma_max < 0.0f) report_error(color_errc::invalid_option_range, "colorcpp: chroma_max must be non-negative");
```

Line 119:
```cpp
    if (o.min_ratio < 1.0f) report_error(color_errc::invalid_option_range, "colorcpp: min_ratio must be >= 1.0");
```

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/operations/random/contrast.hpp
git commit -m "refactor: migrate random/contrast to color_errc"
```

---

### Task 16: Verify migration completeness

**Files:**
- None (verification only)

- [ ] **Step 1: Confirm no raw `throw std::` remains in `include/`**

```bash
grep -rn "throw std::" include/ --include="*.hpp"
```

Expected: zero results.

- [ ] **Step 2: Count converted throw sites**

```bash
grep -rn "report_error" include/ --include="*.hpp" | wc -l
```

Expected: 48

- [ ] **Step 3: Confirm no stray `#include <stdexcept>` left in project files**

```bash
grep -rn '<stdexcept>' include/colorcpp/ --include="*.hpp" | grep -v "color_error.hpp"
```

Expected: zero results. (`color_error.hpp` doesn't include `<stdexcept>` — only `<system_error>` provides `std::system_error`).

---

### Task 17: Build and run existing tests

- [ ] **Step 1: Configure with tests enabled**

```bash
cmake -B build -DCOLORCPP_BUILD_TESTS=ON
```

- [ ] **Step 2: Build**

```bash
cmake --build build -j
```

Expected: build succeeds with no errors.

- [ ] **Step 3: Run tests**

```bash
cd build && ctest --output-on-failure
```

Expected: all existing tests pass.

---

### Task 18: Add unit tests for error module

**Files:**
- Create: `tests/error/test_error.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Read `tests/CMakeLists.txt` to understand test target registration pattern**

```bash
cat tests/CMakeLists.txt
```

- [ ] **Step 2: Create `tests/error/test_error.cpp`**

```cpp
#include <colorcpp/error/error.hpp>
#include <gtest/gtest.h>

TEST(ColorErrorCategory, Name) {
  EXPECT_STREQ(colorcpp::color_error_category::instance().name(), "colorcpp");
}

TEST(ColorErrorCategory, MessageInvalidChannelValue) {
  auto msg = colorcpp::color_error_category::instance().message(
      static_cast<int>(colorcpp::color_errc::invalid_channel_value));
  EXPECT_STREQ(msg.c_str(), "colorcpp: channel value out of range");
}

TEST(ColorErrorCategory, MessageInsufficientStops) {
  auto msg = colorcpp::color_error_category::instance().message(
      static_cast<int>(colorcpp::color_errc::insufficient_stops));
  EXPECT_STREQ(msg.c_str(), "colorcpp: insufficient stops");
}

TEST(ColorErrorCategory, MessageUnknownError) {
  auto msg = colorcpp::color_error_category::instance().message(999);
  EXPECT_STREQ(msg.c_str(), "colorcpp: unknown error");
}

TEST(MakeErrorCode, CreatesErrorCode) {
  auto ec = make_error_code(colorcpp::color_errc::invalid_channel_value);
  EXPECT_EQ(ec.category(), colorcpp::color_error_category::instance());
  EXPECT_EQ(ec.value(), static_cast<int>(colorcpp::color_errc::invalid_channel_value));
}

TEST(MakeErrorCode, AllCodesHaveMessages) {
  using colorcpp::color_errc;
  for (int i = 1; i <= static_cast<int>(color_errc::non_finite_value); ++i) {
    auto msg = colorcpp::color_error_category::instance().message(i);
    EXPECT_NE(msg, "colorcpp: unknown error")
        << "Error code " << i << " has no custom message";
  }
}

TEST(ReportError, ThrowsSystemError) {
  using colorcpp::color_errc;
  EXPECT_THROW(report_error(color_errc::value_out_of_range, "test msg"),
               std::system_error);
}

TEST(ReportError, PreservesMessageAndCode) {
  using colorcpp::color_errc;
  try {
    report_error(color_errc::invalid_hex_format,
                 "colorcpp: invalid hex format");
  } catch (const std::system_error& e) {
    EXPECT_STREQ(e.what(), "colorcpp: invalid hex format");
    EXPECT_EQ(e.code(), make_error_code(color_errc::invalid_hex_format));
  }
}

TEST(StdErrorCodeEnum, ImplicitConversion) {
  std::error_code ec = colorcpp::color_errc::non_finite_value;
  EXPECT_EQ(ec.category(), colorcpp::color_error_category::instance());
  EXPECT_EQ(ec.value(), static_cast<int>(colorcpp::color_errc::non_finite_value));
}
```

- [ ] **Step 3: Register test in `tests/CMakeLists.txt` following existing conventions**

- [ ] **Step 4: Build and run error tests**

```bash
cmake --build build -j && cd build && ctest -R error --output-on-failure
```

Expected: all error tests pass.

- [ ] **Step 5: Commit**

```bash
git add tests/error/test_error.cpp tests/CMakeLists.txt
git commit -m "test: add error module unit tests"
```

---

### Task 19: Final verification — full build + full test suite

- [ ] **Step 1: Clean rebuild**

```bash
rm -rf build && cmake -B build -DCOLORCPP_BUILD_TESTS=ON && cmake --build build -j
```

- [ ] **Step 2: Run full test suite**

```bash
cd build && ctest --output-on-failure
```

Expected: all tests pass with zero failures.
