# CSS `color-mix()` Standard Extension Design

## Goal

Extend `colorcpp`'s CSS parser so `color-mix()` matches the current standards-aligned surface more closely, while preserving all existing passing behavior for the current two-color implementation.

This first-stage project is intentionally narrow. It only covers `color-mix()` parsing and evaluation. It does not attempt to finish all remaining CSS Color 5 work in one pass.

## Problem Statement

The current parser in `include/colorcpp/io/css/parse_detail.hpp` supports a useful but incomplete subset of `color-mix()`:

- exactly two color operands
- optional interpolation space
- polar hue interpolation keywords for `lch` and `oklch`
- premultiplied-alpha style mixing
- nested use with existing color syntaxes

That implementation is structurally specialized for two operands:

- it splits the payload into `raw_first` and `raw_second`
- it resolves weights with a two-operand-only helper
- it evaluates a single pairwise mix directly

This structure blocks standards-aligned extensions such as:

- multi-item `color-mix(...)`
- the progress form described in CSS Values 5
- default interpolation handling without forcing a separate ad hoc path

The repository TODO currently mentions "multi-stop / `at <percentage>` `color-mix()` extensions", but the implementation work for this project should follow current W3C drafts rather than introduce non-standard syntax.

## Scope

### In Scope

- Extend `color-mix()` to support a general item-list model instead of a hard-coded two-item model.
- Support multi-item `color-mix(...)` in the CSS parser.
- Support the progress form of `color-mix(...)` and normalize it into the same internal representation as the item-list form.
- Support omitted interpolation method with the standards-aligned default behavior used by this project.
- Support single-item `color-mix(...)` where the syntax is otherwise valid.
- Preserve existing support for:
  - `srgb`, `srgb-linear`, `display-p3`, `display-p3-linear`, `lab`, `lch`, `oklab`, `oklch`, and `xyz`
  - hue interpolation keywords for polar spaces
  - nested `color(...)`, `light-dark(...)`, context colors, and alpha-aware mixing
- Update docs and tests to match the actual supported `color-mix()` surface.

### Out of Scope

- Remaining relative color syntaxes beyond `rgb(from ...)` and `color(from ...)`
- Other context-sensitive CSS color features not directly required by `color-mix()`
- A unified AST refactor for all CSS color functions
- New non-standard syntax invented by the repository

## Standards Baseline

Implementation should track current W3C drafts rather than the older wording in `docs/todo.md`:

- CSS Color Module Level 5
- CSS Values and Units Level 5

The project should not add a private `at <percentage>` syntax unless a later explicit product decision says to diverge from the standard.

## Design Overview

The recommended design is a local refactor inside `include/colorcpp/io/css/parse_detail.hpp`.

Instead of treating `color-mix()` as a special two-operand parser, the implementation should:

1. parse the interpolation method
2. parse a normalized list of mix items
3. resolve weights for the whole list
4. fold the list left-to-right through the existing pairwise mixing logic

This keeps the change tightly scoped to the CSS parser while making future standards-aligned `color-mix()` work much easier.

## Internal Data Model

### Existing Structure to Replace

Current internal types:

- `color_mix_operand`
- `color_mix_weights`
- direct two-operand extraction in `parse_color_mix_rgbaf(...)`

These are sufficient for the current implementation, but they encode the wrong abstraction for multi-item mixing.

### Proposed New Internal Structures

Retain the existing interpolation method abstractions, but replace the operand model with:

- `color_mix_item`
  - `std::string_view color`
  - `std::optional<float> weight`

- `parsed_color_mix`
  - `color_mix_interpolation_method method`
  - `std::vector<color_mix_item> items`

- `resolved_color_mix_item`
  - `std::string_view color`
  - `float weight`

The resolved form exists so parsing and weight normalization remain separate concerns.

## Parsing Flow

### Top-Level Dispatch

`parse_color_mix_rgbaf(...)` should continue to:

- verify the function name
- extract the parenthesized payload
- reject trailing content

After that, it should stop assuming there are exactly two operands.

### Syntax Branches

The parser should recognize two standards-aligned forms:

1. Item-list form
   - optional leading `in <color-interpolation-method>`
   - followed by a comma-separated list of one or more mix items

2. Progress form
   - `<progress>, <color>, <color>`
   - parsed only when the payload shape matches this form clearly

The progress form should be normalized immediately into an equivalent item-list representation so the evaluator only needs one code path.

### Item Parsing

Each mix item should support:

- `<color>`
- `<color> <percentage>`
- `<percentage> <color>`

Parsing must continue to use top-level comma awareness so nested functions such as `light-dark(...)`, `color(...)`, and nested `color-mix(...)` remain valid.

## Weight Resolution

The current `resolve_color_mix_weights(...)` helper is two-operand-specific. It should be replaced by list-aware logic.

### Requirements

- Missing weights must be filled according to the chosen normalized form for this project.
- Explicit weights must be clamped consistently with current behavior.
- An all-zero effective sum must fail.
- The output of this stage must be a normalized list of resolved items plus any alpha scaling needed to preserve current reduced-alpha semantics when the explicit percentages sum below `100%`.

### Evaluation Strategy

Multi-item mixes should be evaluated left-to-right by repeatedly applying the existing pairwise mixing model.

This is the safest choice because:

- it preserves current pairwise semantics
- it avoids introducing a second independent rectangular/polar mixing algorithm
- it gives a single consistent evaluation path for 2 items and 3+ items

## Mixing Semantics

### Rectangular Spaces

Reuse the current premultiplied-alpha rectangular mixing path:

- `srgb`
- `srgb-linear`
- `lab`
- `oklab`
- `xyz`
- `display-p3`
- `display-p3-linear`

### Polar Spaces

Reuse the current polar mixing path for:

- `lch`
- `oklch`

Hue interpolation keywords remain valid only for polar spaces:

- `shorter hue`
- `longer hue`
- `increasing hue`
- `decreasing hue`

Any attempt to use them with non-polar spaces must still fail.

## Error Handling

The parser should fail fast with `std::nullopt` for:

- malformed top-level `color-mix(...)` syntax
- invalid progress-form arity
- item-list with zero parsed items
- any item whose color payload cannot be parsed
- invalid interpolation space
- hue interpolation keywords used with non-polar spaces
- effective total weight equal to zero
- trailing tokens after the full expression

The parser should remain strict rather than trying to recover.

## Compatibility Rules

This project must preserve all existing valid behavior already covered by `tests/io/test_css.cpp`, especially:

- existing two-color `color-mix()` calls
- nested `light-dark(...)` inside `color-mix()`
- nested `color(...)` inside `color-mix()`
- use with context-sensitive colors such as `currentColor` and system colors
- current alpha behavior for partially weighted mixes

The project should not change unrelated CSS parser behavior.

## Testing Strategy

### Regression Coverage

Keep all existing `color-mix()` tests passing unchanged unless a test is based on a now-corrected standards misunderstanding.

### New Positive Tests

Add focused cases for:

- default interpolation method with no `in ...`
- single-item `color-mix(...)`
- three-item and four-item mixes
- multi-item mixes with mixed explicit and implicit percentages
- progress-form parsing
- nested multi-item use with context-aware colors

### New Negative Tests

Add focused cases for:

- malformed progress form
- malformed multi-item separators
- invalid weight placement
- unsupported hue-keyword usage
- all-zero effective weights
- valid expression plus trailing garbage

### Documentation Alignment Tests

Where practical, add at least one example-backed test case that corresponds to any new README or reference-doc snippet added for this project.

## Documentation Changes

Update the following to reflect the new supported surface:

- `README.md`
- `docs/reference/css_color.rst`
- `docs/todo.md`

The wording should stop promising non-standard `at <percentage>` syntax unless the repository explicitly chooses that divergence later.

## File-Level Responsibilities

- `include/colorcpp/io/css/parse_detail.hpp`
  - primary parser and evaluator changes
- `tests/io/test_css.cpp`
  - regression and new coverage
- `README.md`
  - user-facing support summary
- `docs/reference/css_color.rst`
  - narrative reference update
- `docs/todo.md`
  - mark the completed sub-item accurately once shipped

## Risks

### Risk 1: Silent behavior drift in existing two-color mixes

Mitigation:

- keep existing tests intact
- evaluate new multi-item support through the same pairwise mixing core

### Risk 2: Overreaching into the broader CSS parser

Mitigation:

- keep the change local to `color-mix()` parsing in `parse_detail.hpp`
- do not refactor relative color parsing in this project

### Risk 3: Implementing non-standard syntax by following stale TODO wording

Mitigation:

- align implementation and docs with current W3C drafts
- treat `docs/todo.md` as a project tracker, not the normative grammar source

## Success Criteria

This design is complete when:

- `color-mix()` no longer relies on a hard-coded two-item parser model
- multi-item and progress-form `color-mix()` are supported through a unified implementation
- current passing two-item behavior remains intact
- docs and tests describe the same supported feature set
