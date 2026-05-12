# IO Literals Correctness Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create `feature/io-literals-correctness` from updated `main`, repair the confirmed non-CSS literal correctness bugs, and align the public literal contract so docs, examples, and headers describe the same supported surface.

**Architecture:** Start from a dedicated feature worktree and verify the current `test_literals` baseline before touching code. Then use TDD to add focused regressions for `_hex` invalid-digit rejection and `_oklch` chroma scaling, repair the parsing/helpers in `include/colorcpp/io/literals`, and finally align the public docs/examples around an explicit “common authoring spaces only” policy. Because the core layer does not currently define `oklaba_t` or `oklcha_t`, this cleanup wave narrows the documented Oklab/OkLCH surface instead of inventing new alpha-bearing literal types.

**Tech Stack:** C++17, GoogleTest/CTest, header-only `colorcpp`, user-defined literals, CMake, `.worktrees/` git worktree layout.

---

## File Map

- Create: `/home/merlot/codes/colorspace/.worktrees/io-literals-correctness`
  - Dedicated feature worktree for branch `feature/io-literals-correctness`, based on updated `origin/main`.
- Create: `/home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build`
  - Isolated CMake build directory for the branch.
- Modify: `include/colorcpp/io/literals/details.hpp:1-92`
  - Add strict `_hex` digit validation so malformed runtime string literals throw instead of coercing bad digits to zero.
- Modify: `include/colorcpp/io/literals/oklab.hpp:1-64`
  - Fix `_oklch` chroma scaling and remove unsupported alpha-form documentation drift from the Oklab/OkLCH literal header.
- Modify: `tests/io/test_literals.cpp:1-360`
  - Add regression tests for `_hex` invalid digits and `_oklch` max-chroma mapping; add a small Oklab/OkLCH happy-path coverage block so the repaired surface stays protected.
- Modify: `include/colorcpp/io/literals.hpp:1-14`
  - Tighten the aggregate-header summary so it explicitly describes the supported literal families as a curated common-authoring-space surface.
- Modify: `docs/reference/literals.rst:1-80`
  - Align the reference page with the repaired `_hex` and Oklab/OkLCH contract, and document the intentional coverage policy.
- Modify: `README.md`
  - Correct the literal feature summary and example comments so they reflect actual stored ranges and the supported Oklab/OkLCH surface.
- Modify: `examples/io_example.cpp`
  - Keep the user-facing literals example truthful by fixing stored-value comments and adding Oklab/OkLCH examples that match the repaired contract.

## Scope Guard

This plan intentionally does **not**:

- add new literal families such as LAB/LCH, Display P3, or XYZ
- add new core alpha-bearing Oklab/OkLCH types
- broaden `_hex` into a flexible parser beyond `#RGB`, `#RGBA`, `#RRGGBB`, `#RRGGBBAA`
- touch CSS named-color literals or CSS string parsing

If literal-surface expansion is still desired after this branch lands, write a second plan on top of the repaired and documented contract.

### Task 1: Create The Literals Worktree And Verify The Baseline

**Files:**
- Create: `/home/merlot/codes/colorspace/.worktrees/io-literals-correctness`
- Create: `/home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build`
- Read: `tests/io/test_literals.cpp`
- Read: `include/colorcpp/io/literals/details.hpp`
- Read: `include/colorcpp/io/literals/oklab.hpp`
- Read: `docs/reference/literals.rst`
- Read: `README.md`
- Read: `examples/io_example.cpp`

- [ ] **Step 1: Refresh `origin/main` before branching**

Run:

```bash
git -C /home/merlot/codes/colorspace fetch origin
```

Expected: local refs update so `origin/main` reflects the latest merge base for `feature/io-literals-correctness`.

- [ ] **Step 2: Create the isolated feature worktree**

Run:

```bash
git -C /home/merlot/codes/colorspace worktree add /home/merlot/codes/colorspace/.worktrees/io-literals-correctness -b feature/io-literals-correctness origin/main
```

Expected: a new worktree appears at `/home/merlot/codes/colorspace/.worktrees/io-literals-correctness` on branch `feature/io-literals-correctness`.

If this fails because the branch or worktree already exists, stop and resolve that ambiguity before continuing.

- [ ] **Step 3: Configure the branch with tests and examples enabled**

Run:

```bash
cmake -S /home/merlot/codes/colorspace/.worktrees/io-literals-correctness -B /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
```

Expected: CMake configuration completes successfully and writes build files under the branch-local `build` directory.

- [ ] **Step 4: Build the focused literals test target and example target**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build --target test_literals io_example
```

Expected: both `test_literals` and `io_example` build successfully on the inherited baseline.

- [ ] **Step 5: Verify the current literals baseline before adding regressions**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build -R '^test_literals\.' --output-on-failure
```

Expected: PASS on the inherited baseline. If this fails, stop and report the baseline failure before adding new coverage.

### Task 2: Add Failing Regression Tests For The Confirmed Literal Bugs

**Files:**
- Modify: `tests/io/test_literals.cpp:1-360`
- Test: `tests/io/test_literals.cpp`

- [ ] **Step 1: Add invalid-hex and Oklab/OkLCH regression coverage**

Update `tests/io/test_literals.cpp` in two places.

After `TEST(HexLiteralTest, InvalidLengthThrows)`, insert:

```cpp
TEST(HexLiteralTest, InvalidHexDigitsThrow) {
  EXPECT_THROW((void)"#GG0000"_hex, std::invalid_argument);
  EXPECT_THROW((void)"#12X"_hex, std::invalid_argument);
  EXPECT_THROW((void)"#F63Z"_hex, std::invalid_argument);
}
```

Then, after `TEST(HsvaLiteralTest, FullyTransparent)`, insert:

```cpp
TEST(OklabLiteralTest, NeutralMidpoint) {
  constexpr auto lab = 050'050'050_oklab;
  static_assert(lab.l() == 0.5f);
  EXPECT_FLOAT_EQ(lab.a(), 0.0f);
  EXPECT_FLOAT_EQ(lab.b(), 0.0f);
}

TEST(OklchLiteralTest, MaxChromaMapsToPointFour) {
  auto lch = 050'100'120_oklch;
  EXPECT_FLOAT_EQ(lch.l(), 0.5f);
  EXPECT_FLOAT_EQ(lch.c(), 0.4f);
  EXPECT_FLOAT_EQ(lch.h(), 120.0f);
}
```

The Oklab test is expected to pass on both old and new code; it exists to keep the repaired section from staying effectively untested once the regression work lands.

- [ ] **Step 2: Build and run only the new regression tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build --target test_literals
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build -R '^test_literals\.(HexLiteralTest\.InvalidHexDigitsThrow|OklchLiteralTest\.MaxChromaMapsToPointFour)$' --output-on-failure
```

Expected: FAIL.

Expected failure shape:

- `HexLiteralTest.InvalidHexDigitsThrow` fails because `_hex` currently coerces invalid digits to zero instead of rejecting them
- `OklchLiteralTest.MaxChromaMapsToPointFour` fails because `_oklch` currently maps `C=100` to `1.0`, which exceeds the `core::oklch_t` chroma limit and throws `std::out_of_range`

- [ ] **Step 3: Confirm the observed failures before editing the implementation**

Expected observations:

- malformed `_hex` input is still being accepted today
- the `_oklch` literal contract is incompatible with the core type range

If the failures do not match that shape, stop and reassess before writing the production fix.

- [ ] **Step 4: Commit the failing regression tests**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/io-literals-correctness add tests/io/test_literals.cpp
git -C /home/merlot/codes/colorspace/.worktrees/io-literals-correctness commit -m "test: add literal correctness regressions"
```

Expected: the branch is intentionally red after this commit until Task 3 repairs the literal helpers.

### Task 3: Repair `_hex` Validation And The Oklab/OkLCH Literal Contract

**Files:**
- Modify: `include/colorcpp/io/literals/details.hpp:1-92`
- Modify: `include/colorcpp/io/literals/oklab.hpp:1-64`
- Modify: `tests/io/test_literals.cpp:1-360`
- Test: `tests/io/test_literals.cpp`

- [ ] **Step 1: Add strict `_hex` digit validation in `details.hpp`**

Update `include/colorcpp/io/literals/details.hpp` so malformed `_hex` input is rejected before any channel values are constructed.

Introduce these helpers above `hex_expand()`:

```cpp
constexpr bool is_hex_digit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

constexpr void validate_hex_chars(const char* s, size_t offset, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    if (!is_hex_digit(s[offset + i])) {
      throw std::invalid_argument("colorcpp: invalid hex digit in _hex literal");
    }
  }
}
```

Then call the validator at the top of `parse_hex_string()` right after `offset` and `len` are computed:

```cpp
constexpr core::rgba8_t parse_hex_string(const char* s, size_t n) {
  size_t offset = (n > 0 && s[0] == '#') ? 1 : 0;
  size_t len = n - offset;

  validate_hex_chars(s, offset, len);

  auto get_val = [&](size_t i) { return char_to_hex(s[offset + i]); };
  // existing length-switch logic stays in place below
}
```

Keep `char_to_hex()` itself unchanged so the compile-time numeric literal code paths are not disturbed.

- [ ] **Step 2: Fix `_oklch` scaling and remove unsupported alpha-form claims**

Update `include/colorcpp/io/literals/oklab.hpp` so the documented examples match the implementation that actually exists.

Replace the Oklab comment block with:

```cpp
/**
 * @name Oklab Literal Operators
 *
 * Supported forms: three-channel Oklab literals only.
 *
 * Encoding: val = L * 1'000'000 + A * 1'000 + B
 *   - L: 0–100 (percentage, 0–1 in storage), A/B: 000–100 (mapped to -0.5–0.5)
 * Example: 050'050'050_oklab → oklab_t{0.50f, 0.0f, 0.0f}
 * @{
 */
```

Replace the OkLCH comment block and `_oklch` return statement with:

```cpp
/**
 * @name OkLCH Literal Operators
 *
 * Supported forms: three-channel OkLCH literals only.
 *
 * Encoding: val = L * 1'000'000 + C * 1'000 + H
 *   - L: 0–100 (percentage, 0–1 in storage), C: 000–100 (mapped to 0.0–0.4), H: 0–360 (degrees)
 * Example: 050'100'120_oklch → oklch_t{0.50f, 0.40f, 120.0f}
 * @{
 */

template <char... Chars>
constexpr auto operator""_oklch() {
  constexpr uint64_t val = details::parse_dec_template<Chars...>();
  constexpr uint64_t l = val / 1000000ULL;
  constexpr uint64_t c = (val / 1000ULL) % 1000ULL;
  constexpr uint64_t h = val % 1000ULL;
  static_assert(l <= 100, "colorcpp: _oklch L out of range (000–100)");
  static_assert(c <= 100, "colorcpp: _oklch C out of range (000–100)");
  static_assert(h <= 360, "colorcpp: _oklch H out of range (0–360)");
  return core::oklch_t{static_cast<float>(l) / 100.0f, static_cast<float>(c) * 0.4f / 100.0f,
                       static_cast<float>(h)};
}
```

This task deliberately narrows the documented surface instead of inventing `_oklaba` / `_oklcha`, because there are no corresponding public core alpha types to return.

- [ ] **Step 3: Rebuild and rerun the focused regressions**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build --target test_literals
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build -R '^test_literals\.(HexLiteralTest\.InvalidHexDigitsThrow|OklchLiteralTest\.MaxChromaMapsToPointFour|OklabLiteralTest\.NeutralMidpoint)$' --output-on-failure
```

Expected: PASS for all three targeted tests.

- [ ] **Step 4: Run the full literals module and commit the repaired implementation**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build -R '^test_literals\.' --output-on-failure
git -C /home/merlot/codes/colorspace/.worktrees/io-literals-correctness add include/colorcpp/io/literals/details.hpp include/colorcpp/io/literals/oklab.hpp tests/io/test_literals.cpp
git -C /home/merlot/codes/colorspace/.worktrees/io-literals-correctness commit -m "fix: harden io literals contract"
```

Expected: the full `test_literals` module passes and the branch now contains the behavior repair commit.

### Task 4: Align The Aggregate Header, Reference Docs, README, And Example Program

**Files:**
- Modify: `include/colorcpp/io/literals.hpp:1-14`
- Modify: `docs/reference/literals.rst:1-80`
- Modify: `README.md`
- Modify: `examples/io_example.cpp`
- Test: `tests/io/test_literals.cpp`
- Build: `examples/io_example.cpp`

- [ ] **Step 1: Tighten the aggregate-header summary and the reference page**

Update `include/colorcpp/io/literals.hpp` to:

```cpp
/**
 * @file literals.hpp
 * @brief Aggregate header for the supported common-authoring-space color literal operators.
 *
 * Includes RGB/RGBA, HSL/HSLA, HSV/HSVA, HWB/HWBA, CMYK, Oklab, and OkLCH literal operators.
 */
```

Then rewrite the opening sections of `docs/reference/literals.rst` so they read:

```rst
Color Literals
==============

User-defined C++ literals for convenient inline color creation in the supported common authoring spaces.

In colorcpp
-----------

Header path: ``<colorcpp/io/literals.hpp>``

Import namespace: ``using namespace colorcpp::io::literals;``

Available literal suffixes:

* ``_rgb`` / ``_rgba`` / ``_argb`` / ``_hex`` - RGBA8-oriented hex literals
* ``_hsl`` / ``_hsla`` - HSL colors
* ``_hsv`` / ``_hsva`` - HSV colors
* ``_hwb`` / ``_hwba`` - HWB colors
* ``_cmyk`` - CMYK colors
* ``_oklab`` - three-channel Oklab perceptual colors
* ``_oklch`` - three-channel OkLCH perceptual colors
```

- [ ] **Step 2: Rewrite the literals notes and examples so they tell the truth**

Replace the `Notes` block in `docs/reference/literals.rst` with:

```rst
Notes
-----

Numeric literal families are constexpr-friendly and use packed numeric groups documented in the header comments.

``_hex`` validates runtime string input and throws ``std::invalid_argument`` on malformed length or invalid digits.

The literals layer is intentionally curated around common authoring spaces; not every public core color model has a literal suffix.

Oklab / OkLCH currently expose three-channel literal forms only.
```

Then update the literal examples in `README.md` so the feature bullet and snippet become:

```md
- **Type-safe literals**: User-defined literals for RGB, HSL, HSV, HWB, CMYK, OkLab, OkLCH, and named colors
```

```cpp
using namespace colorcpp::io::literals;

// RGB/Hex literals
auto coral = 0xFF6347_rgb;              // -> rgba8_t{255, 99, 71, 255}
auto with_alpha = 0xFF634780_rgba;      // -> rgba8_t{255, 99, 71, 128}
auto argb = 0x80FF6347_argb;            // -> rgba8_t{255, 99, 71, 128} (AARRGGBB)
auto from_hex = "#FF6347"_hex;          // -> rgba8_t

// HSL / HSV literals
auto mint = 160'070'080_hsl;            // -> hsl_float_t{160.0f, 0.70f, 0.80f}
auto sky = 210'080'090_hsv;             // -> hsv_float_t{210.0f, 0.80f, 0.90f}

// CMYK / Oklab / OkLCH literals
auto teal = 50'030'000'020_cmyk;        // -> cmyk8_t{50, 30, 0, 20}
auto neutral = 050'050'050_oklab;       // -> oklab_t{0.50f, 0.0f, 0.0f}
auto vivid = 050'100'120_oklch;         // -> oklch_t{0.50f, 0.40f, 120.0f}
```

Finally, update the literal section of `examples/io_example.cpp` so the declarations and output include:

```cpp
  auto lit_hsl = 120'050'075_hsl;        // hsl_float_t{120.0f, 0.50f, 0.75f}
  auto lit_hsla = 120'050'075'100_hsla;  // hsla_float_t{120.0f, 0.50f, 0.75f, 1.0f}
  auto lit_hsv = 210'080'090_hsv;        // hsv_float_t{210.0f, 0.80f, 0.90f}
  auto lit_hsva = 210'080'090'075_hsva;  // hsva_float_t{210.0f, 0.80f, 0.90f, 0.75f}
  auto lit_cmyk = 50'030'000'020_cmyk;   // cmyk8_t{50, 30, 0, 20}
  auto lit_oklab = 050'050'050_oklab;    // oklab_t{0.50f, 0.0f, 0.0f}
  auto lit_oklch = 050'100'120_oklch;    // oklch_t{0.50f, 0.40f, 120.0f}
```

and the corresponding output lines:

```cpp
  std::cout << "050'050'050_oklab:      ";
  print_swatch(std::cout, lit_oklab, 4);
  std::cout << " " << lit_oklab << "\n";
  std::cout << "050'100'120_oklch:      ";
  print_swatch(std::cout, lit_oklch, 4);
  std::cout << " " << lit_oklch << "\n";
```

- [ ] **Step 3: Rebuild the example and rerun the literals module**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build --target io_example
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-literals-correctness/build -R '^test_literals\.' --output-on-failure
```

Expected: `io_example` builds successfully and the full `test_literals` module stays green after the public-surface text and example changes.

- [ ] **Step 4: Commit the doc/example alignment**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/io-literals-correctness add include/colorcpp/io/literals.hpp docs/reference/literals.rst README.md examples/io_example.cpp
git -C /home/merlot/codes/colorspace/.worktrees/io-literals-correctness commit -m "docs: align literal docs and examples"
```

Expected: the branch ends with separate behavior and documentation/example commits, both green.

## Self-Review Checklist

Before treating this plan as complete, verify:

- the plan does **not** drift into new literal-family expansion or new alpha-bearing Oklab core types
- the failing tests directly prove the two confirmed bugs from the approved spec: `_hex` invalid-digit acceptance and `_oklch` chroma scaling
- the implementation task narrows unsupported `_oklaba` / `_oklcha` claims instead of inventing unsupported return types
- the docs/examples consistently describe literal coverage as a curated common-authoring-space surface
- every command path uses `/home/merlot/codes/colorspace/.worktrees/io-literals-correctness`
