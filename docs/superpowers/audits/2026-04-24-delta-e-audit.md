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

No confirmed arithmetic defect was proven in this audit, so the report does not assign any `P0` findings. The highest-risk metric is `delta_e_din99`: its current transform looks DIN99-inspired rather than demonstrably DIN99-faithful, and the checked-in tests only cover generic metric invariants rather than published DIN99 conformance data. That combination makes DIN99 the clearest correctness-risk candidate even though the audit stops short of claiming a proven wrong answer.

The next tier of risk is validation depth, not confirmed mismatch. `delta_e_94`, `delta_e_cmc`, and `delta_e_2000` all look structurally plausible or better, but the permanent test suite in `tests/algorithms/test_delta_e.cpp` still relies mostly on zero/symmetry/non-negativity smoke tests. The module is especially light on golden reference coverage: there are no checked-in published sample vectors for `delta_e_94`, `delta_e_cmc`, or `delta_e_din99`, and even `delta_e_2000` still lacks embedded Sharma regression vectors despite the separate Task 2 probe matching six published samples.

The public documentation is therefore only partially aligned with the implementation confidence level. The docs are appropriately careful about `ΔE94` asymmetry and the experimental SIMD gate, but they currently present `delta_e_ok()` as a broadly recommended modern metric and `delta_e_din99()` as a straightforward industry-standard option even though the audit evidence is much weaker than those descriptions suggest. Readers who only skim the public page would come away with more confidence than the checked-in validation presently justifies.

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

Risk is moderate rather than high. The formula review did not reveal a correctness issue, and the docs are reasonably clear about asymmetry, but the automated tests are not strong enough to protect that contract. A future refactor could accidentally make the function symmetric, change reference-color semantics, or alter default weighting behavior without the current suite failing on that contract.

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

Risk is moderate. The implementation itself looks formula-faithful, but public numeric reference data was not checked and the current tests are too shallow to prove more than basic invariants. The main remaining risk is silent regression in the reference-color weighting behavior, especially because the asymmetry contract is described in prose and test names but not enforced by assertions.

### `delta_e_din99`

#### Implementation observations

`din99.hpp` cannot yet be treated as formula-faithful with confidence. It does perform a recognizable DIN99-style transform by logarithmically compressing lightness and chroma and rotating hue by 16 degrees, but the public DIN99 transforms usually include additional intermediate rotated coordinates and scale factors beyond a direct `C99 * cos(h - 16°)` / `C99 * sin(h - 16°)` mapping. This implementation also applies `k_E` inside the coordinate transform and then multiplies the final Euclidean distance by `k_E` again, which leaves the overall scaling behavior insufficiently justified against the published formulas.

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

Risk is low to moderate. The path is opt-in and currently documented as experimental, and the tests do a decent job of checking SIMD/scalar consistency on the encoded cases. The residual risk is mostly coverage-related: the suite validates consistency, not independent scalar correctness, and it exercises only a small set of hand-picked inputs.

## Cross-Cutting Findings

The audit splits cleanly into three buckets. First, there is one plausible correctness risk: `delta_e_din99` appears incomplete relative to published DIN99 transform descriptions, and nothing in `tests/algorithms/test_delta_e.cpp` would reliably catch a structurally wrong but monotonic implementation. That is the only metric whose implementation confidence is low enough to justify treating it as a likely correctness hazard rather than a routine test gap.

Second, several findings are confidence gaps rather than mismatches. One gap is missing numeric conformance coverage where published vectors are available or reasonably expected: `delta_e_2000` already has an external Sharma subset probe from the audit, but those published cases are not in the permanent suite, and `delta_e_94`, `delta_e_cmc`, and `delta_e_din99` still lack checked-in numeric vectors despite being presented as standard or standards-adjacent metrics. A separate gap is missing behavioral contract coverage: `delta_e_94` and `delta_e_cmc` both depend on reference-color asymmetry, yet the tests named `IsGenerallyAsymmetric` only assert non-negativity instead of proving that argument order can matter. The SIMD path is similarly better than average, but its evidence is scalar-equivalence coverage rather than external numeric conformance.

Third, the docs currently overstate certainty in a few places. `docs/reference/delta_e.rst` accurately documents `ΔE94` asymmetry and the experimental SIMD opt-in, but it also labels `delta_e_din99()` as an improved German standard without flagging the current audit uncertainty and describes `delta_e_ok()` as "recommended for most new applications" even though the evidence here supports "useful Euclidean Oklab heuristic" more strongly than "default replacement for standardized Delta E metrics." The cross-cutting remediation theme is therefore to add validation before changing most implementations, while treating DIN99 as the one area where validation is needed urgently enough to confirm or disprove a likely defect.

## Priority Table

| ID | Priority | Finding | Evidence | Impact | Suggested remediation |
|----|----------|---------|----------|--------|-----------------------|
| F1 | P1 | `delta_e_din99()` is the strongest correctness risk because the audited transform appears incomplete relative to published DIN99 formulas and has no golden-vector backstop. | Published DIN99 writeups summarized in the `delta_e_din99` section require additional rotated/anisotropic steps; `tests/algorithms/test_delta_e.cpp` only checks generic invariants in `DeltaEDIN99Test.*` (`SameColorIsZero`, `IsSymmetric`, `IsNonNegative`, threshold helpers). | A structurally wrong DIN99 implementation could ship plausible-looking numbers and still pass the current suite. | Add DIN99 reference-vector validation first; if the vectors fail, then update the implementation to match the chosen published DIN99 variant and keep those vectors as regression tests. |
| F2 | P1 | Checked-in numeric conformance coverage is missing for the metrics where published vectors are available or reasonably expected, including the default `ΔE2000` path. | `tests/algorithms/test_delta_e.cpp` uses property-style checks for `DeltaE94Test.*`, `DeltaE2000Test.*`, `DeltaECMCTest.*`, and `DeltaEDIN99Test.*`; no published sample tables are embedded there. The audit's Task 2 probe matched six Sharma `ΔE2000` samples, but that evidence is not in the permanent suite. | Future formula regressions can slip through because passing tests only proves broad invariants, not whether outputs still match reference values. | Promote published sample vectors into the checked-in tests, starting with Sharma `ΔE2000`, then add the best available numeric references for `ΔE94`, CMC, and DIN99 when those sources are confirmed. |
| F3 | P1 | Behavioral contract tests are missing for the reference-order semantics of `delta_e_94()` and `delta_e_cmc()`. | `docs/reference/delta_e.rst` notes `ΔE94` asymmetry; `tests/algorithms/test_delta_e.cpp` lines covering `DeltaE94Test.IsGenerallyAsymmetric` and `DeltaECMCTest.IsGenerallyAsymmetric` only assert both call orders are non-negative. | A refactor could accidentally make these APIs symmetric or swap reference/comparison semantics without obvious test failures even if numeric smoke tests still pass. | Replace the current asymmetry smoke tests with explicit argument-order-sensitive examples so the reference/comparison contract is enforced independently of numeric conformance coverage. |
| F4 | P2 | Public docs currently overclaim confidence for `delta_e_ok()` and understate uncertainty around `delta_e_din99()`. | `docs/reference/delta_e.rst` describes `delta_e_din99()` as an improved DIN standard and `delta_e_ok()` as "Recommended for most new applications"; the audit found weaker evidence for DIN99 correctness and only heuristic-level support for `ΔE_OK` positioning. | Users may choose a metric based on stronger claims than the implementation and test evidence presently support. | Revise the reference page to distinguish standardized CIE metrics from heuristic Oklab distance, and add a caution note that DIN99 remains under audit until conformance data is added. |
| F5 | P2 | SIMD confidence is narrower than the scalar path confidence because tests prove consistency, not independent correctness. | `tests/algorithms/test_delta_e_simd.cpp` validates opt-in gating and scalar equivalence for a few `oklab_t` and converted RGBA inputs, but it cites no external reference vectors. | The experimental fast path is unlikely to be wrong independently, yet its coverage is too narrow to catch edge-case divergences with high confidence. | Keep the path experimental and extend the SIMD suite with more edge cases after the scalar `ΔE_OK` documentation and validation story are clarified. |

## Recommended Fix Order

Start by adding validation, not by rewriting formulas. The highest-value first move is to assemble a small set of trusted numeric vectors for `delta_e_din99()` and run them against the current implementation, because that is the fastest way to confirm whether the strongest suspected defect is real. In parallel or immediately after, promote the already-reviewed Sharma `ΔE2000` examples into `tests/algorithms/test_delta_e.cpp`; that work is low effort and quickly upgrades the default helper path from "audited once" to "continuously guarded."

Next, tighten the misuse-prone contracts. Replace the current `ΔE94` and CMC asymmetry smoke tests with cases that actually fail if argument order stops mattering. That work should stand on its own as behavioral contract coverage, separate from the numeric conformance vectors above, so reference/comparison semantics stay protected even before broader published-vector coverage is complete.

Only after those checks are in place should implementation changes be considered. If DIN99 fails the newly added vectors, that becomes the first code-level correction target. If the asymmetric metrics and `ΔE2000` pass their stronger tests, implementation changes there should wait, and the remaining work shifts to documentation cleanup plus incremental coverage expansion.

## Suggested Follow-Up Work

After the first validation pass, align the public docs with whatever confidence remains. If DIN99 is still unverified, mark it as provisional instead of presenting it as a settled standard implementation. For `delta_e_ok()`, keep the emphasis on Euclidean distance in Oklab and practical utility, but avoid broad recommendation language until there is a clearer basis for comparing it against the CIE metrics in this library's intended use cases.

Once the highest-value vectors are in place, broaden coverage deliberately rather than uniformly. Add hue-wrap and neutral-axis edge cases for `ΔE2000`, reference-order examples for `ΔE94` and CMC, and a few more SIMD/scalar equivalence cases around threshold-adjacent Oklab values. That follow-up work is mostly about durability and maintenance confidence, not immediate algorithm replacement.
