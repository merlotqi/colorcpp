# CSS `color-mix()` Extensions Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Extend `colorcpp`'s CSS parser from the current two-operand `color-mix()` implementation to a list-based implementation that supports omitted interpolation methods, single-item mixes, multi-item mixes, and the CSS Values 5 progress form.

**Architecture:** Keep the refactor local to `include/colorcpp/io/css/parse_detail.hpp`. Parse top-level comma-separated segments once, normalize both item-list and progress-form inputs into a shared `parsed_color_mix` model, resolve weights separately from color parsing, and evaluate the mix by folding through the existing pairwise mixing helpers so rectangular and polar semantics stay in one place.

**Tech Stack:** C++20, header-only parser internals, GoogleTest/CTest, existing `colorcpp::operations::conversion` and interpolation utilities.

---

## File Map

- Modify: `include/colorcpp/io/css/parse_detail.hpp:42-145`
  - Replace the two-operand `color_mix_operand` and `resolve_color_mix_weights(...)` helpers with list-aware parsing and normalization helpers while keeping `color_mix_weights` as the pairwise mix input type.
- Modify: `include/colorcpp/io/css/parse_detail.hpp:308-371`
  - Rewrite `parse_color_mix_rgbaf(...)` so it dispatches through the shared parsed model instead of hard-coding `raw_first` / `raw_second`.
- Modify: `tests/io/test_css.cpp:640-733`
  - Add focused positive tests for default interpolation, single-item mixes, multi-item mixes, and progress-form parsing.
- Modify: `tests/io/test_css.cpp:858-932`
  - Extend the exact corpus and invalid corpus with new `color-mix()` coverage.
- Modify: `README.md:152-164`
  - Update the public capability list and remove the stale “multi-stop / at <percentage>” caveat.
- Modify: `docs/reference/css_color.rst:45-86`
  - Document omitted-method, multi-item, and progress-form support.
- Modify: `docs/todo.md`
  - Mark the `color-mix()` gap as closed while leaving the remaining CSS parser work visible.

## Standards Note

This plan tracks the current W3C surface described by CSS Color Module Level 5 (published April 13, 2026) and CSS Values and Units Level 5 (published November 11, 2024):

- CSS Color 5 now defines `color-mix()` as a list of one or more mix items and defaults omitted interpolation methods to `oklab`.
- CSS Values 5 adds the progress-form wrapper `color-mix([ <progress> && <color-interpolation-method>? ], <color>, <color>)`.

This plan intentionally keeps one current repository policy from the approved design: zero-total mixes stay invalid instead of returning transparent black. That preserves current tests and behavior for this pass. If strict April 2026 spec parity becomes the priority, change Task 4’s zero-total expectations before execution.

### Task 1: Default Method And Single-Item Support

**Files:**
- Modify: `tests/io/test_css.cpp:640-694`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:42-108`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:308-371`
- Test: `tests/io/test_css.cpp`

- [ ] **Step 1: Write the failing test**

```cpp
TEST(Css, ColorMixDefaultsToOklabAndAllowsSingleItem) {
  auto implicit = parse_css_color_rgbaf("color-mix(red, blue)");
  auto explicit_oklab = parse_css_color_rgbaf("color-mix(in oklab, red, blue)");
  ASSERT_TRUE(implicit);
  ASSERT_TRUE(explicit_oklab);
  expect_rgbaf_near(*implicit, explicit_oklab->r(), explicit_oklab->g(), explicit_oklab->b(), explicit_oklab->a(),
                    0.01f);

  auto single = parse_css_color_rgbaf("color-mix(in srgb, red)");
  ASSERT_TRUE(single);
  expect_rgbaf_near(*single, 1.0f, 0.0f, 0.0f, 1.0f, 0.01f);
}
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `ctest --test-dir build -R '^test_css\.Css\.ColorMixDefaultsToOklabAndAllowsSingleItem$' --output-on-failure`

Expected: FAIL because the current parser rejects `color-mix(red, blue)` and any single-item form that does not have exactly two operands.

- [ ] **Step 3: Write the minimal implementation**

Add the missing include near the top of `include/colorcpp/io/css/parse_detail.hpp`:

```cpp
#include <vector>
```

Replace the two-item-only operand model with the shared parsed form:

```cpp
struct color_mix_item {
  std::string_view color;
  std::optional<float> weight;
};

struct parsed_color_mix {
  color_mix_interpolation_method method{color_mix_space::oklab};
  std::vector<color_mix_item> items;
};
```

Add a comma splitter that preserves nested function calls:

```cpp
inline std::optional<std::vector<std::string_view>> split_top_level_comma_list(std::string_view s) {
  std::vector<std::string_view> parts;
  size_t start = 0;
  while (start <= s.size()) {
    const size_t comma = details::find_top_level_comma(s, start);
    std::string_view part =
        comma == std::string_view::npos ? s.substr(start) : s.substr(start, comma - start);
    details::trim(part);
    if (part.empty()) return std::nullopt;
    parts.push_back(part);
    if (comma == std::string_view::npos) break;
    start = comma + 1;
  }
  return parts;
}
```

Add a first-pass item-list parser that defaults to `oklab` when `in ...` is omitted:

```cpp
inline std::optional<parsed_color_mix> parse_color_mix_item_list(
    const std::vector<std::string_view>& parts) {
  if (parts.empty()) return std::nullopt;

  parsed_color_mix parsed;
  size_t first_item_index = 0;

  details::Cursor header{parts.front(), 0};
  if (header.consume_ci("in")) {
    header.skip_ws();
    auto method = parse_color_mix_space(header);
    if (!method) return std::nullopt;
    header.skip_ws();
    if (!header.eof()) return std::nullopt;
    parsed.method = *method;
    first_item_index = 1;
  }

  if (first_item_index >= parts.size()) return std::nullopt;
  for (size_t index = first_item_index; index < parts.size(); ++index) {
    auto item = split_color_and_optional_percent(parts[index]);
    if (!item) {
      parsed.items.push_back(color_mix_item{parts[index], std::nullopt});
      continue;
    }
    parsed.items.push_back(color_mix_item{item->color, item->weight});
  }
  return parsed;
}
```

Keep the existing `color_mix_operand` and `resolve_color_mix_weights(...)` helpers in place for this bootstrapping step; Task 2 removes that temporary bridge.

Rewrite `parse_color_mix_rgbaf(...)` so it no longer requires an explicit `in ...` prefix and temporarily allows one or two items:

```cpp
auto parts = split_top_level_comma_list(inner);
if (!parts) {
  c.i = save;
  return std::nullopt;
}

auto parsed = parse_color_mix_item_list(*parts);
if (!parsed) {
  c.i = save;
  return std::nullopt;
}

if (parsed->items.size() == 1) {
  auto only = parse_css_color_rgbaf(parsed->items.front().color, context);
  if (!only) {
    c.i = save;
    return std::nullopt;
  }
  return *only;
}

if (parsed->items.size() != 2) {
  c.i = save;
  return std::nullopt;
}

const color_mix_operand first{parsed->items[0].color, parsed->items[0].weight};
const color_mix_operand second{parsed->items[1].color, parsed->items[1].weight};
const auto weights = resolve_color_mix_weights(first, second);
if (!weights) {
  c.i = save;
  return std::nullopt;
}

auto c1 = parse_css_color_rgbaf(first.color, context);
auto c2 = parse_css_color_rgbaf(second.color, context);
if (!c1 || !c2) {
  c.i = save;
  return std::nullopt;
}

return mix_colors_in_space(parsed->method, *c1, *c2, *weights);
```

- [ ] **Step 4: Run the test to verify it passes**

Run: `ctest --test-dir build -R '^test_css\.Css\.(ColorMixDefaultsToOklabAndAllowsSingleItem|ColorMixInSrgb|ColorMixNestedAndInvalidForms|ColorMixHueInterpolationKeywords)$' --output-on-failure`

Expected: PASS for all four tests.

- [ ] **Step 5: Commit**

```bash
git add tests/io/test_css.cpp include/colorcpp/io/css/parse_detail.hpp
git commit -m "feat: add default and single-item color-mix parsing"
```

### Task 2: Multi-Item Parsing And Folded Evaluation

**Files:**
- Modify: `tests/io/test_css.cpp:640-733`
- Modify: `tests/io/test_css.cpp:858-891`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:42-305`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:308-371`
- Test: `tests/io/test_css.cpp`

- [ ] **Step 1: Write the failing tests**

Add a focused multi-item test:

```cpp
TEST(Css, ColorMixSupportsMultiItemLists) {
  auto equal = parse_css_color_rgba8("color-mix(in srgb, red, green, blue)");
  ASSERT_TRUE(equal);
  expect_rgba(*equal, 85, 85, 85, 255);

  auto weighted = parse_css_color_rgba8("color-mix(in srgb, 25% red, blue, white 25%)");
  ASSERT_TRUE(weighted);
  expect_rgba(*weighted, 128, 64, 191, 255);

  auto folded = parse_css_color_rgbaf(
      "color-mix(in oklch longer hue, oklch(0.7 0.04 30), oklch(0.7 0.04 90), oklch(0.7 0.04 150))");
  auto manual = parse_css_color_rgbaf(
      "color-mix(in oklch longer hue, color-mix(in oklch longer hue, oklch(0.7 0.04 30), oklch(0.7 0.04 90)), "
      "oklch(0.7 0.04 150) 33.3333%)");
  ASSERT_TRUE(folded);
  ASSERT_TRUE(manual);
  expect_rgbaf_near(*folded, manual->r(), manual->g(), manual->b(), manual->a(), 0.02f);
}
```

Extend the exact corpus with one exact multi-item case:

```cpp
{"color-mix(in srgb, red, green, blue)", 85, 85, 85, 255},
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `ctest --test-dir build -R '^test_css\.Css\.(ColorMixSupportsMultiItemLists|WptStyleExactCorpus)$' --output-on-failure`

Expected: FAIL because the current temporary implementation still rejects more than two items and does not parse leading percentages such as `25% red`.

- [ ] **Step 3: Write the minimal implementation**

Add dedicated leading and trailing percentage helpers:

```cpp
inline std::optional<std::pair<float, std::string_view>> split_optional_leading_percent(
    std::string_view s) {
  details::Cursor c{s, 0};
  auto cv = c.parse_component_value();
  if (!cv || !cv->second) return std::nullopt;
  std::string_view color = s.substr(c.i);
  details::trim(color);
  if (color.empty()) return std::nullopt;
  const float weight = static_cast<float>(std::clamp(cv->first, 0.0, 100.0) / 100.0);
  return std::pair<float, std::string_view>{weight, color};
}

inline std::optional<std::pair<std::string_view, float>> split_optional_trailing_percent(
    std::string_view s) {
  details::trim(s);
  if (s.empty() || s.back() != '%') return std::nullopt;

  size_t i = s.size() - 1;
  if (i == 0) return std::nullopt;
  --i;
  while (i < s.size() && details::is_space(s[i])) --i;
  if (i >= s.size()) return std::nullopt;

  const size_t number_end = i + 1;
  size_t number_start = number_end;
  while (number_start > 0) {
    const char ch = s[number_start - 1];
    if (std::isdigit(static_cast<unsigned char>(ch)) || ch == '.' || ch == '-' || ch == '+') {
      --number_start;
    } else {
      break;
    }
  }

  if (number_start == number_end) return std::nullopt;

  details::Cursor nc{s.substr(number_start, number_end - number_start), 0};
  auto number = nc.parse_number();
  if (!number) return std::nullopt;

  size_t color_end = number_start;
  while (color_end > 0 && details::is_space(s[color_end - 1])) --color_end;
  std::string_view color = s.substr(0, color_end);
  details::trim(color);
  if (color.empty()) return std::nullopt;

  const float weight = static_cast<float>(std::clamp(*number, 0.0, 100.0) / 100.0);
  return std::pair<std::string_view, float>{color, weight};
}
```

Upgrade item parsing so both `red 25%` and `25% red` work:

```cpp
inline std::optional<color_mix_item> parse_color_mix_item(std::string_view s) {
  details::trim(s);
  if (s.empty()) return std::nullopt;

  if (auto leading = split_optional_leading_percent(s)) {
    return color_mix_item{leading->second, leading->first};
  }
  if (auto trailing = split_optional_trailing_percent(s)) {
    return color_mix_item{trailing->first, trailing->second};
  }
  return color_mix_item{s, std::nullopt};
}
```

Replace the old item push loop in `parse_color_mix_item_list(...)`:

```cpp
for (size_t index = first_item_index; index < parts.size(); ++index) {
  auto item = parse_color_mix_item(parts[index]);
  if (!item) return std::nullopt;
  parsed.items.push_back(*item);
}
```

Add list-aware resolution and evaluation helpers:

```cpp
struct resolved_color_mix_item {
  core::rgbaf_t color;
  float weight = 0.0f;
};

struct resolved_color_mix {
  color_mix_interpolation_method method{color_mix_space::oklab};
  std::vector<resolved_color_mix_item> items;
  float alpha_multiplier = 1.0f;
};

inline std::optional<resolved_color_mix> resolve_color_mix_items(
    const parsed_color_mix& parsed, const parse_css_color_context& context) {
  if (parsed.items.empty()) return std::nullopt;

  resolved_color_mix resolved;
  resolved.method = parsed.method;

  size_t missing_count = 0;
  float explicit_sum = 0.0f;
  for (const auto& item : parsed.items) {
    if (item.weight) {
      explicit_sum += std::clamp(*item.weight, 0.0f, 1.0f);
    } else {
      ++missing_count;
    }
  }

  const float fill_weight =
      missing_count == 0
          ? 0.0f
          : (explicit_sum <= 0.0f && missing_count == parsed.items.size()
                 ? 1.0f / static_cast<float>(missing_count)
                 : std::max(0.0f, 1.0f - explicit_sum) / static_cast<float>(missing_count));

  float total = 0.0f;
  for (const auto& item : parsed.items) {
    auto color = parse_css_color_rgbaf(item.color, context);
    if (!color) return std::nullopt;
    const float weight = item.weight ? std::clamp(*item.weight, 0.0f, 1.0f) : fill_weight;
    resolved.items.push_back(resolved_color_mix_item{*color, weight});
    total += weight;
  }

  if (total <= 0.0f) return std::nullopt;

  resolved.alpha_multiplier = total < 1.0f ? total : 1.0f;
  for (auto& item : resolved.items) {
    item.weight /= total;
  }
  return resolved;
}

inline core::rgbaf_t evaluate_color_mix(const resolved_color_mix& resolved) {
  if (resolved.items.size() == 1) {
    auto out = resolved.items.front().color;
    out.a() = std::clamp(out.a() * resolved.alpha_multiplier, 0.0f, 1.0f);
    return out;
  }

  core::rgbaf_t mixed = resolved.items.front().color;
  float accumulated_weight = resolved.items.front().weight;
  for (size_t index = 1; index < resolved.items.size(); ++index) {
    const float combined = accumulated_weight + resolved.items[index].weight;
    color_mix_weights pair;
    pair.first = accumulated_weight / combined;
    pair.second = resolved.items[index].weight / combined;
    pair.alpha_multiplier = 1.0f;
    mixed = mix_colors_in_space(resolved.method, mixed, resolved.items[index].color, pair);
    accumulated_weight = combined;
  }

  mixed.a() = std::clamp(mixed.a() * resolved.alpha_multiplier, 0.0f, 1.0f);
  return mixed;
}
```

Finish the refactor in `parse_color_mix_rgbaf(...)`:

```cpp
auto parts = split_top_level_comma_list(inner);
if (!parts) {
  c.i = save;
  return std::nullopt;
}

auto parsed = parse_color_mix_item_list(*parts);
if (!parsed) {
  c.i = save;
  return std::nullopt;
}

auto resolved = resolve_color_mix_items(*parsed, context);
if (!resolved) {
  c.i = save;
  return std::nullopt;
}

return evaluate_color_mix(*resolved);
```

Delete the now-unused two-item-only bridge once the list-aware path is in place:

```cpp
// Remove:
// struct color_mix_operand { ... };
// inline std::optional<color_mix_weights> resolve_color_mix_weights(...);
```

- [ ] **Step 4: Run the tests to verify they pass**

Run: `ctest --test-dir build -R '^test_css\.Css\.(ColorMixSupportsMultiItemLists|WptStyleExactCorpus|ColorMixInSrgb|ColorMixNestedAndInvalidForms|ColorMixHueInterpolationKeywords)$' --output-on-failure`

Expected: PASS for all five tests.

- [ ] **Step 5: Commit**

```bash
git add tests/io/test_css.cpp include/colorcpp/io/css/parse_detail.hpp
git commit -m "feat: support multi-item color-mix lists"
```

### Task 3: Progress-Form Parsing

**Files:**
- Modify: `tests/io/test_css.cpp:696-733`
- Modify: `tests/io/test_css.cpp:858-891`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:42-185`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:308-371`
- Test: `tests/io/test_css.cpp`

- [ ] **Step 1: Write the failing tests**

```cpp
TEST(Css, ColorMixSupportsProgressForm) {
  auto simple = parse_css_color_rgbaf("color-mix(25%, red, blue)");
  auto equivalent = parse_css_color_rgbaf("color-mix(red 75%, blue 25%)");
  ASSERT_TRUE(simple);
  ASSERT_TRUE(equivalent);
  expect_rgbaf_near(*simple, equivalent->r(), equivalent->g(), equivalent->b(), equivalent->a(), 0.01f);

  auto method_first = parse_css_color_rgbaf("color-mix(in srgb 25%, red, blue)");
  auto method_equivalent = parse_css_color_rgbaf("color-mix(in srgb, red 75%, blue 25%)");
  ASSERT_TRUE(method_first);
  ASSERT_TRUE(method_equivalent);
  expect_rgbaf_near(*method_first, method_equivalent->r(), method_equivalent->g(), method_equivalent->b(),
                    method_equivalent->a(), 0.01f);

  auto progress_first = parse_css_color_rgbaf("color-mix(25% in srgb, red, blue)");
  ASSERT_TRUE(progress_first);
  expect_rgbaf_near(*progress_first, method_equivalent->r(), method_equivalent->g(), method_equivalent->b(),
                    method_equivalent->a(), 0.01f);
}
```

Add one exact progress-form corpus case:

```cpp
{"color-mix(25% in srgb, red, blue)", 191, 0, 64, 255},
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `ctest --test-dir build -R '^test_css\.Css\.(ColorMixSupportsProgressForm|WptStyleExactCorpus)$' --output-on-failure`

Expected: FAIL because the current parser treats the first `25%` segment as an invalid item instead of a progress header.

- [ ] **Step 3: Write the minimal implementation**

Add a progress header struct:

```cpp
struct color_mix_progress_header {
  color_mix_interpolation_method method{color_mix_space::oklab};
  float progress = 0.5f;
};
```

Add a progress parser that accepts either order for progress and interpolation method:

```cpp
inline std::optional<float> parse_color_mix_progress_value(details::Cursor& c) {
  auto value = c.parse_alpha_value();
  if (!value) return std::nullopt;
  return static_cast<float>(std::clamp(*value, 0.0, 1.0));
}

inline std::optional<color_mix_progress_header> parse_color_mix_progress_header(
    std::string_view s) {
  details::Cursor c{s, 0};
  color_mix_progress_header header;
  bool saw_method = false;
  bool saw_progress = false;

  auto try_parse_method = [&]() -> bool {
    const size_t save = c.i;
    if (!c.consume_ci("in")) {
      c.i = save;
      return false;
    }
    c.skip_ws();
    auto method = parse_color_mix_space(c);
    if (!method) {
      c.i = save;
      return false;
    }
    header.method = *method;
    saw_method = true;
    return true;
  };

  auto try_parse_progress = [&]() -> bool {
    const size_t save = c.i;
    auto value = parse_color_mix_progress_value(c);
    if (!value) {
      c.i = save;
      return false;
    }
    header.progress = *value;
    saw_progress = true;
    return true;
  };

  if (!try_parse_method() && !try_parse_progress()) return std::nullopt;
  if (!saw_method) try_parse_method();
  if (!saw_progress) try_parse_progress();
  c.skip_ws();
  if (!c.eof() || !saw_progress) return std::nullopt;
  return header;
}
```

Normalize the progress form into the shared parsed model:

```cpp
inline std::optional<parsed_color_mix> parse_color_mix_progress_form(
    const std::vector<std::string_view>& parts) {
  if (parts.size() != 3) return std::nullopt;

  auto header = parse_color_mix_progress_header(parts[0]);
  if (!header) return std::nullopt;

  auto first = parse_color_mix_item(parts[1]);
  auto second = parse_color_mix_item(parts[2]);
  if (!first || !second) return std::nullopt;
  if (first->weight || second->weight) return std::nullopt;

  parsed_color_mix parsed;
  parsed.method = header->method;
  parsed.items.push_back(color_mix_item{first->color, 1.0f - header->progress});
  parsed.items.push_back(color_mix_item{second->color, header->progress});
  return parsed;
}
```

Add a shared dispatcher and call it from `parse_color_mix_rgbaf(...)`:

```cpp
inline std::optional<parsed_color_mix> parse_color_mix_arguments(std::string_view inner) {
  auto parts = split_top_level_comma_list(inner);
  if (!parts) return std::nullopt;
  if (parts->size() == 3) {
    if (auto progress = parse_color_mix_progress_form(*parts)) {
      return progress;
    }
  }
  return parse_color_mix_item_list(*parts);
}
```

```cpp
auto parsed = parse_color_mix_arguments(inner);
if (!parsed) {
  c.i = save;
  return std::nullopt;
}
```

- [ ] **Step 4: Run the tests to verify they pass**

Run: `ctest --test-dir build -R '^test_css\.Css\.(ColorMixSupportsProgressForm|WptStyleExactCorpus|ColorMixInSrgb|ColorMixSupportsMultiItemLists)$' --output-on-failure`

Expected: PASS for all four tests.

- [ ] **Step 5: Commit**

```bash
git add tests/io/test_css.cpp include/colorcpp/io/css/parse_detail.hpp
git commit -m "feat: add color-mix progress-form parsing"
```

### Task 4: Negative Coverage And Corpus Alignment

**Files:**
- Modify: `tests/io/test_css.cpp:658-694`
- Modify: `tests/io/test_css.cpp:898-932`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:42-185`
- Modify: `include/colorcpp/io/css/parse_detail.hpp:308-371`
- Test: `tests/io/test_css.cpp`

- [ ] **Step 1: Write the failing tests**

Add a dedicated invalid-extensions test:

```cpp
TEST(Css, ColorMixRejectsMalformedExtendedForms) {
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(25%, red)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb 25%, red)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red,, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red 25% 50%, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb longer hue, red, blue)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(in srgb, red 0%, blue 0%, white 0%)").has_value());
  EXPECT_FALSE(parse_css_color_rgba8("color-mix(25% in srgb, red, blue) trailing").has_value());
}
```

Extend the invalid corpus with new malformed cases:

```cpp
{"color-mix(25%, red)"},
{"color-mix(in srgb 25%, red)"},
{"color-mix(in srgb, red,, blue)"},
{"color-mix(in srgb, red 25% 50%, blue)"},
{"color-mix(in srgb, red 0%, blue 0%, white 0%)"},
{"color-mix(25% in srgb, red, blue) trailing"},
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `ctest --test-dir build -R '^test_css\.Css\.(ColorMixRejectsMalformedExtendedForms|InvalidCorpusRejectsMalformedInputs)$' --output-on-failure`

Expected: FAIL because the new malformed forms are not all rejected yet.

- [ ] **Step 3: Write the minimal implementation**

Tighten `parse_color_mix_item(...)` so bare percentages are not treated as colors:

```cpp
inline bool is_bare_percentage_token(std::string_view s) {
  details::Cursor c{s, 0};
  auto cv = c.parse_component_value();
  if (!cv || !cv->second) return false;
  c.skip_ws();
  return c.eof();
}

inline std::optional<color_mix_item> parse_color_mix_item(std::string_view s) {
  details::trim(s);
  if (s.empty()) return std::nullopt;
  if (is_bare_percentage_token(s)) return std::nullopt;

  if (auto leading = split_optional_leading_percent(s)) {
    return color_mix_item{leading->second, leading->first};
  }
  if (auto trailing = split_optional_trailing_percent(s)) {
    return color_mix_item{trailing->first, trailing->second};
  }
  return color_mix_item{s, std::nullopt};
}
```

Keep header parsing strict so malformed progress headers fail instead of silently falling through:

```cpp
inline std::optional<color_mix_progress_header> parse_color_mix_progress_header(
    std::string_view s) {
  details::Cursor c{s, 0};
  color_mix_progress_header header;
  bool saw_method = false;
  bool saw_progress = false;

  while (!c.eof()) {
    const size_t before = c.i;
    if (!saw_method && c.consume_ci("in")) {
      c.skip_ws();
      auto method = parse_color_mix_space(c);
      if (!method) return std::nullopt;
      header.method = *method;
      saw_method = true;
    } else if (!saw_progress) {
      auto value = parse_color_mix_progress_value(c);
      if (!value) return std::nullopt;
      header.progress = *value;
      saw_progress = true;
    } else {
      return std::nullopt;
    }

    c.skip_ws();
    if (c.i == before) return std::nullopt;
  }

  if (!saw_progress) return std::nullopt;
  return header;
}
```

Leave zero-total handling explicit in `resolve_color_mix_items(...)` so the repository policy is visible:

```cpp
if (total <= 0.0f) return std::nullopt;
```

- [ ] **Step 4: Run the tests to verify they pass**

Run: `ctest --test-dir build -R '^test_css\.' --output-on-failure`

Expected: PASS for the entire `test_css` module.

- [ ] **Step 5: Commit**

```bash
git add tests/io/test_css.cpp include/colorcpp/io/css/parse_detail.hpp
git commit -m "test: tighten color-mix invalid coverage"
```

### Task 5: Documentation And TODO Cleanup

**Files:**
- Modify: `README.md:152-164`
- Modify: `docs/reference/css_color.rst:45-86`
- Modify: `docs/todo.md`
- Test: `tests/io/test_css.cpp`

- [ ] **Step 1: Write the doc-backed regression test**

Keep the progress-form exact corpus entry from Task 3 and add one single-item regression to the exact corpus:

```cpp
{"color-mix(in srgb, red)", 255, 0, 0, 255},
```

This makes the public examples in the docs executable through `WptStyleExactCorpus`.

- [ ] **Step 2: Run the regression test**

Run: `ctest --test-dir build -R '^test_css\.Css\.WptStyleExactCorpus$' --output-on-failure`

Expected: PASS before the docs change, confirming the doc snippets are backed by code.

- [ ] **Step 3: Update the docs**

Update the README capability bullets:

```md
- **`color-mix()`:** omitted method defaults to `oklab`; item-list mixes support one or more colors; progress-form mixes support `color-mix(25% in srgb, red, blue)`; interpolation spaces include `srgb`, `srgb-linear`, `display-p3`, `display-p3-linear`, `lab`, `lch`, `oklab`, `oklch`, and `xyz`
```

Replace the stale pending sentence:

```md
**Still pending:** relative syntaxes beyond `rgb(from …)` / `color(from …)` and the remaining context-sensitive CSS color features that depend on authoring-time style state.
```

Update the reference doc section:

```rst
* ``color-mix()`` - omitted interpolation defaults to ``oklab``; item-list mixes support one or more colors; progress-form mixes support ``color-mix(25% in srgb, red, blue)``; ``lch`` / ``oklch`` also accept ``shorter|longer|increasing|decreasing hue``
```

Update the note block:

```rst
* ``color-mix()`` follows CSS premultiplied-alpha mixing semantics, including reduced-alpha results when explicit percentages sum below ``100%``
* Progress-form ``color-mix()`` inputs are normalized into the same list-based evaluator as the item-list form
```

Mark the `color-mix()` TODO item complete:

```md
- [x] Add the missing multi-item and progress-form `color-mix()` support.
```

- [ ] **Step 4: Run the verification tests**

Run: `ctest --test-dir build -R '^test_css\.' --output-on-failure`

Expected: PASS for the full CSS parser module after the doc update.

- [ ] **Step 5: Commit**

```bash
git add README.md docs/reference/css_color.rst docs/todo.md tests/io/test_css.cpp
git commit -m "docs: document extended color-mix support"
```

## Self-Review

### Spec coverage

- Omitted interpolation method defaulting to `oklab`: covered in Task 1.
- Single-item `color-mix()`: covered in Task 1 and backed by the exact corpus in Task 5.
- Multi-item item-list parsing and left-to-right folded evaluation: covered in Task 2.
- Progress-form parsing, including both `in srgb 25%` and `25% in srgb`: covered in Task 3.
- Strict failure for malformed progress forms, malformed separators, invalid weight placement, unsupported hue-keyword usage, and trailing garbage: covered in Task 4.
- README/reference/TODO alignment: covered in Task 5.

### Placeholder scan

- Checked for `TBD`, `TODO`, “implement later”, “appropriate error handling”, and “similar to Task N”.
- Every code-changing step includes concrete code or exact text to insert.
- Every verification step includes an exact `ctest` command and an expected outcome.

### Type consistency

- `color_mix_item`, `parsed_color_mix`, `resolved_color_mix_item`, and `resolved_color_mix` are introduced in the same names they are referenced later.
- `parse_color_mix_item_list(...)`, `parse_color_mix_progress_form(...)`, `parse_color_mix_arguments(...)`, `resolve_color_mix_items(...)`, and `evaluate_color_mix(...)` are defined once and reused consistently.
- The plan keeps the existing `color_mix_interpolation_method`, `color_mix_space`, `color_mix_hue_method`, `mix_colors_in_space(...)`, and `color_mix_weights` helpers intact so the refactor stays local.
