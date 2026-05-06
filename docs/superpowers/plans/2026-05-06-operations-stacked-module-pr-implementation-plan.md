# Operations Stacked Module PR Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Deliver the `operations` audit remediation as six independent module branches and PRs, merged in a stacked order that preserves module isolation while minimizing cross-branch conflicts.

**Architecture:** Treat each `operations` module as its own PR unit, but stack them by dependency and shared-file pressure: `conversion` first, `compare` second, then the lighter truth-alignment passes for `random`, `blend`, `interpolate`, and `palette`. Each branch gets its own worktree under `.worktrees/`, its own local `build/`, and its own targeted tests or acceptance checks. Shared files such as `README.md` and `docs/reference/*.rst` are owned by the earliest module branch that truth-aligns the relevant section.

**Tech Stack:** C++17 header-only library, CMake, GoogleTest/CTest, Sphinx reference docs, git worktrees, stacked PR workflow.

---

## Stack Order

Run all branch-creation commands from `/home/merlot/codes/colorspace`.

| PR Order | Module | Branch | Worktree | PR Base |
| --- | --- | --- | --- | --- |
| 1 | `conversion` | `feature/operations-conversion` | `.worktrees/operations-conversion` | `main` |
| 2 | `compare` | `feature/operations-compare` | `.worktrees/operations-compare` | `feature/operations-conversion` |
| 3 | `random` | `feature/operations-random` | `.worktrees/operations-random` | `feature/operations-compare` |
| 4 | `blend` | `feature/operations-blend` | `.worktrees/operations-blend` | `feature/operations-random` |
| 5 | `interpolate` | `feature/operations-interpolate` | `.worktrees/operations-interpolate` | `feature/operations-blend` |
| 6 | `palette` | `feature/operations-palette` | `.worktrees/operations-palette` | `feature/operations-interpolate` |

## PR Workflow

- Open each PR against the base branch listed above, not directly against `main`.
- Merge PRs in stack order from top to bottom: `conversion`, `compare`, `random`, `blend`, `interpolate`, `palette`.
- After each merge, retarget the next PR to the newly merged branch if your forge supports stacked bases, or rebase the child branch onto the updated parent locally.

After merging `feature/operations-conversion`, refresh the next branch like this before pushing:

```bash
cd /home/merlot/codes/colorspace/.worktrees/operations-compare
git fetch origin
git rebase origin/feature/operations-conversion
git push --force-with-lease
```

Repeat the same pattern for each subsequent child branch:

```bash
cd /home/merlot/codes/colorspace/.worktrees/operations-<child-module>
git fetch origin
git rebase origin/feature/operations-<parent-module>
git push --force-with-lease
```

## Shared File Ownership Rules

- `README.md` is edited in multiple PRs, but each PR touches only the subsection that belongs to its module.
- `docs/reference/conversion.rst` belongs only to the conversion PR.
- `docs/reference/compare.rst` belongs only to the compare PR.
- `docs/reference/random.rst` belongs only to the random PR.
- `docs/reference/blend.rst` belongs only to the blend PR.
- `docs/reference/interpolate.rst` belongs only to the interpolate PR.
- `docs/reference/palette.rst` belongs only to the palette PR.
- Header file comment blocks are updated in the same PR that owns the module.
- Tests under `tests/operations/` are updated only by the PR that owns the corresponding module file.

## File Map

- `conversion` PR
  - Modify: `include/colorcpp/operations/conversion.hpp`
  - Modify: `include/colorcpp/operations/conversion/debug.hpp`
  - Modify: `include/colorcpp/operations/conversion/traits.hpp`
  - Modify: `include/colorcpp/operations/conversion/color_space_registry.hpp`
  - Modify: `tests/operations/test_conversion.cpp`
  - Modify: `docs/reference/conversion.rst`
  - Modify: `README.md`

- `compare` PR
  - Modify: `include/colorcpp/operations/compare/perceptual.hpp`
  - Modify: `include/colorcpp/operations/compare/equal.hpp`
  - Modify: `include/colorcpp/operations/compare.hpp`
  - Modify: `tests/operations/test_compare.cpp`
  - Modify: `docs/reference/compare.rst`
  - Modify: `examples/compare_example.cpp`

- `random` PR
  - Modify: `include/colorcpp/operations/random.hpp`
  - Modify: `docs/reference/random.rst`
  - Modify: `README.md`

- `blend` PR
  - Modify: `include/colorcpp/operations/blend.hpp`
  - Modify: `docs/reference/blend.rst`
  - Modify: `README.md`
  - Modify: `examples/blend_example.cpp`

- `interpolate` PR
  - Modify: `include/colorcpp/operations/interpolate.hpp`
  - Modify: `docs/reference/interpolate.rst`
  - Modify: `README.md`

- `palette` PR
  - Modify: `include/colorcpp/operations/palette.hpp`
  - Modify: `include/colorcpp/operations/palette/material.hpp`
  - Modify: `include/colorcpp/operations/palette/theme.hpp`
  - Modify: `docs/reference/palette.rst`
  - Modify: `README.md`

### Task 1: Conversion PR `feature/operations-conversion`

**Files:**
- Modify: `include/colorcpp/operations/conversion.hpp`
- Modify: `include/colorcpp/operations/conversion/debug.hpp`
- Modify: `include/colorcpp/operations/conversion/traits.hpp`
- Modify: `include/colorcpp/operations/conversion/color_space_registry.hpp`
- Modify: `tests/operations/test_conversion.cpp`
- Modify: `docs/reference/conversion.rst`
- Modify: `README.md`
- Test: `tests/operations/test_conversion.cpp`

- [ ] **Step 1: Create the worktree and configure the local build**

```bash
cd /home/merlot/codes/colorspace
git check-ignore -q .worktrees
git worktree add .worktrees/operations-conversion -b feature/operations-conversion main
cd .worktrees/operations-conversion
cmake -S . -B build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
cmake --build build --target test_conversion -j4
```

- [ ] **Step 2: Write the failing contract test for the graph-based public debug story**

Append this test near the weighted-routing assertions in `tests/operations/test_conversion.cpp`:

```cpp
TEST(ConversionDebugContractTest, PublicDebugHelpersReflectGraphRouting) {
  static_assert(can_convert<rgbf_t, oklab_t>());
  static_assert(verify_path<rgbf_t, oklab_t>());

  using info = conversion_path_info<rgbf_t, oklab_t>;
  static_assert(info::is_identity == false);
  static_assert(info::has_direct_conversion == false);
  static_assert(info::has_graph_path);
  static_assert(info::minimal_graph_cost == 2);
  static_assert(info::is_possible);
}
```

- [ ] **Step 3: Run the test to verify it fails**

Run: `cmake --build build --target test_conversion -j4`

Expected: FAIL at compile time because `conversion.hpp` does not currently expose `can_convert`, `verify_path`, or `conversion_path_info` through the documented public include, and `conversion_path_info` does not currently provide `has_graph_path` / `minimal_graph_cost`.

- [ ] **Step 4: Write the minimal implementation and truth-alignment docs**

Expose debug helpers from the documented public conversion header in `include/colorcpp/operations/conversion.hpp`:

```cpp
#include <colorcpp/operations/conversion/cast_impl.hpp>
#include <colorcpp/operations/conversion/color_space_registry.hpp>
#include <colorcpp/operations/conversion/debug.hpp>
#include <colorcpp/operations/conversion/detail.hpp>
#include <colorcpp/operations/conversion/functions/index.hpp>
#include <colorcpp/operations/conversion/registry.hpp>
```

Rewrite the public `conversion_path_info` contract in `include/colorcpp/operations/conversion/debug.hpp` so it reports graph semantics instead of mixed graph/hub semantics:

```cpp
template <typename From, typename To>
struct conversion_path_info {
  using from_type = From;
  using to_type = To;

  static constexpr bool is_identity = std::is_same_v<From, To>;
  static constexpr bool has_direct_conversion = has_registered_conversion_v<From, To>;
  static constexpr std::size_t minimal_graph_cost = graph::minimal_conversion_cost<From, To>();
  static constexpr bool has_graph_path = minimal_graph_cost != graph::inf;
  static constexpr bool is_possible = is_identity || has_direct_conversion || has_graph_path;
};
```

Retitle the comments in `include/colorcpp/operations/conversion/traits.hpp` and `include/colorcpp/operations/conversion/color_space_registry.hpp` so they stop claiming public hub-based dispatch. Use wording like:

```cpp
/**
 * @file traits.hpp
 * @brief Conversion metadata traits and compatibility helpers.
 *
 * Public routing is graph-driven through color_cast and graph.hpp.
 * The helpers in this file describe model relationships and support
 * compatibility calculations; they are not the primary dispatch contract.
 */
```

Replace the top architecture section of `docs/reference/conversion.rst` with graph-only wording:

```rst
**System features**:

  * ✅ **Compile time safety**: Unsupported conversions fail at compile time with clear static_assert messages
  * ✅ **Compile-time graph routing**: Full weighted shortest-path routing runs during compilation
  * ✅ **Weighted edges**: Expensive conversions can be assigned higher cost for optimal path selection
  * ✅ **Extensible**: Add new color spaces externally without modifying core library
  * ✅ **constexpr support**: All conversions can be evaluated at compile time
  * ✅ **Zero runtime overhead**: All abstractions resolve directly to function calls with zero indirection
  * ✅ **Compile-time debugging**: Inspect graph reachability, path cost, and registration state
```

Replace the routing section with:

```rst
colorcpp uses a graph-based routing system, evaluated in order:

1. **Identity conversion (cost 0)**
2. **Direct registered conversion (registered edge cost)**
3. **Global graph shortest path (weighted multi-hop route)**
4. **Compile-time error if no graph path exists**
```

Update the debug examples in `docs/reference/conversion.rst` to remove `get_hub_t` and use the new field names:

```rst
    constexpr bool possible = colorcpp::operations::conversion::can_convert<From, To>();
    using info = colorcpp::operations::conversion::conversion_path_info<From, To>;
    static_assert(info::has_graph_path);
    static_assert(info::minimal_graph_cost < colorcpp::operations::conversion::graph::inf);
```

Update the conversion section in `README.md` so the cross-space note stays truthful:

```md
// Cross-space conversions (automatically graph-routed)
auto lab_to_ok = color_cast<oklab_t>(lab_color);  // graph chooses the lowest-cost registered route
```

- [ ] **Step 5: Run the tests and verify they pass**

Run: `cmake --build build --target test_conversion -j4 && ctest --test-dir build -R '^test_conversion\.' --output-on-failure`

Expected: PASS, including `test_conversion.ConversionDebugContractTest.PublicDebugHelpersReflectGraphRouting`.

- [ ] **Step 6: Commit**

```bash
git add include/colorcpp/operations/conversion.hpp \
        include/colorcpp/operations/conversion/debug.hpp \
        include/colorcpp/operations/conversion/traits.hpp \
        include/colorcpp/operations/conversion/color_space_registry.hpp \
        tests/operations/test_conversion.cpp \
        docs/reference/conversion.rst \
        README.md
git commit -m "refactor: align conversion with graph routing contract"
```

### Task 2: Compare PR `feature/operations-compare`

**Files:**
- Modify: `include/colorcpp/operations/compare/perceptual.hpp`
- Modify: `include/colorcpp/operations/compare/equal.hpp`
- Modify: `include/colorcpp/operations/compare.hpp`
- Modify: `tests/operations/test_compare.cpp`
- Modify: `docs/reference/compare.rst`
- Modify: `examples/compare_example.cpp`
- Test: `tests/operations/test_compare.cpp`

- [ ] **Step 1: Create the worktree from the conversion branch and configure the build**

```bash
cd /home/merlot/codes/colorspace
git worktree add .worktrees/operations-compare -b feature/operations-compare feature/operations-conversion
cd .worktrees/operations-compare
cmake -S . -B build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
cmake --build build --target test_compare compare_example -j4
```

- [ ] **Step 2: Write the failing cross-type perceptual comparison test**

Add this block after the current perceptual tests in `tests/operations/test_compare.cpp`:

```cpp
TEST(PerceptualCompareTest, CrossTypeDeltaE2000Comparison) {
  rgb8_t a{128, 64, 32};
  rgbaf_t b{128.0f / 255.0f, 64.0f / 255.0f, 32.0f / 255.0f, 1.0f};

  EXPECT_TRUE(perceptual_equal(a, b, 0.05f));
  EXPECT_TRUE(perceptual_equal_de2000(a, b, 0.05f));
  EXPECT_TRUE((equal<perceptual_policy>(a, b, 0.05f)));
}
```

- [ ] **Step 3: Run the test to verify it fails**

Run: `cmake --build build --target test_compare -j4`

Expected: FAIL at compile time because `perceptual_equal` and `equal<perceptual_policy>` currently require both operands to have the same `Color` type, and `perceptual_equal_de2000` does not yet exist.

- [ ] **Step 4: Write the minimal implementation and update the public wording**

Generalize the policy overload in `include/colorcpp/operations/compare/equal.hpp`:

```cpp
template <typename Policy, typename ColorA, typename ColorB, typename... Args>
constexpr auto equal(const ColorA& a, const ColorB& b, Args&&... args)
    noexcept(noexcept(Policy::compare(a, b, std::forward<Args>(args)...)))
    -> decltype(Policy::compare(a, b, std::forward<Args>(args)...)) {
  return Policy::compare(a, b, std::forward<Args>(args)...);
}
```

Make perceptual comparison explicitly ΔE2000-based and cross-type in `include/colorcpp/operations/compare/perceptual.hpp`:

```cpp
struct perceptual_policy {
  template <typename ColorA, typename ColorB>
  static bool compare(const ColorA& a, const ColorB& b, float threshold = 1.0f) {
    return algorithms::delta_e::delta_e_2000(a, b) < threshold;
  }
};

template <typename ColorA, typename ColorB>
bool perceptual_equal_de2000(const ColorA& a, const ColorB& b, float threshold = 1.0f) {
  return perceptual_policy::compare(a, b, threshold);
}

template <typename ColorA, typename ColorB>
bool perceptual_equal(const ColorA& a, const ColorB& b, float threshold = 1.0f) {
  return perceptual_equal_de2000(a, b, threshold);
}
```

Clarify the header comment in `include/colorcpp/operations/compare.hpp`:

```cpp
 * - perceptual: Comparison using ΔE2000 perceptual distance via CIELAB conversion
```

Replace the misleading note in `docs/reference/compare.rst` with:

```rst
Perceptual comparison in `compare::perceptual_policy` currently uses `algorithms::delta_e::delta_e_2000()`,
which converts through CIELAB and applies a ΔE2000 threshold.

`perceptual_equal()` and `compare::equal<compare::perceptual_policy>(...)` support mixed color types as long as
both inputs are convertible by `operations::conversion::color_cast`.
```

Update `examples/compare_example.cpp` to demonstrate cross-type perceptual comparison directly:

```cpp
  rgb8_t ui_rgb{128, 64, 32};
  rgbaf_t converted_rgb{128.0f / 255.0f, 64.0f / 255.0f, 32.0f / 255.0f, 1.0f};
  std::cout << "  perceptual_equal_de2000(rgb8, rgbaf, 0.05): "
            << compare::perceptual_equal_de2000(ui_rgb, converted_rgb, 0.05f) << '\n';
```

- [ ] **Step 5: Run the tests and example build**

Run: `cmake --build build --target test_compare compare_example -j4 && ctest --test-dir build -R '^test_compare\.' --output-on-failure`

Expected: PASS, including `test_compare.PerceptualCompareTest.CrossTypeDeltaE2000Comparison`.

- [ ] **Step 6: Commit**

```bash
git add include/colorcpp/operations/compare/perceptual.hpp \
        include/colorcpp/operations/compare/equal.hpp \
        include/colorcpp/operations/compare.hpp \
        tests/operations/test_compare.cpp \
        docs/reference/compare.rst \
        examples/compare_example.cpp
git commit -m "feat: make perceptual compare explicit and cross-type"
```

### Task 3: Random PR `feature/operations-random`

**Files:**
- Modify: `include/colorcpp/operations/random.hpp`
- Modify: `docs/reference/random.rst`
- Modify: `README.md`
- Test: `docs/reference/random.rst`

- [ ] **Step 1: Create the worktree from the compare branch and configure the build**

```bash
cd /home/merlot/codes/colorspace
git worktree add .worktrees/operations-random -b feature/operations-random feature/operations-compare
cd .worktrees/operations-random
cmake -S . -B build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
cmake --build build --target test_random test_random_constrained -j4
```

- [ ] **Step 2: Write the failing documentation acceptance check**

Run this check from the worktree root:

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/random.rst").read_text()
header = Path("include/colorcpp/operations/random.hpp").read_text()
readme = Path("README.md").read_text()

assert "Capability families" in doc
assert "Constrained generators" in doc
assert "Perceptual-space sampling caveats" in doc
assert "basic random generation" in header
assert "constrained random generation" in header
assert "Random colors in different spaces" in readme and "contrast" in readme
PY
```

- [ ] **Step 3: Verify the acceptance check fails**

Run: the same `python` block from Step 2.

Expected: FAIL with `AssertionError` because the current doc and header text do not yet expose the planned capability-family taxonomy.

- [ ] **Step 4: Write the minimal implementation**

Add a taxonomy section to `docs/reference/random.rst`:

```rst
Capability families
-------------------

* **Basic uniform generators** — channel-box sampling with no gamut guarantee
* **Constrained generators** — rejection-sampled outputs that target luminance or contrast constraints
* **Perceptual-space sampling caveats** — OkLab, CIELAB, OkLCH, and CIELCH sampling uses axis-aligned boxes and may
  leave common RGB display gamuts
```

Clarify the aggregate header comment in `include/colorcpp/operations/random.hpp`:

```cpp
 * @brief Aggregate header for basic random generation, constrained random generation,
 *        and harmony-oriented random helpers.
```

Expand the random section in `README.md`:

```md
// Basic random colors
auto random_rgb = random_color<rgbf_t>();

// Constrained random colors
auto accessible = random_contrast_color<rgbf_t>(core::rgbf_t{1.0f, 1.0f, 1.0f}, 4.5f, seed);
auto mid_light = random_luminance_color<rgbf_t>(0.4f, 0.6f, seed);
```

- [ ] **Step 5: Run the acceptance check and the random tests**

Run:

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/random.rst").read_text()
header = Path("include/colorcpp/operations/random.hpp").read_text()
readme = Path("README.md").read_text()

assert "Capability families" in doc
assert "Constrained generators" in doc
assert "Perceptual-space sampling caveats" in doc
assert "basic random generation" in header
assert "constrained random generation" in header
assert "random_contrast_color" in readme
assert "random_luminance_color" in readme
PY
ctest --test-dir build -R '^(test_random|test_random_constrained)\.' --output-on-failure
```

Expected: PASS for both the acceptance check and the random test executables.

- [ ] **Step 6: Commit**

```bash
git add include/colorcpp/operations/random.hpp \
        docs/reference/random.rst \
        README.md
git commit -m "docs: classify random generator capability families"
```

### Task 4: Blend PR `feature/operations-blend`

**Files:**
- Modify: `include/colorcpp/operations/blend.hpp`
- Modify: `docs/reference/blend.rst`
- Modify: `README.md`
- Modify: `examples/blend_example.cpp`
- Test: `docs/reference/blend.rst`

- [ ] **Step 1: Create the worktree from the random branch and configure the build**

```bash
cd /home/merlot/codes/colorspace
git worktree add .worktrees/operations-blend -b feature/operations-blend feature/operations-random
cd .worktrees/operations-blend
cmake -S . -B build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
cmake --build build --target test_blend test_blend_simd blend_example -j4
```

- [ ] **Step 2: Write the failing documentation acceptance check**

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/blend.rst").read_text()
header = Path("include/colorcpp/operations/blend.hpp").read_text()
example = Path("examples/blend_example.cpp").read_text()

assert "Current implementation note" in doc
assert "encoded `rgbaf_t` working space" in doc
assert "current encoded-sRGB-style implementation" in header
assert "implementation note" in example
PY
```

- [ ] **Step 3: Verify the acceptance check fails**

Run: the same `python` block from Step 2.

Expected: FAIL with `AssertionError` because the current blend docs and comments still describe the module as linearized-sRGB compositing without the implementation caveat.

- [ ] **Step 4: Write the minimal implementation**

Replace the opening paragraph of `docs/reference/blend.rst` with:

```rst
Compositing combines a **source** and **destination** color with alpha. The current colorcpp implementation converts
inputs through ``rgbaf_t`` and applies the blend formulas there. Treat this as the current encoded ``rgbaf_t`` working
space contract, not as a verified linear-sRGB compositing claim.
```

Add an explicit note block:

```rst
Current implementation note
---------------------------

* The public implementation currently works in encoded ``rgbaf_t`` space.
* The documentation therefore describes the module as an encoded working-space implementation, even though a future
  follow-up may introduce a true linear-sRGB policy.
```

Update the aggregate header comment in `include/colorcpp/operations/blend.hpp`:

```cpp
/** @brief Alpha-aware blending with selectable blend mode; current implementation uses an encoded `rgbaf_t`
 *         working space rather than a verified linear-sRGB compositing path. */
```

Add a note near the top of `examples/blend_example.cpp`:

```cpp
// implementation note: examples reflect the current encoded-sRGB-style blend path described in docs/reference/blend.rst
```

Update the README blend section by inserting:

```md
Current note: the shipped `blend()` implementation currently evaluates through `rgbaf_t`; do not read the examples as
proof of a linear-sRGB compositing contract.
```

- [ ] **Step 5: Run the acceptance check and blend tests**

Run:

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/blend.rst").read_text()
header = Path("include/colorcpp/operations/blend.hpp").read_text()
example = Path("examples/blend_example.cpp").read_text()
readme = Path("README.md").read_text()

assert "Current implementation note" in doc
assert "encoded ``rgbaf_t`` working space" in doc
assert "encoded `rgbaf_t` working space" in header
assert "implementation note" in example
assert "currently evaluates through `rgbaf_t`" in readme
PY
ctest --test-dir build -R '^(test_blend|test_blend_simd)\.' --output-on-failure
```

Expected: PASS for the acceptance check and both blend test executables.

- [ ] **Step 6: Commit**

```bash
git add include/colorcpp/operations/blend.hpp \
        docs/reference/blend.rst \
        README.md \
        examples/blend_example.cpp
git commit -m "docs: align blend contract with current working space"
```

### Task 5: Interpolate PR `feature/operations-interpolate`

**Files:**
- Modify: `include/colorcpp/operations/interpolate.hpp`
- Modify: `docs/reference/interpolate.rst`
- Modify: `README.md`
- Test: `docs/reference/interpolate.rst`

- [ ] **Step 1: Create the worktree from the blend branch and configure the build**

```bash
cd /home/merlot/codes/colorspace
git worktree add .worktrees/operations-interpolate -b feature/operations-interpolate feature/operations-blend
cd .worktrees/operations-interpolate
cmake -S . -B build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
cmake --build build --target test_interpolate interpolate_example -j4
```

- [ ] **Step 2: Write the failing documentation acceptance check**

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/interpolate.rst").read_text()
header = Path("include/colorcpp/operations/interpolate.hpp").read_text()
readme = Path("README.md").read_text()

assert "Semantic grouping" in doc
assert "Working space families" in doc
assert "Alpha handling" in doc
assert "gamut strategy" in doc
assert "space-aware interpolation families" in header
assert "Perceptual interpolation via OkLab" in readme and "Multi-stop path interpolation" in readme
PY
```

- [ ] **Step 3: Verify the acceptance check fails**

Run: the same `python` block from Step 2.

Expected: FAIL with `AssertionError` because the current interpolate docs list helpers, but do not yet organize them around semantic axes.

- [ ] **Step 4: Write the minimal implementation**

Add a semantic grouping section to `docs/reference/interpolate.rst`:

```rst
Semantic grouping
-----------------

Working space families
^^^^^^^^^^^^^^^^^^^^^^

* **RGB-style interpolation** — ``lerp()``
* **Cylindrical hue-aware interpolation** — ``lerp_hsl()``, ``lerp_hsv()``, ``lerp_oklch()``, ``lerp_lch()``
* **Perceptual cartesian interpolation** — ``lerp_oklab()``, ``lerp_lab()``
* **Curve/path helpers** — cubic, Catmull-Rom, monotonic spline, and path helpers layered on the same space choices

Alpha handling
^^^^^^^^^^^^^^

Most interpolation helpers recover alpha separately from ``rgbaf_t`` after color interpolation. Treat that as part of
the current contract when selecting helpers.

gamut strategy
^^^^^^^^^^^^^^

Interpolation helpers do not promise automatic display-gamut preservation. Use explicit gamut utilities when that
behavior matters.
```

Update the aggregate header comment in `include/colorcpp/operations/interpolate.hpp`:

```cpp
/** @brief Space-aware interpolation families: RGB-style, hue-aware cylindrical, perceptual, and path/spline helpers. */
```

Add one explanatory sentence to the README interpolation section:

```md
Choose interpolation helpers by semantic family first: RGB-style, hue-aware cylindrical, perceptual, or path/spline.
```

- [ ] **Step 5: Run the acceptance check and interpolate tests**

Run:

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/interpolate.rst").read_text()
header = Path("include/colorcpp/operations/interpolate.hpp").read_text()
readme = Path("README.md").read_text()

assert "Semantic grouping" in doc
assert "Working space families" in doc
assert "Alpha handling" in doc
assert "gamut strategy" in doc
assert "Space-aware interpolation families" in header
assert "semantic family first" in readme
PY
ctest --test-dir build -R '^test_interpolate\.' --output-on-failure
```

Expected: PASS for the acceptance check and `test_interpolate`.

- [ ] **Step 6: Commit**

```bash
git add include/colorcpp/operations/interpolate.hpp \
        docs/reference/interpolate.rst \
        README.md
git commit -m "docs: regroup interpolate APIs by semantic family"
```

### Task 6: Palette PR `feature/operations-palette`

**Files:**
- Modify: `include/colorcpp/operations/palette.hpp`
- Modify: `include/colorcpp/operations/palette/material.hpp`
- Modify: `include/colorcpp/operations/palette/theme.hpp`
- Modify: `docs/reference/palette.rst`
- Modify: `README.md`
- Test: `docs/reference/palette.rst`

- [ ] **Step 1: Create the worktree from the interpolate branch and configure the build**

```bash
cd /home/merlot/codes/colorspace
git worktree add .worktrees/operations-palette -b feature/operations-palette feature/operations-interpolate
cd .worktrees/operations-palette
cmake -S . -B build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
cmake --build build --target test_palette palette_example -j4
```

- [ ] **Step 2: Write the failing documentation acceptance check**

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/palette.rst").read_text()
header = Path("include/colorcpp/operations/palette.hpp").read_text()
material = Path("include/colorcpp/operations/palette/material.hpp").read_text()
theme = Path("include/colorcpp/operations/palette/theme.hpp").read_text()
readme = Path("README.md").read_text()

assert "Structural palette APIs" in doc
assert "Heuristic design-system helpers" in doc
assert "heuristic" in material
assert "heuristic" in theme
assert "structural palette APIs" in header
assert "heuristic theme and material helpers" in readme
PY
```

- [ ] **Step 3: Verify the acceptance check fails**

Run: the same `python` block from Step 2.

Expected: FAIL with `AssertionError` because the current palette docs and comments do not yet separate structural palette tools from heuristic design-system helpers.

- [ ] **Step 4: Write the minimal implementation**

Insert the following section in `docs/reference/palette.rst` after the feature list:

```rst
Structural palette APIs
-----------------------

* ``schemes::*`` harmony generation
* scale builders such as ``linear_scale()``, ``visual_scale()``, and ``perceptual_scale()``
* families and rainbow helpers

Heuristic design-system helpers
-------------------------------

* ``material_*`` helpers are lightweight heuristic approximations, not a standards-faithful Material token pipeline
* ``theme`` derives semantic colors heuristically from a seed and should be treated as an evolvable design helper
```

Update the aggregate header comment in `include/colorcpp/operations/palette.hpp`:

```cpp
/** @brief structural palette APIs plus heuristic theme and material helpers built on interpolate and harmony rules. */
```

Clarify the comments in `include/colorcpp/operations/palette/material.hpp` and `include/colorcpp/operations/palette/theme.hpp`:

```cpp
 * @brief Heuristic Material-style palette generator.
```

```cpp
 * @brief Heuristic semantic theme system derived from a seed color.
```

Add one sentence to the README palette section:

```md
`schemes`, `families`, and scale builders are structural palette APIs; `material_*` and `theme` are heuristic design helpers.
```

- [ ] **Step 5: Run the acceptance check and palette tests**

Run:

```bash
python - <<'PY'
from pathlib import Path

doc = Path("docs/reference/palette.rst").read_text()
header = Path("include/colorcpp/operations/palette.hpp").read_text()
material = Path("include/colorcpp/operations/palette/material.hpp").read_text()
theme = Path("include/colorcpp/operations/palette/theme.hpp").read_text()
readme = Path("README.md").read_text()

assert "Structural palette APIs" in doc
assert "Heuristic design-system helpers" in doc
assert "heuristic" in material
assert "heuristic" in theme
assert "structural palette APIs plus heuristic" in header
assert "heuristic design helpers" in readme
PY
ctest --test-dir build -R '^test_palette\.' --output-on-failure
```

Expected: PASS for the acceptance check and `test_palette`.

- [ ] **Step 6: Commit**

```bash
git add include/colorcpp/operations/palette.hpp \
        include/colorcpp/operations/palette/material.hpp \
        include/colorcpp/operations/palette/theme.hpp \
        docs/reference/palette.rst \
        README.md
git commit -m "docs: separate structural and heuristic palette APIs"
```

## Self-Review

### Spec coverage

- `conversion` and `compare` are handled first and receive the only code-bearing foundation changes in this stack.
- `random`, `blend`, `interpolate`, and `palette` each get their own branch, worktree, docs, and validation path.
- The stacked PR order matches the approved architecture priority: foundation first, then the higher-level truth-alignment branches.
- Shared file ownership is explicitly assigned so later tasks do not fight over the same sections accidentally.

### Placeholder scan

- No unresolved placeholders or hand-wavy implementation instructions remain.
- Every task includes exact branch names, worktree paths, files, commands, and expected outcomes.
- Every implementation step includes the concrete text or code to add.

### Type consistency

- Conversion task uses `has_graph_path` and `minimal_graph_cost` consistently across test, docs, and debug helper code.
- Compare task uses `perceptual_equal_de2000` consistently across tests, implementation, and example text.
- Later tasks use acceptance checks only for their own module docs/comments and do not reference undefined helper names.
