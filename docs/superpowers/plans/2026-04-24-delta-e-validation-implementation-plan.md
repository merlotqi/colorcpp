# Delta E Validation Branch Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create `feature/delta-e-validation` as a green, mergeable validation branch that adds checked-in Sharma `ΔE2000` vectors plus real reference-order contract tests for `delta_e_94()` and `delta_e_cmc()`, with only minimal documentation clarification.

**Architecture:** Start from a fresh worktree branched from `main`, configure an isolated local build, and verify the existing focused `delta_e` baseline before touching tests. Then strengthen `tests/algorithms/test_delta_e.cpp` in two narrow passes: first with direct `cielab_t` Sharma `ΔE2000` vectors, then by replacing the weak `IsGenerallyAsymmetric` smoke tests with order-sensitive contract checks that use direct LAB pairs and do not require production implementation changes. Finish with a minimal docs note so the public reference page matches the tested asymmetry contract.

**Tech Stack:** C++17, GoogleTest/CTest, CMake, header-only `colorcpp` algorithms, Sharma CIEDE2000 supplementary data, existing `.worktrees/` git worktree layout.

---

## File Map

- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-validation`
  - Dedicated feature worktree on branch `feature/delta-e-validation`, based on `main`.
- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-validation/build`
  - Isolated CMake build directory for the validation branch.
- Modify: `tests/algorithms/test_delta_e.cpp:57-121`
  - Add direct LAB `ΔE2000` published-vector coverage and keep the existing symmetric/property checks.
- Modify: `tests/algorithms/test_delta_e.cpp:68-76`
  - Replace the weak `DeltaE94Test.IsGenerallyAsymmetric` smoke test with a real reference-order contract test.
- Modify: `tests/algorithms/test_delta_e.cpp:182-189`
  - Replace the weak `DeltaECMCTest.IsGenerallyAsymmetric` smoke test with a real reference-order contract test.
- Modify: `docs/reference/delta_e.rst:56-63`
  - Clarify that `ΔE94` and CMC l:c both treat the first argument as the reference color.

## Source Note

This validation plan intentionally avoids `DIN99` implementation or failing `DIN99` strict vectors. That work belongs to the later `feature/delta-e-fixes` branch after this validation branch has been merged through GitHub.

The `ΔE2000` sample values in this plan come from Sharma's supplementary test data and are the same six rows already used in the audit probe:

- `https://hajim.rochester.edu/ece/sites/gsharma/ciede2000/`
- `https://hajim.rochester.edu/ece/sites/gsharma/ciede2000/dataNprograms/ciede2000testdata.txt`

### Task 1: Create The Validation Worktree And Verify The Baseline

**Files:**
- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-validation`
- Create: `/home/merlot/codes/colorspace/.worktrees/delta-e-validation/build`
- Read: `tests/algorithms/test_delta_e.cpp`
- Read: `docs/reference/delta_e.rst`

- [ ] **Step 1: Create the isolated feature worktree**

Run:

```bash
git -C /home/merlot/codes/colorspace worktree add /home/merlot/codes/colorspace/.worktrees/delta-e-validation -b feature/delta-e-validation main
```

Expected: a new worktree appears at `/home/merlot/codes/colorspace/.worktrees/delta-e-validation` on branch `feature/delta-e-validation`.

If this command fails because the branch or worktree already exists, stop and resolve that state before continuing. Do not reuse an ambiguous existing tree.

- [ ] **Step 2: Configure a dedicated local build for the validation branch**

Run:

```bash
cmake -S /home/merlot/codes/colorspace/.worktrees/delta-e-validation -B /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build -DCOLORCPP_BUILD_TESTS=ON
```

Expected: CMake configuration completes successfully and writes build files under `/home/merlot/codes/colorspace/.worktrees/delta-e-validation/build`.

- [ ] **Step 3: Build the focused delta_e test binary**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build --target test_delta_e
```

Expected: the `test_delta_e` target builds successfully.

- [ ] **Step 4: Verify the baseline delta_e suite before modifying tests**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build -R '^test_delta_e\.' --output-on-failure
```

Expected: PASS for the current `test_delta_e` suite on the fresh validation branch. If this fails, stop and report the failure before adding any new coverage.

### Task 2: Add Checked-In Sharma `ΔE2000` Vector Coverage

**Files:**
- Modify: `tests/algorithms/test_delta_e.cpp:87-121`
- Test: `tests/algorithms/test_delta_e.cpp`

- [ ] **Step 1: Add the published `ΔE2000` vector test**

Insert this test after `DeltaE2000Test.IsNonNegative` and before `DeltaE2000Test.PerceivedOrdering`:

```cpp
TEST(DeltaE2000Test, MatchesPublishedSharmaSamples) {
  struct Sample {
    core::cielab_t reference;
    core::cielab_t comparison;
    float expected;
  };

  const Sample samples[] = {
      {core::cielab_t(50.0000f, 2.6772f, -79.7751f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 2.0425f},
      {core::cielab_t(50.0000f, 3.1571f, -77.2803f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 2.8615f},
      {core::cielab_t(50.0000f, 2.8361f, -74.0200f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 3.4412f},
      {core::cielab_t(50.0000f, -1.3802f, -84.2814f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
      {core::cielab_t(50.0000f, -1.1848f, -84.8006f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
      {core::cielab_t(50.0000f, -0.9009f, -85.5211f), core::cielab_t(50.0000f, 0.0000f, -82.7485f), 1.0000f},
  };

  for (const auto& sample : samples) {
    EXPECT_NEAR(delta_e_2000(sample.reference, sample.comparison), sample.expected, 1e-3f);
    EXPECT_NEAR(delta_e_2000(sample.comparison, sample.reference), sample.expected, 1e-3f);
  }
}
```

- [ ] **Step 2: Build and run the focused `ΔE2000` tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build --target test_delta_e
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build -R '^test_delta_e\.DeltaE2000Test\.(MatchesPublishedSharmaSamples|SameColorIsZero|BlackToWhiteIsLarge|IsSymmetric|IsNonNegative|PerceivedOrdering)$' --output-on-failure
```

Expected: PASS. This branch is validation-only, so the current implementation should already satisfy these published Sharma samples.

- [ ] **Step 3: Commit the `ΔE2000` validation upgrade**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-validation add tests/algorithms/test_delta_e.cpp
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-validation commit -m "test: add published delta_e_2000 vectors"
```

### Task 3: Replace Weak Asymmetry Smoke Tests With Real Contract Checks

**Files:**
- Modify: `tests/algorithms/test_delta_e.cpp:68-76`
- Modify: `tests/algorithms/test_delta_e.cpp:182-189`
- Test: `tests/algorithms/test_delta_e.cpp`

- [ ] **Step 1: Replace the weak `ΔE94` asymmetry test**

Replace `DeltaE94Test.IsGenerallyAsymmetric` with:

```cpp
TEST(DeltaE94Test, ReferenceOrderAffectsResult) {
  const core::cielab_t reference(50.0f, 60.0f, 30.0f);
  const core::cielab_t comparison(50.0f, 20.0f, 10.0f);

  const float d_ref_cmp = delta_e_94(reference, comparison);
  const float d_cmp_ref = delta_e_94(comparison, reference);

  EXPECT_GT(d_ref_cmp, 0.0f);
  EXPECT_GT(d_cmp_ref, 0.0f);
  EXPECT_GT(d_cmp_ref - d_ref_cmp, 1.0f);
}
```

- [ ] **Step 2: Replace the weak CMC asymmetry test**

Replace `DeltaECMCTest.IsGenerallyAsymmetric` with:

```cpp
TEST(DeltaECMCTest, ReferenceOrderAffectsResult) {
  const core::cielab_t reference(50.0f, 60.0f, 30.0f);
  const core::cielab_t comparison(50.0f, 20.0f, 10.0f);

  const float d_ref_cmp = delta_e_cmc(reference, comparison);
  const float d_cmp_ref = delta_e_cmc(comparison, reference);

  EXPECT_GT(d_ref_cmp, 0.0f);
  EXPECT_GT(d_cmp_ref, 0.0f);
  EXPECT_GT(d_cmp_ref - d_ref_cmp, 1.0f);
}
```

- [ ] **Step 3: Build and run the strengthened asymmetric-metric tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build --target test_delta_e
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build -R '^test_delta_e\.(DeltaE94Test\.ReferenceOrderAffectsResult|DeltaECMCTest\.ReferenceOrderAffectsResult|DeltaECMCTest\.PerceptibilityVsAcceptability)$' --output-on-failure
```

Expected: PASS. The new tests should prove that argument order matters for the documented reference-color metrics instead of merely asserting non-negativity.

- [ ] **Step 4: Commit the asymmetry contract upgrade**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-validation add tests/algorithms/test_delta_e.cpp
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-validation commit -m "test: enforce asymmetric delta_e contracts"
```

### Task 4: Clarify The Public Asymmetry Note And Re-Run The Focused Suite

**Files:**
- Modify: `docs/reference/delta_e.rst:59-62`
- Test: `tests/algorithms/test_delta_e.cpp`

- [ ] **Step 1: Tighten the reference docs to match the new tests**

Replace this note block:

```rst
* ΔE94 is asymmetric: reference color is the first parameter
* ΔE2000 uses double precision internally for accuracy near neutral axis
* ΔE_OK values are approximately 2.5x smaller scale than ΔE2000
```

with:

```rst
* ΔE94 and CMC l:c are asymmetric: the reference color is the first parameter
* ΔE2000 uses double precision internally for accuracy near neutral axis
* ΔE_OK values are approximately 2.5x smaller scale than ΔE2000
```

- [ ] **Step 2: Rebuild and run the full focused delta_e validation suite**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build --target test_delta_e test_delta_e_simd
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/delta-e-validation/build -R '^(test_delta_e|test_delta_e_simd)\.' --output-on-failure
```

Expected: PASS for all focused `delta_e` and `delta_e_simd` tests. The validation branch must remain green and mergeable.

- [ ] **Step 3: Verify the branch only contains the intended validation changes**

Run:

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-validation status --short
```

Expected: only `docs/reference/delta_e.rst` is modified before the final commit, because the test-file changes were already committed in Tasks 2 and 3.

- [ ] **Step 4: Commit the doc clarification and final validation pass**

```bash
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-validation add docs/reference/delta_e.rst
git -C /home/merlot/codes/colorspace/.worktrees/delta-e-validation commit -m "docs: clarify asymmetric delta_e metrics"
```

## Self-Review

### Spec coverage

- The plan creates the dedicated `feature/delta-e-validation` worktree from `main`.
- The plan adds published Sharma `ΔE2000` vectors to the checked-in test suite.
- The plan upgrades `delta_e_94()` and `delta_e_cmc()` from weak asymmetry smoke tests to real reference-order contract tests.
- The plan keeps `din99.hpp` untouched and leaves strict `DIN99` vectors for the later fixes branch.
- The plan includes the minimal formal-doc clarification allowed by the spec.

### Placeholder scan

- Checked for `TBD`, `TODO`, “implement later”, and other placeholder language.
- Every code-changing step includes the exact code block to insert or replace.
- Every verification step names the exact command and expected result.

### Type consistency

- The plan consistently uses `feature/delta-e-validation` and `.worktrees/delta-e-validation` for the stage-1 branch and worktree.
- The strengthened test names are consistent with the intended contract semantics: `MatchesPublishedSharmaSamples`, `ReferenceOrderAffectsResult`.
- All verification commands target the validation worktree build directory at `/home/merlot/codes/colorspace/.worktrees/delta-e-validation/build`.
