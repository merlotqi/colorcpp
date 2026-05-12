# IO Binary IO Hardening Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create `feature/io-binary-io-hardening` from updated `main`, fix the confirmed `binary_io` correctness failures in `lut.hpp`, and align the public contract so `apply_color()` and the `.cube`-only format story are truthful and tested.

**Architecture:** Start from a dedicated feature worktree and verify the current `test_binary_io` baseline before touching code. Then use TDD in two passes: first add focused regression tests for `lut3d.size == 1` and `apply_color()` behavior on integer and non-RGB color types, then rework `include/colorcpp/io/binary_io/lut.hpp` so 3D single-cell LUTs bypass interpolation math and all `apply_color()` overloads evaluate through `core::rgbaf_t` via `operations::conversion::color_cast`. Finish by tightening comments and `docs/reference/binary_io.rst` so the module explicitly describes `.cube` as the only implemented format and describes `apply_color()` as conversion-based rather than direct channel mutation.

**Tech Stack:** C++17, GoogleTest/CTest, CMake, header-only `colorcpp`, conversion graph via `operations::conversion::color_cast`, `.worktrees/` git worktree layout.

---

## File Map

- Create: `/home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening`
  - Dedicated feature worktree for branch `feature/io-binary-io-hardening`, based on updated `origin/main`.
- Create: `/home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build`
  - Isolated CMake build directory for the branch.
- Modify: `include/colorcpp/io/binary_io/lut.hpp:1-225`
  - Fix `lut3d.size == 1` behavior, route `apply_color()` through `core::rgbaf_t`, and tighten module comments.
- Modify: `tests/io/test_binary_io.cpp:1-240`
  - Add regression coverage for the two confirmed correctness failures and the conversion-based `apply_color()` contract.
- Modify: `docs/reference/binary_io.rst:1-55`
  - Align the public reference page with the corrected `.cube`-only and conversion-based contract.

## Scope Guard

This plan intentionally does **not** implement `.cube` parser diagnostics yet. It covers only:

- correctness hardening for `lut.hpp`
- tests that prove the repaired behavior
- doc and comment alignment required by that behavior

If additional parser error detail is desired, create a second plan after this branch lands.

### Task 1: Create The Hardening Worktree And Verify The Baseline

**Files:**
- Create: `/home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening`
- Create: `/home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build`
- Read: `tests/io/test_binary_io.cpp`
- Read: `include/colorcpp/io/binary_io/lut.hpp`
- Read: `docs/reference/binary_io.rst`

- [ ] **Step 1: Refresh `origin/main` before branching**

Run:

```bash
git -C /home/merlot/codes/colorspace fetch origin
```

Expected: local refs update so `origin/main` reflects the latest merge base for `feature/io-binary-io-hardening`.

- [ ] **Step 2: Create the isolated feature worktree**

Run:

```bash
git -C /home/merlot/codes/colorspace worktree add /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening -b feature/io-binary-io-hardening origin/main
```

Expected: a new worktree appears at `/home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening` on branch `feature/io-binary-io-hardening`.

If this fails because the branch or worktree already exists, stop and resolve that state before continuing. Do not
reuse an ambiguous tree.

- [ ] **Step 3: Configure the branch with tests enabled**

Run:

```bash
cmake -S /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening -B /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build -DCOLORCPP_BUILD_TESTS=ON
```

Expected: CMake configuration completes successfully and writes build files under the branch-local `build`
directory.

- [ ] **Step 4: Build the focused `binary_io` test target**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build --target test_binary_io
```

Expected: the `test_binary_io` target builds successfully.

- [ ] **Step 5: Verify the current `binary_io` baseline before adding regression tests**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build -R '^test_binary_io\.' --output-on-failure
```

Expected: PASS on the inherited baseline. If this fails, stop and report the baseline failure before adding new
coverage.

### Task 2: Add Failing Regression Tests For The Confirmed Correctness Bugs

**Files:**
- Modify: `tests/io/test_binary_io.cpp:1-240`
- Test: `tests/io/test_binary_io.cpp`

- [ ] **Step 1: Add the missing includes and regression tests**

Update the top of `tests/io/test_binary_io.cpp` so it includes the conversion and HSL types needed by the new
contract tests:

```cpp
#include <colorcpp/core/hsl.hpp>
#include <colorcpp/io/binary_io.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <gtest/gtest.h>

#include <sstream>
#include <string>
```

Then add these tests after `TEST(Lut3d, InvalidLutPassthrough)` and before the `.cube` section:

```cpp
TEST(Lut3d, SingleEntryLutReturnsStoredColor) {
  lut3d lut;
  lut.size = 1;
  lut.data = {{{0.25f, 0.5f, 0.75f}}};

  auto result = apply(lut, 0.3f, 0.4f, 0.5f);
  EXPECT_FLOAT_EQ(result[0], 0.25f);
  EXPECT_FLOAT_EQ(result[1], 0.5f);
  EXPECT_FLOAT_EQ(result[2], 0.75f);
}

TEST(Lut1d, ApplyColorPreservesRgba8Identity) {
  lut1d lut;
  lut.size = 2;
  lut.r = {0.0f, 1.0f};
  lut.g = {0.0f, 1.0f};
  lut.b = {0.0f, 1.0f};

  colorcpp::core::rgba8_t input{255, 32, 0, 128};
  auto output = apply_color(lut, input);

  EXPECT_EQ(output.r(), 255);
  EXPECT_EQ(output.g(), 32);
  EXPECT_EQ(output.b(), 0);
  EXPECT_EQ(output.a(), 128);
}

TEST(Lut1d, ApplyColorSupportsConvertibleNonRgbType) {
  lut1d lut;
  lut.size = 2;
  lut.r = {0.0f, 1.0f};
  lut.g = {0.0f, 1.0f};
  lut.b = {0.0f, 1.0f};

  colorcpp::core::hsl_float_t input{210.0f, 0.75f, 0.40f};
  const auto before = colorcpp::operations::conversion::color_cast<colorcpp::core::rgbaf_t>(input);
  const auto output = apply_color(lut, input);
  const auto after = colorcpp::operations::conversion::color_cast<colorcpp::core::rgbaf_t>(output);

  EXPECT_NEAR(after.r(), before.r(), 1e-5f);
  EXPECT_NEAR(after.g(), before.g(), 1e-5f);
  EXPECT_NEAR(after.b(), before.b(), 1e-5f);
}
```

- [ ] **Step 2: Build and run only the new regression tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build --target test_binary_io
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build -R '^test_binary_io\.(Lut3d\.SingleEntryLutReturnsStoredColor|Lut1d\.ApplyColorPreservesRgba8Identity|Lut1d\.ApplyColorSupportsConvertibleNonRgbType)$' --output-on-failure
```

Expected: FAIL.

Expected failure shape:

- `Lut3d.SingleEntryLutReturnsStoredColor` fails or crashes because the current `lut3d.size == 1` path underflows
  its interpolation indices
- `Lut1d.ApplyColorPreservesRgba8Identity` fails because the current implementation writes normalized float values
  directly into `rgba8_t`, producing `1` instead of `255`
- `Lut1d.ApplyColorSupportsConvertibleNonRgbType` fails to compile or fails at runtime because the current helper
  assumes direct `r()`, `g()`, `b()` access on the original color type

- [ ] **Step 3: Confirm the failure mode before editing `lut.hpp`**

Expected observations:

- the single-entry 3D LUT case is not safe today
- integer RGBA colors are not preserved by identity LUT application
- non-RGB model inputs are not handled by the current direct-channel contract

If the failure output does not match that shape, stop and reassess before implementing the fix.

- [ ] **Step 4: Commit the failing regression tests**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening add tests/io/test_binary_io.cpp
git -C /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening commit -m "test: add binary io hardening regressions"
```

Expected: the branch is intentionally red after this commit until `lut.hpp` is repaired in Task 3.

### Task 3: Repair `lut.hpp` So `apply_color()` Uses `rgbaf_t` And Single-Cell 3D LUTs Are Safe

**Files:**
- Modify: `include/colorcpp/io/binary_io/lut.hpp:1-225`
- Test: `tests/io/test_binary_io.cpp`

- [ ] **Step 1: Update the includes and file header to match the real implementation dependency**

At the top of `include/colorcpp/io/binary_io/lut.hpp`, replace the current include block and file header summary with:

```cpp
/**
 * @file lut.hpp
 * @brief LUT data structures and apply helpers for the current `.cube` workflow.
 *
 * Provides 1D and 3D LUT types plus interpolation helpers used by the current
 * DaVinci Resolve `.cube` reader/writer implementation.
 */

#pragma once

#include <array>
#include <colorcpp/core/rgb.hpp>
#include <colorcpp/operations/conversion.hpp>
#include <cstddef>
#include <string>
#include <vector>
```

This makes `core::rgbaf_t` and `operations::conversion::color_cast` available to the repaired `apply_color()`
implementation and removes the misleading `.csp` hint from the file-level documentation.

- [ ] **Step 2: Special-case `lut3d.size == 1` and route all `apply_color()` overloads through `rgbaf_t`**

Replace the `apply(const lut3d&, ...)` early section and both `apply_color()` overloads with:

```cpp
inline std::array<float, 3> apply(const lut3d& lut, float r, float g, float b) {
  if (!lut.valid()) return {r, g, b};
  if (lut.size == 1) return lut.data.front();

  float tr = details::normalize(r, lut.domain_min[0], lut.domain_max[0]);
  float tg = details::normalize(g, lut.domain_min[1], lut.domain_max[1]);
  float tb = details::normalize(b, lut.domain_min[2], lut.domain_max[2]);

  float idx_r = tr * static_cast<float>(lut.size - 1);
  float idx_g = tg * static_cast<float>(lut.size - 1);
  float idx_b = tb * static_cast<float>(lut.size - 1);

  auto clamp_idx = [sz = lut.size](float idx) -> std::pair<std::size_t, float> {
    auto i = static_cast<std::size_t>(idx);
    if (i >= sz - 1) return {sz - 2, 1.0f};
    return {i, idx - static_cast<float>(i)};
  };

  auto [r0, fr] = clamp_idx(idx_r);
  auto [g0, fg] = clamp_idx(idx_g);
  auto [b0, fb] = clamp_idx(idx_b);

  std::size_t r1 = r0 + 1;
  std::size_t g1 = g0 + 1;
  std::size_t b1 = b0 + 1;

  const auto& c000 = lut.at(r0, g0, b0);
  const auto& c001 = lut.at(r0, g0, b1);
  const auto& c010 = lut.at(r0, g1, b0);
  const auto& c011 = lut.at(r0, g1, b1);
  const auto& c100 = lut.at(r1, g0, b0);
  const auto& c101 = lut.at(r1, g0, b1);
  const auto& c110 = lut.at(r1, g1, b0);
  const auto& c111 = lut.at(r1, g1, b1);

  std::array<float, 3> result{};
  for (int ch = 0; ch < 3; ++ch) {
    float c00 = details::lerp(c000[ch], c100[ch], fr);
    float c01 = details::lerp(c001[ch], c101[ch], fr);
    float c10 = details::lerp(c010[ch], c110[ch], fr);
    float c11 = details::lerp(c011[ch], c111[ch], fr);
    float c0 = details::lerp(c00, c10, fg);
    float c1 = details::lerp(c01, c11, fg);
    result[ch] = details::lerp(c0, c1, fb);
  }

  return result;
}

template <typename Color>
Color apply_color(const lut3d& lut, const Color& c) {
  auto rgba = operations::conversion::color_cast<core::rgbaf_t>(c);
  auto [r, g, b] = apply(lut, rgba.r(), rgba.g(), rgba.b());
  rgba.r() = r;
  rgba.g() = g;
  rgba.b() = b;
  return operations::conversion::color_cast<Color>(rgba);
}

template <typename Color>
Color apply_color(const lut1d& lut, const Color& c) {
  auto rgba = operations::conversion::color_cast<core::rgbaf_t>(c);
  auto [r, g, b] = apply(lut, rgba.r(), rgba.g(), rgba.b());
  rgba.r() = r;
  rgba.g() = g;
  rgba.b() = b;
  return operations::conversion::color_cast<Color>(rgba);
}
```

This keeps the public helper simple while making its behavior match the repaired contract: LUT evaluation happens in
normalized RGB space, and the final value is converted back to the caller's color type.

- [ ] **Step 3: Tighten the `apply_color()` doc comments so they describe conversion-based semantics**

Update the `apply_color()` comments in `lut.hpp` to say:

```cpp
/**
 * @brief Apply a 3D LUT to any color type convertible to and from `core::rgbaf_t`.
 * @details The input color is converted to `core::rgbaf_t`, the LUT is evaluated in normalized RGB space,
 *          and the result is converted back to `Color`.
 */

/**
 * @brief Apply a 1D LUT to any color type convertible to and from `core::rgbaf_t`.
 * @details The input color is converted to `core::rgbaf_t`, the LUT is evaluated in normalized RGB space,
 *          and the result is converted back to `Color`.
 */
```

- [ ] **Step 4: Rebuild and rerun the focused regression tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build --target test_binary_io
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build -R '^test_binary_io\.(Lut3d\.SingleEntryLutReturnsStoredColor|Lut1d\.ApplyColorPreservesRgba8Identity|Lut1d\.ApplyColorSupportsConvertibleNonRgbType)$' --output-on-failure
```

Expected: PASS for all three regression tests.

- [ ] **Step 5: Run the full `test_binary_io` module and commit the repaired implementation**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build -R '^test_binary_io\.' --output-on-failure
git -C /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening add include/colorcpp/io/binary_io/lut.hpp tests/io/test_binary_io.cpp
git -C /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening commit -m "fix: harden binary io lut application"
```

Expected: the entire `test_binary_io` module passes, and the branch now contains the correctness repair commit.

### Task 4: Align The Public Reference Page With The Repaired Contract

**Files:**
- Modify: `docs/reference/binary_io.rst:1-55`
- Read: `include/colorcpp/io/binary_io/lut.hpp`
- Test: `tests/io/test_binary_io.cpp`

- [ ] **Step 1: Rewrite the overview and function bullets to match the repaired contract**

Update `docs/reference/binary_io.rst` so the opening sections read:

```rst
Binary IO (LUT)
================

Overview
--------

Binary IO provides support for reading and writing DaVinci Resolve ``.cube`` LUT files and for applying 1D / 3D LUT
data to normalized RGB values or to color types that can round-trip through ``core::rgbaf_t``.

In colorcpp
-----------

**Headers:** ``include/colorcpp/io/binary_io.hpp``

Data Structures
~~~~~~~~~~~~~~~~

- ``binary_io::lut1d`` — 1D LUT: independent per-channel mapping with domain min/max
- ``binary_io::lut3d`` — 3D LUT: NxNxN grid with trilinear interpolation

Functions
~~~~~~~~~~

- ``binary_io::apply(lut, r, g, b)`` — apply a LUT to normalized RGB values
- ``binary_io::apply_color(lut, color)`` — convert ``color`` through ``core::rgbaf_t``, apply the LUT in normalized RGB space, and convert the result back
- ``binary_io::cube::read_3d(path)`` — read a 3D LUT from a ``.cube`` file
- ``binary_io::cube::read_1d(path)`` — read a 1D LUT from a ``.cube`` file
- ``binary_io::cube::write(path, lut)`` — write a LUT to a ``.cube`` file
```

- [ ] **Step 2: Rewrite the notes section so it no longer implies broader format support**

Replace the current notes block with:

```rst
Notes
-----

- The only implemented file parser/writer in this module is DaVinci Resolve ``.cube``
- ``binary_io::apply()`` and ``binary_io::apply_color()`` evaluate LUTs in normalized RGB space
- ``binary_io::apply_color()`` relies on ``operations::conversion::color_cast`` to convert to and from ``core::rgbaf_t``
- 3D LUT uses trilinear interpolation for sub-grid values, with the single-cell case returning the only stored RGB triplet
- Data ordering is ``R`` slowest, then ``G``, then ``B`` in ``.cube`` storage
```

- [ ] **Step 3: Run the full `binary_io` test module one last time**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening/build -R '^test_binary_io\.' --output-on-failure
```

Expected: PASS. There are no doc tests here, so this final run confirms the code and regression suite are still green
after the contract text was aligned.

- [ ] **Step 4: Commit the contract-alignment documentation**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening add docs/reference/binary_io.rst
git -C /home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening commit -m "docs: align binary io contract"
```

Expected: the branch ends with separate code and doc commits, both green.

## Self-Review Checklist

Before treating this plan as complete, verify:

- the plan does **not** drift into parser diagnostics or new LUT formats
- every modified file listed in the file map appears in at least one task
- the regression tests really prove the two confirmed failures from the approved spec
- the repaired contract consistently says ``.cube``-only plus conversion-through-``rgbaf_t``
- every command path uses `/home/merlot/codes/colorspace/.worktrees/io-binary-io-hardening`
