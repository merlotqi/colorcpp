# Delta E Module Audit

## Scope

This report audits the current `delta_e` module implementation from the `main` worktree and stores the resulting analysis on the `ai/superpowers` branch.

Audited source areas:

- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de76.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de94.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de2000.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/cmc.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/din99.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/oklab.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/simd.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/helpers.hpp`
- `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp`
- `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp`
- `/home/merlot/codes/colorspace/docs/reference/delta_e.rst`

## Baseline Verification

Focused local verification passed before the deeper audit began.

Commands run:

- `cmake --build /home/merlot/codes/colorspace/build --target test_delta_e test_delta_e_simd`
- `ctest --test-dir /home/merlot/codes/colorspace/build -R '^(test_delta_e|test_delta_e_simd)\.' --output-on-failure`

Observed result:

- The focused `delta_e` and `delta_e_simd` tests passed in the current local build before formula review and reference comparison.

## Executive Summary

## Metric-By-Metric Findings

### `delta_e_76`

#### Implementation observations

No implementation mismatch was found in `de76.hpp`. The function converts both inputs to `core::cielab_t`, computes `dL`, `da`, and `db`, and returns `sqrt(dL^2 + da^2 + db^2)`, which is the expected CIE 1976 `ΔE*ab` Euclidean distance in LAB space. The implementation is intentionally minimal, symmetric, and does not introduce weighting terms or branchy edge-case handling beyond the upstream color conversion.

#### Reference comparison

The implementation structure matches the standard CIE 1976 definition: Euclidean distance in CIELAB coordinates with no chroma- or hue-specific correction terms. This task did not add an external sample-data probe for `delta_e_76`, so the conclusion here is a structural formula match rather than a published-vector conformance check.

#### Test adequacy

Current tests cover the basic invariants that matter most for this formula: identical colors return approximately zero, black-to-white is about `100`, the metric is symmetric, the result is non-negative, and mixed-type inputs compile and run. That is a reasonable baseline for such a small implementation. The main gap is that the tests mostly exercise RGB-to-LAB conversion indirectly rather than asserting a few hand-constructed LAB-space numeric examples, so they are better at catching gross regressions than subtle conversion or arithmetic drift.

#### Risk assessment

Risk is low. The formula is simple, the implementation matches that formula directly, and the current tests cover the core invariants. The remaining risk is mostly around indirect dependencies such as `color_cast<core::cielab_t>` rather than the `delta_e_76` arithmetic itself.

### `delta_e_94`

#### Implementation observations

No implementation mismatch was found in `de94.hpp`. The function uses the first argument as the reference color, computes `C1`, `C2`, `dL`, `dC`, and `dH^2 = da^2 + db^2 - dC^2`, clamps `dH^2` at zero to avoid small negative values from float rounding, and applies the usual `S_L`, `S_C`, and `S_H` weighting terms with configurable `kL`, `kC`, `kH`, `K1`, and `K2`. The code comment and parameter docs explicitly mark the formula as asymmetric in `a` and `b`, which is important because this is a reference-versus-comparison metric rather than a fully symmetric distance.

#### Reference comparison

The implementation structure matches the standard CIE 1994 graphic-arts form: `S_L = 1`, `S_C = 1 + K1*C1`, `S_H = 1 + K2*C1`, with the chroma-dependent weighting driven by the reference color. No structural mismatch was found in that review. This task did not include a published numerical conformance probe for `delta_e_94`, so confidence here comes from formula inspection rather than sample-data confirmation.

#### Test adequacy

The documentation explains the asymmetry better than the tests do. `docs/reference/delta_e.rst` includes a direct note that `ΔE94` is asymmetric and that the first parameter is the reference color, and the function-level docs in `de94.hpp` repeat that contract. By contrast, `DeltaE94Test.IsGenerallyAsymmetric` does not actually assert asymmetry; despite its name, it only checks that both call orders are non-negative. The rest of the tests cover zero and "large difference" behavior, but they do not pin a known numeric example or verify that swapping arguments can change the result.

#### Risk assessment

Risk is moderate rather than high. The formula review did not reveal a correctness issue, and the docs are reasonably clear about asymmetry, but the automated tests are not strong enough to protect that contract. A future refactor could accidentally make the function symmetric, change reference-color semantics, or alter default weighting behavior without the current suite clearly failing.

### `delta_e_2000`

#### Implementation observations

No implementation mismatch was found in `de2000.hpp` during the structural review. The code converts to CIELAB, promotes the working values to `double`, computes the `G` factor from the average unadjusted chroma, applies the adjusted `a'` values, derives `C'` and hue angles in degrees, handles the `Δh'` wrap-around cases, computes `ΔH'`, forms the mean `L'`, `C'`, and `h'` values with the achromatic and 360-degree boundary cases handled explicitly, and then applies the standard `T`, `S_L`, `S_C`, `S_H`, `R_C`, and `R_T` terms before returning the final square-root expression. The internal `double` path called out in the docs is real and directly visible in the implementation.

#### Reference comparison

The structural comparison against Sharma's public CIEDE2000 reference material did not find a formula mismatch. The implementation follows the published sequence for `G`, adjusted `a'`, `h'` normalization into `[0, 360)`, conditional hue-difference averaging, the `T` trigonometric weighting term, `R_C`, and the final `R_T` blue-region rotation correction. Separately, a standalone probe was compiled against six published Sharma sample pairs from `ciede2000testdata.txt`, and all six reproduced the expected values with printed `abs_diff=0.0000` at four decimal places. That probe is strong sample-data confirmation for those cases, but it is still only a subset check rather than complete proof over the full published table.

#### Test adequacy

Validation is better than for the other two metrics, but it is still only partially strong. The implementation now has two independent supports: formula-structure review against the public reference and an external probe that matches published sample data exactly for the exercised rows. However, the repository's permanent test suite still contains only generic property checks such as zero, symmetry, non-negativity, and rough ordering. It does not embed the published Sharma vectors, does not regression-test the hue wrap-around edge cases explicitly, and does not exercise the achromatic branches directly. For that reason, `delta_e_2000` is partially validated rather than strongly validated by the checked-in tests alone.

#### Risk assessment

Risk is moderate. The current implementation appears correct based on both structural review and sample-data confirmation, so no correctness mismatch was identified in this audit. The remaining concern is durability: without published-vector regression tests in `tests/algorithms/test_delta_e.cpp`, future edits to the delicate hue-averaging or rotation-term logic could slip through despite the function's current correctness.

### `delta_e_cmc`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_din99`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_ok`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_ok` SIMD path

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

## Cross-Cutting Findings

## Priority Table

| ID | Priority | Finding | Evidence | Impact | Suggested remediation |
|----|----------|---------|----------|--------|-----------------------|

## Recommended Fix Order

## Suggested Follow-Up Work
