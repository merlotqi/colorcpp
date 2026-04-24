# colorcpp TODO

This file tracks main-branch follow-up work for capabilities that already exist in the repository, but are not fully closed out yet.

Priority is ordered by likely end-user impact rather than implementation ease.

## P1. Close Remaining CSS Color Parsing Gaps

- [x] Add the missing multi-item and progress-form `color-mix()` support.
- [ ] Implement the remaining relative color syntaxes beyond `rgb(from ...)` and `color(from ...)`.
- [ ] Add the missing multi-stop and `at <percentage>` `color-mix()` extensions.
- [ ] Finish the remaining context-sensitive CSS color features that still depend on external authoring state.
- [ ] Add focused tests and examples for each newly completed syntax so the supported surface is easy to trust.

Why this is still open:

- The parser is already a major public capability, but there are still CSS Color 5 surfaces beyond the newly completed `color-mix()` extensions.
- Users are likely to discover these missing cases through normal CSS Color 5 usage rather than through edge-case exploration.

Done when:

- The README, reference docs, and TODO agree on the supported parser surface and the remaining gaps.
- Each supported syntax has direct tests and at least one representative example.

Relevant files:

- `README.md`
- `docs/reference/css_color.rst`
- `include/colorcpp/io/css/*`
- `tests/io/test_css.cpp`

## P2. Finish Self-Describing Serialization

- [ ] Implement the documented tagged / self-describing serialization mode for JSON.
- [ ] Decide whether MessagePack also supports a tagged map form, and document that decision clearly.
- [ ] Wire `serialization_options::include_color_space` into the actual serialization output.
- [ ] Add end-to-end tests with a concrete adapter instead of only trait-level coverage.

Why this is still open:

- The docs currently describe compact, named, and tagged formats, but the code only exposes compact and named formats.
- `serialization_options` already contains `include_color_space`, which suggests an unfinished feature rather than an intentionally omitted one.

Done when:

- Public docs, public options, and actual behavior all describe the same set of modes.
- Tests verify round-tripping for compact, named, and any supported tagged format with a real adapter.

Relevant files:

- `docs/reference/serialization.rst`
- `include/colorcpp/io/serialization/traits.hpp`
- `include/colorcpp/io/serialization/json.hpp`
- `include/colorcpp/io/serialization/msgpack.hpp`
- `tests/io/test_serialization.cpp`
- `examples/serialization_example.cpp`

## P3. Reopen and Validate Display P3 Conversion Coverage

- [ ] Re-enable the disabled Display P3 conversion and round-trip tests.
- [ ] Confirm the current conversion graph and direct conversion paths satisfy those tests.
- [ ] Update the stale test comments so they reflect current repository reality.
- [ ] Add one public-facing example or reference note that makes the Display P3 story easier to trust.

Why this is still open:

- The conversion code exists, but the dedicated conversion tests are still commented out with an old "missing conversion implementation" note.
- That leaves a gap between "capability appears implemented" and "capability is actively verified."

Done when:

- The commented-out Display P3 conversion tests are restored or replaced by active coverage.
- Wide-gamut conversion paths are validated by normal CI rather than by source inspection alone.

Relevant files:

- `include/colorcpp/operations/conversion/functions/display_p3.hpp`
- `include/colorcpp/operations/conversion/functions/xyz.hpp`
- `tests/core/test_display_p3.cpp`
- `examples/display_p3_example.cpp`

## P4. Decide the Long-Term Shape of Experimental SIMD Paths

- [ ] Decide whether `delta_e_ok()` SIMD should graduate from experimental, stay experimental, or be removed.
- [ ] Decide whether Machado vision SIMD should graduate from experimental, stay experimental, or be removed.
- [ ] Back the decision with benchmark evidence that is easy for contributors to reproduce.
- [ ] Align the README, reference docs, CMake options, and tests with that final policy.

Why this is still open:

- The repository already exposes SIMD options, but some paths remain behind extra experimental flags because the end-to-end benefit is still unclear.
- That is a valid intermediate state, but it is not a finished public performance story.

Done when:

- Each SIMD path has a clear status: stable, experimental, or removed.
- The build flags and docs match the measured behavior on supported targets.

Relevant files:

- `CMakeLists.txt`
- `README.md`
- `docs/reference/delta_e.rst`
- `docs/reference/vision.rst`
- `include/colorcpp/algorithms/delta_e/simd.hpp`
- `include/colorcpp/algorithms/vision/simd.hpp`
- `tests/algorithms/test_delta_e_simd.cpp`
- `tests/algorithms/test_vision_simd.cpp`
