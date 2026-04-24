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

The structural comparison against Sharma's public CIEDE2000 reference material did not find a formula mismatch. The implementation follows the published sequence for `G`, adjusted `a'`, `h'` normalization into `[0, 360)`, conditional hue-difference averaging, the `T` trigonometric weighting term, `R_C`, and the final `R_T` blue-region rotation correction. Separately, a standalone probe was compiled against six published Sharma sample pairs from `ciede2000testdata.txt`; these were the six samples embedded in the Task 2 standalone probe, and each matched the published value to the displayed precision with printed `abs_diff=0.0000` at four decimal places. That probe is strong sample-data confirmation for those cases, but it is still only a subset check rather than complete proof over the full published table.

#### Test adequacy

Validation is better than for the other two metrics, but it is still only partially strong. The implementation now has two independent supports: formula-structure review against the public reference and an external probe whose exercised rows matched the published Sharma values to four decimal places. However, the repository's permanent test suite still contains only generic property checks such as zero, symmetry, non-negativity, and rough ordering. It does not embed the published Sharma vectors, does not regression-test the hue wrap-around edge cases explicitly, and does not exercise the achromatic branches directly. For that reason, `delta_e_2000` is partially validated rather than strongly validated by the checked-in tests alone.

#### Risk assessment

Risk is moderate. The current implementation appears correct based on both structural review and sample-data confirmation, so no correctness mismatch was identified in this audit. The remaining concern is durability: without published-vector regression tests in `tests/algorithms/test_delta_e.cpp`, future edits to the delicate hue-averaging or rotation-term logic could slip through despite the function's current correctness.

### `delta_e_cmc`

#### Implementation observations

`cmc.hpp` looks formula-faithful rather than merely plausible. The function converts both inputs to CIELAB, treats the first color as the reference color, computes `dL`, `dC`, and `dH` from the usual LAB/chroma decomposition, derives the reference hue angle `h1`, and then applies the standard CMC lightness, chroma, and hue weighting terms `S_L`, `S_C`, and `S_H` with caller-controlled `l` and `c` weights. The documented defaults `l=2`, `c=1` match the common acceptability preset, and the code comments also call out `l=1`, `c=1` as the perceptibility preset. The sign choices for `dL` and `dC` do not look problematic because those terms are squared in the final expression.

#### Reference comparison

The structure matches the public CMC l:c equations commonly reproduced in secondary references: the same low-light `S_L = 0.511` branch, the same `0.040975` and `0.01765` lightness coefficients, the same `S_C = 0.638 + 0.0638 C / (1 + 0.0131 C)` form, and the same hue-region split used for `T`. No structural mismatch stood out in that review, so this implementation appears formula-faithful. Public numeric conformance vectors were not available in the sources reviewed for this task, so this is still a formula-structure conclusion rather than a published-sample-data confirmation.

#### Test adequacy

The current tests prove only narrower properties. They cover zero distance, non-negativity, a large black-versus-white separation, the default-parameter relationship between perceptibility and acceptability, and the helper threshold wrappers. `DeltaECMCTest.IsGenerallyAsymmetric` does not actually assert asymmetry even though the algorithm is reference-color based, and the suite does not pin any published numeric CMC examples. That means the checked-in tests provide weak regression coverage for parameter semantics and almost no direct protection against formula drift.

#### Risk assessment

Risk is moderate. The implementation itself looks formula-faithful, but public numeric reference data was not checked and the current tests are too shallow to prove more than basic invariants. The main remaining risk is silent regression in the reference-color weighting behavior, especially because the asymmetry contract is described in prose and test names more clearly than it is enforced by assertions.

### `delta_e_din99`

#### Implementation observations

`din99.hpp` looks questionable, not confidently formula-faithful. It does perform a recognizable DIN99-style transform by logarithmically compressing lightness and chroma and rotating hue by 16 degrees, but the public DIN99 transforms usually include additional intermediate rotated coordinates and scale factors beyond a direct `C99 * cos(h - 16°)` / `C99 * sin(h - 16°)` mapping. This implementation also applies `k_E` inside the coordinate transform and then multiplies the final Euclidean distance by `k_E` again, which makes the overall scaling behavior look suspicious rather than obviously standard.

#### Reference comparison

Against public DIN99 writeups, the implementation is at best only partially aligned. The `L99 = 105.51 * log(1 + 0.0158 L)` portion is recognizable, but the widely reproduced DIN99 equations also introduce the rotated `e/f` coordinates, the `a99 = C99 cos(h99)` and `b99 = 0.7 C99 sin(h99)` style anisotropy, and a derived `G = sqrt(e^2 + f^2)` term before taking Euclidean distance in DIN99 space. Those extra steps are absent here, so the current transform looks more like a plausible simplification than a faithful implementation of the published DIN99 metric. Public numeric reference data was not available in the reviewed sources, so this section cannot quantify the resulting numeric error.

#### Test adequacy

The current DIN99 tests prove only very general metric behavior: zero on identical colors, symmetry, non-negativity, a large black-versus-white separation, and the helper threshold wrappers. They do not exercise any published DIN99 vectors, do not distinguish among DIN99 formula variants, and do not test the `k_L` or `k_E` parameter semantics at all. As a result, the suite would not reliably catch a structurally wrong but monotonic transform.

#### Risk assessment

Risk is high. The implementation is recognizable as DIN99-inspired, but the missing public-formula steps make correctness uncertain, and the tests are far too weak to compensate. Until this function is checked against a primary or high-quality public DIN99 reference with numeric vectors, it should be treated as questionable rather than validated.

### `delta_e_ok`

#### Implementation observations

`oklab.hpp` is straightforward and appears formula-faithful for what it claims to compute. The function converts both inputs to `core::oklab_t`, optionally takes an explicitly gated fast path, and otherwise returns `sqrt(ΔL^2 + Δa^2 + Δb^2)` in Oklab space. The paired `is_visually_same_ok()` helper is just a threshold wrapper over that Euclidean distance, and `oklab_jnd_typical = 0.02f` is presented as a typical JND-style threshold rather than a standards-derived constant.

#### Reference comparison

The public Oklab references requested for this task support the semantics here: Oklab is defined as a perceptual color space with coordinates intended for Euclidean operations, and CSS Color 4 uses Oklab/OkLCh as modern interpolation spaces. That confirms `delta_e_ok()` is Euclidean distance in Oklab space, not a CIE-standard `ΔE*ab`, `ΔE94`, or `ΔE00` formula. Public numeric reference data for an independently standardized "`ΔE_OK`" metric was not available in those sources, because the sources define the space and its use, not a separate published conformance table for this helper.

#### Test adequacy

The scalar tests only prove the narrow properties expected of a Euclidean distance: zero on identical inputs, symmetry, non-negativity, and the default-threshold behavior for identical colors. They do not compare against published Oklab numeric examples, and they do not prove that `0.02` is an externally validated universal JND threshold. So the suite supports the implementation's basic Euclidean semantics, but not broader perceptual-quality or standards claims.

#### Risk assessment

Risk is moderate. The implementation itself is simple and appears correct for Euclidean Oklab distance, but the documentation currently overstates confidence by presenting `delta_e_ok()` as a "Modern Perceptual Metric" that is "recommended for most new applications." The evidence reviewed here supports "Euclidean distance in Oklab" and "useful modern heuristic," not "standardized replacement for CIE Delta E" or broad superiority across use cases.

### `delta_e_ok` SIMD path

#### Implementation observations

The SIMD path is narrowly scoped and implementation-faithful to the scalar helper. `simd.hpp` keeps the optimization behind two gates: global SIMD enablement plus the extra experimental opt-in macro `COLORCPP_ENABLE_EXPERIMENTAL_DELTA_E_OK_SIMD`, and then an SSE2-capable target check. When enabled, the fast path simply loads the three Oklab components plus a zero padding lane into an `__m128`, computes squared component differences, horizontally sums the first three lanes, and takes the square root. That is the same Euclidean Oklab calculation as the scalar helper, just with SSE2 intrinsics.

#### Reference comparison

There is no separate public reference formula for the SIMD path beyond the scalar Oklab semantics it is supposed to preserve. The right comparison standard here is therefore scalar equivalence, not an external "`ΔE_OK SIMD`" definition. The implementation matches that expectation: it is an alternate execution path for the same Euclidean Oklab distance, not a different metric.

#### Test adequacy

This is the strongest-tested part of the remaining batch, but it still proves SIMD consistency more than scalar correctness. The focused `test_delta_e_simd` binary passed locally in this task run, with all 4 tests passing, and those tests verify that fast-path availability matches the explicit opt-in gate, that `delta_e_ok()` matches the scalar helper on several direct Oklab pairs, that the internal fast path matches the scalar helper when compiled in, and that mixed non-Oklab inputs still agree with the scalar Oklab reference after conversion. What they do not prove is that either path matches a published external numeric reference or that the SIMD path has broad edge-case coverage.

#### Risk assessment

Risk is low to moderate. The path is clearly opt-in and currently documented as experimental, and the tests do a decent job of checking SIMD/scalar consistency on the encoded cases. The residual risk is mostly coverage-related: the suite validates consistency, not independent scalar correctness, and it exercises only a small set of hand-picked inputs.

## Cross-Cutting Findings

## Priority Table

| ID | Priority | Finding | Evidence | Impact | Suggested remediation |
|----|----------|---------|----------|--------|-----------------------|

## Recommended Fix Order

## Suggested Follow-Up Work
