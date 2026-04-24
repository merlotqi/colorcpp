# Delta E Fixes Branch Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create `feature/delta-e-fixes` from the merged validation baseline, add strict DIN99 conformance coverage that initially fails, repair `delta_e_din99()` to match the published DIN99 transform and parameter semantics, and finish with public docs that match the repaired behavior.

**Architecture:** Start from the GitHub-merged validation result on `origin/main`, create a dedicated fixes worktree, and verify the strengthened `delta_e` baseline before touching DIN99. Then use TDD in two narrow passes: first add direct `cielab_t` DIN99 reference vectors and scaling-parameter checks derived from the published DIN99 equations, then rewrite `include/colorcpp/algorithms/delta_e/din99.hpp` so the internal transform uses the rotated `e/f` axes, the `0.7` anisotropy factor, and `k_E` as an inverse overall scale instead of an in-transform multiplier. Finish by updating the public reference page so the checked-in docs describe the repaired and tested DIN99 behavior without overclaiming.

**Tech Stack:** C++17, GoogleTest/CTest, CMake, header-only `colorcpp` algorithms, primary DIN99 equations from Cui et al. / Witt, `.worktrees/` git worktree layout, GitHub-merged `feature/delta-e-validation` baseline.

---

## File Map

- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-fixes`
  - Dedicated feature worktree on branch `feature/delta-e-fixes`, based on the merged validation result from `origin/main`.
- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build`
  - Isolated CMake build directory for the fixes branch.
- Modify: `include/colorcpp/algorithms/delta_e/din99.hpp:1-104`
  - Replace the current DIN99-inspired transform with the published rotated-axis DIN99 transform and correct `k_L` / `k_E` distance scaling semantics.
- Modify: `tests/algorithms/test_delta_e.cpp:248-285`
  - Add strict DIN99 equation-derived conformance vectors and parameter-scaling assertions that fail against the inherited implementation and pass after the repair.
- Modify: `docs/reference/delta_e.rst:17-26`
  - Tighten the public DIN99 wording so it reflects the repaired transform and checked-in conformance coverage.

## Source Note

This fixes plan uses the published DIN99 transform reviewed in the audit:

- Cui et al., *Uniform colour spaces based on the DIN99 colour-difference formula*
- ResearchGate mirror used in the audit: `https://www.researchgate.net/publication/229891006_Uniform_colour_spaces_based_on_the_DIN99_colour-difference_formula`

The plan treats the DIN99 equations as:

- `L99 = 105.51 * ln(1 + 0.0158 * L*)`
- `e = a* cos(16°) + b* sin(16°)`
- `f = 0.7 * (b* cos(16°) - a* sin(16°))`
- `G = sqrt(e^2 + f^2)`
- `C99 = ln(1 + 0.045 * G) / 0.045`
- `a99 = C99 * e / G`, `b99 = C99 * f / G` when `G > 0`
- `ΔE99 = (1 / k_E) * sqrt((ΔL99 / k_L)^2 + Δa99^2 + Δb99^2)`

The reference values in Task 2 are equation-derived direct `cielab_t` cases computed from those formulas. They are intentionally chosen to expose the current implementation's missing `e/f` anisotropy step and its incorrect `k_E` scaling.

### Task 1: Create The Fixes Worktree And Verify The Merged Baseline

**Files:**
- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-fixes`
- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build`
- Read: `tests/algorithms/test_delta_e.cpp`
- Read: `include/colorcpp/algorithms/delta_e/din99.hpp`
- Read: `docs/reference/delta_e.rst`

- [ ] **Step 1: Refresh the merged main branch from GitHub**

Run:

```bash
git -C /home/merlot/codes/colorspace fetch origin
```

Expected: `origin/main` advances to the merge commit that already contains `feature/delta-e-validation`.

- [ ] **Step 2: Create the isolated fixes worktree from the merged baseline**

Run:

```bash
git -C /home/merlot/codes/colorspace worktree add /home/merlot/codes/colorspace/.worktrees/delta-e-fixes -b feature/delta-e-fixes origin/main
```

Expected: a new worktree appears at `/home/merlot/codes/colorspace/.worktrees/delta-e-fixes` on branch `feature/delta-e-fixes`, tracking the merged validation baseline.

If this fails because the branch or worktree already exists, stop and resolve that state before continuing. Do not reuse an ambiguous existing tree.

- [ ] **Step 3: Configure the fixes branch with tests enabled**

Run:

```bash
cmake -S /home/merlot/codes/colorspace/.worktrees/delta-e-fixes -B /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build -DCOLORCPP_BUILD_TESTS=ON
```

Expected: CMake configuration completes successfully, including any `googletest` setup required by `FetchContent`, and writes build files under `/home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build`.

- [ ] **Step 4: Build the focused DIN99-related test binaries**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build --target test_delta_e test_delta_e_simd
```

Expected: both test targets build successfully.

- [ ] **Step 5: Verify the strengthened delta_e baseline before adding DIN99 strict checks**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build -R '^(test_delta_e|test_delta_e_simd)\.' --output-on-failure
```

Expected: PASS for the merged validation baseline. If this fails, stop and report the failure before adding new DIN99 coverage.

### Task 2: Add Strict DIN99 Conformance Coverage That Exposes The Current Failure Mode

**Files:**
- Modify: `tests/algorithms/test_delta_e.cpp:248-285`
- Test: `tests/algorithms/test_delta_e.cpp`

- [ ] **Step 1: Add equation-derived DIN99 reference vectors**

Insert these tests after `DeltaEDIN99Test.IsNonNegative` and before `DeltaEDIN99Test.IsVisuallySameDin99`:

```cpp
TEST(DeltaEDIN99Test, MatchesEquationDerivedReferencePairs) {
  struct Sample {
    const char* label;
    core::cielab_t reference;
    core::cielab_t comparison;
    float expected;
  };

  const Sample samples[] = {
      {"neutral_to_positive_b", core::cielab_t(50.0f, 0.0f, 0.0f), core::cielab_t(50.0f, 0.0f, 10.0f), 6.2908f},
      {"orthogonal_hue_swap", core::cielab_t(50.0f, 30.0f, 0.0f), core::cielab_t(50.0f, 0.0f, 30.0f), 21.7733f},
      {"mixed_lightness_chroma", core::cielab_t(20.0f, 40.0f, 10.0f), core::cielab_t(80.0f, -20.0f, -30.0f), 70.6783f},
      {"small_hue_rotation", core::cielab_t(50.0f, 5.0f, 0.0f), core::cielab_t(50.0f, 0.0f, 5.0f), 5.0306f},
  };

  for (const auto& sample : samples) {
    SCOPED_TRACE(sample.label);
    EXPECT_NEAR(delta_e_din99(sample.reference, sample.comparison), sample.expected, 1e-3f);
    EXPECT_NEAR(delta_e_din99(sample.comparison, sample.reference), sample.expected, 1e-3f);
  }
}

TEST(DeltaEDIN99Test, ScalingParametersMatchPublishedEquation) {
  const core::cielab_t mixed_reference(20.0f, 40.0f, 10.0f);
  const core::cielab_t mixed_comparison(80.0f, -20.0f, -30.0f);
  const core::cielab_t light_reference(20.0f, 0.0f, 0.0f);
  const core::cielab_t light_comparison(80.0f, 0.0f, 0.0f);

  EXPECT_NEAR(delta_e_din99(mixed_reference, mixed_comparison, 2.0f, 1.0f), 50.3771f, 1e-3f);
  EXPECT_NEAR(delta_e_din99(light_reference, light_comparison, 1.0f, 2.0f), 28.6215f, 1e-3f);
}
```

- [ ] **Step 2: Build and run only the DIN99 tests to capture the inherited failure**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build --target test_delta_e
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build -R '^test_delta_e\.DeltaEDIN99Test\.' --output-on-failure
```

Expected: FAIL. The inherited implementation should overshoot the `neutral_to_positive_b`, `orthogonal_hue_swap`, and `small_hue_rotation` reference pairs because it skips the rotated `e/f` anisotropy term, and it should fail the `k_E` assertion badly because it multiplies by `k_E` instead of dividing by it.

- [ ] **Step 3: Inspect the failure output and confirm the failure mode before repairing code**

Expected observations from the failing run:

- the default-parameter DIN99 cases with strong `b*` or rotated-hue content are larger than the published-equation expectations
- the `k_E = 2.0f` case is off by an order of magnitude instead of halving the distance

If the failures do not match that shape, stop and reassess before editing `din99.hpp`.

- [ ] **Step 4: Commit the failing DIN99 conformance tests**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-fixes add tests/algorithms/test_delta_e.cpp
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-fixes commit -m "test: add strict din99 conformance vectors"
```

Expected: the branch is intentionally red after this commit until Task 3 repairs the implementation.

### Task 3: Repair `delta_e_din99()` To Match The Published DIN99 Transform

**Files:**
- Modify: `include/colorcpp/algorithms/delta_e/din99.hpp:1-104`
- Test: `tests/algorithms/test_delta_e.cpp`

- [ ] **Step 1: Replace the DIN99 helper and distance calculation with the published transform**

Update `include/colorcpp/algorithms/delta_e/din99.hpp` so the transform helper no longer accepts `k_L` / `k_E`, uses the rotated `e/f` coordinates, and applies `k_E` only as an inverse scale on the final distance:

```cpp
namespace details {
/**
 * @brief Transform CIELAB to DIN99 coordinates.
 *
 * Implements the published DIN99 transform:
 * - lightness compression to L99
 * - 16-degree rotation into intermediate e/f axes
 * - 0.7 compression on the rotated blue-yellow axis
 * - logarithmic chroma compression before Euclidean distance in DIN99 space
 */
inline void lab_to_din99(double L, double a, double b, double& L99, double& a99, double& b99) {
  constexpr double kCos16 = 0.9612616959383189;
  constexpr double kSin16 = 0.27563735581699916;

  L99 = 105.51 * std::log(1.0 + 0.0158 * L);

  const double e = a * kCos16 + b * kSin16;
  const double f = 0.7 * (b * kCos16 - a * kSin16);
  const double G = std::sqrt(e * e + f * f);

  if (G == 0.0) {
    a99 = 0.0;
    b99 = 0.0;
    return;
  }

  const double C99 = std::log(1.0 + 0.045 * G) / 0.045;
  const double scale = C99 / G;
  a99 = scale * e;
  b99 = scale * f;
}
}  // namespace details

template <typename ColorA, typename ColorB>
float delta_e_din99(const ColorA& a, const ColorB& b, float k_L = 1.0f, float k_E = 1.0f) {
  using namespace operations::conversion;

  auto la = color_cast<core::cielab_t>(a);
  auto lb = color_cast<core::cielab_t>(b);

  const double L1 = la.template get_index<0>();
  const double a1 = la.template get_index<1>();
  const double b1 = la.template get_index<2>();
  const double L2 = lb.template get_index<0>();
  const double a2 = lb.template get_index<1>();
  const double b2 = lb.template get_index<2>();

  double L99_1, a99_1, b99_1;
  double L99_2, a99_2, b99_2;

  details::lab_to_din99(L1, a1, b1, L99_1, a99_1, b99_1);
  details::lab_to_din99(L2, a2, b2, L99_2, a99_2, b99_2);

  const double dL99 = (L99_2 - L99_1) / static_cast<double>(k_L);
  const double da99 = a99_2 - a99_1;
  const double db99 = b99_2 - b99_1;

  return static_cast<float>((1.0 / static_cast<double>(k_E)) * std::sqrt(dL99 * dL99 + da99 * da99 + db99 * db99));
}
```

While making this change, also rewrite the top-of-file comments so they describe the rotated `e/f` axes and inverse `k_E` scaling instead of the old simplified "hue rotation plus chroma compression" wording.

- [ ] **Step 2: Rebuild and rerun the DIN99-only tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build --target test_delta_e
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build -R '^test_delta_e\.DeltaEDIN99Test\.' --output-on-failure
```

Expected: PASS. The new strict DIN99 conformance tests should now pass alongside the existing symmetry, non-negativity, and helper-threshold checks.

- [ ] **Step 3: Commit the DIN99 implementation repair**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-fixes add include/colorcpp/algorithms/delta_e/din99.hpp
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-fixes commit -m "fix: align delta_e_din99 with published transform"
```

### Task 4: Update The Public DIN99 Wording And Run Final Verification

**Files:**
- Modify: `docs/reference/delta_e.rst:17-26`
- Test: `tests/algorithms/test_delta_e.cpp`
- Test: `tests/algorithms/test_delta_e_simd.cpp`

- [ ] **Step 1: Tighten the public DIN99 description to match the repaired implementation**

Replace this DIN99 bullet:

```rst
    * ``delta_e_din99()`` - DIN99 (DIN 6176)
        - German standard with improved perceptual uniformity
        - Applies non-linear transforms to CIELAB
```

with:

```rst
    * ``delta_e_din99()`` - DIN99 (DIN 6176)
        - Euclidean distance in the DIN99-transformed LAB space
        - Supports ``k_L`` lightness scaling and inverse ``k_E`` overall scaling
        - Checked against equation-derived reference pairs from the published DIN99 transform
```

- [ ] **Step 2: Rebuild and run the full focused delta_e suite**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build --target test_delta_e test_delta_e_simd
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build -R '^(test_delta_e|test_delta_e_simd)\.' --output-on-failure
```

Expected: PASS for all focused `delta_e` and `delta_e_simd` tests, including the newly strengthened DIN99 coverage.

- [ ] **Step 3: Run the full repository test suite on the repaired branch**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-fixes/build --output-on-failure
```

Expected: PASS for the full test suite. This catches any aggregate-header or unrelated regression introduced by the DIN99 header rewrite.

- [ ] **Step 4: Verify the branch only contains the intended DIN99 fixes**

Run:

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-fixes status --short
```

Expected: only `docs/reference/delta_e.rst` is modified before the final commit, because the test and implementation changes were already committed in Tasks 2 and 3.

- [ ] **Step 5: Commit the doc refresh and final verification state**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-fixes add docs/reference/delta_e.rst
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-fixes commit -m "docs: refresh din99 reference wording"
```

## Self-Review

### Spec coverage

- The plan creates `feature/delta-e-fixes` from the GitHub-merged validation baseline.
- The plan adds strict DIN99 equation-derived vectors that are expected to fail against the inherited implementation.
- The plan explicitly captures the current failure mode before editing production code.
- The plan repairs `include/colorcpp/algorithms/delta_e/din99.hpp` with the rotated-axis transform and corrected `k_E` semantics.
- The plan updates `docs/reference/delta_e.rst` so public DIN99 wording matches the repaired and tested behavior.

### Placeholder scan

- No `TODO`, `TBD`, or deferred "implement later" language remains.
- Every code-changing step includes exact code blocks or exact replacement text.
- Every verification step includes explicit commands and expected outcomes.

### Type consistency

- The tests use the existing public API names: `delta_e_din99()` and `is_visually_same_din99()`.
- The implementation keeps the public `delta_e_din99(..., float k_L = 1.0f, float k_E = 1.0f)` signature intact.
- The doc changes describe `k_L` and `k_E` using the same names exposed by the public API.
