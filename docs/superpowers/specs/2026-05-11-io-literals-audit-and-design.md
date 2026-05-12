# IO Literals Audit And Design

Date: 2026-05-11

## Summary

This document defines the audit and design direction for `include/colorcpp/io/literals.hpp` and the non-CSS literal
subheaders it aggregates.

The literal layer is valuable because it gives the library a compact compile-time authoring surface. The current
problem is not lack of suffixes alone. The current problem is that correctness, validation, and coverage policy are
not aligned. There is at least one confirmed scaling bug, at least one confirmed invalid-input acceptance bug, and
several places where the docs describe a larger or cleaner surface than the implementation actually provides.

## Scope

This design covers:

- `include/colorcpp/io/literals.hpp`
- `include/colorcpp/io/literals/details.hpp`
- `include/colorcpp/io/literals/{rgb,hsl,hsv,hwb,cmyk,oklab}.hpp`
- `tests/io/test_literals.cpp`
- `docs/reference/literals.rst`
- literal examples in `README.md` and `examples/io_example.cpp`

This design does not cover:

- CSS named color literals under `colorcpp::io::css::named_literal`
- stream parsing in `colorcpp::core::io`
- CSS `color()` / `lab()` / `oklch()` string parsing

## Inputs Reviewed

The design is based on the current repository state, including:

- `include/colorcpp/io/literals/*`
- `tests/io/test_literals.cpp`
- `docs/reference/literals.rst`
- `README.md`
- `examples/io_example.cpp`
- relevant core type definitions under `include/colorcpp/core`

## Current State

The literal surface currently includes:

- RGB-family literals: `_rgb`, `_rgba`, `_argb`, `_hex`
- packed decimal literals for `_hsl`, `_hsla`, `_hsv`, `_hsva`, `_hwb`, `_hwba`
- packed decimal literals for `_cmyk`
- packed decimal literals for `_oklab` and `_oklch`

This is already a substantial and pleasant authoring surface. The problem is that it mixes three different policy
layers without documenting them cleanly:

- strict compile-time numeric packing
- runtime string parsing for `_hex`
- selective coverage of only some public color models

## Current State Assessment

Module state: `Useful But Inconsistent`

Why:

- the happy-path RGB/HSL/HSV/HWB/CMYK authoring experience is already useful
- most literal operators are constexpr and easy to demonstrate
- coverage and validation rules are inconsistent across suffix families
- the current tests do not yet protect the most important edge cases

## Confirmed Findings

### 1. `_oklch` chroma scaling is incompatible with the core type range

The `_oklch` documentation says that the packed `C` field uses `000-100` and maps to `0.0-0.4`. The implementation
currently constructs:

- `core::oklch_t{l / 100.0f, c / 100.0f, h}`

That maps `C=100` to `1.0`, which exceeds the `core::oklch_t` chroma range of `0.0-0.4`.

Local validation confirmed the mismatch: `50'050'120_oklch` throws `colorcpp: channel value out of range` even
though the documented example family implies that such values should be valid.

This is a real correctness bug.

### 2. `_oklaba` and `_oklcha` are documented but not implemented

`include/colorcpp/io/literals/oklab.hpp` describes four-channel Oklab and OkLCH literal forms with `_oklaba` and
`_oklcha` examples, but the file only implements `_oklab` and `_oklch`.

This creates direct API drift:

- docs imply a larger surface
- tests do not guard the mismatch
- users cannot tell whether the missing alpha forms are a temporary omission or an intentional non-feature

### 3. `_hex` string parsing accepts invalid hexadecimal characters as zero

`details::char_to_hex()` returns `0` for non-hex characters, and `parse_hex_string()` does not validate that every
digit is legal before constructing the result.

Local validation confirmed the effect:

- `"#GG0000"_hex` yields `rgba8_t{0, 0, 0, 255}`

This is not acceptable for a public parsing surface. Invalid input should be rejected, not coerced into an unrelated
valid color.

### 4. The coverage policy for public color models is not explicit

The aggregate header currently exposes literals for:

- RGB-family
- HSL/HSV/HWB
- CMYK
- Oklab/OkLCH

It does not expose literal forms for other public core types such as:

- `cielab_t` / `cielch_t`
- `display_p3f_t` / `display_p3af_t`
- `xyz_t`
- `cmyk_float_t`

The gap may be intentional, but the policy is not stated anywhere. That makes the surface feel incomplete rather than
selectively curated.

### 5. The tests miss the highest-value contract cases

`tests/io/test_literals.cpp` currently exercises many happy paths, but it does not yet protect:

- `_oklch` scaling behavior
- invalid hex digit rejection for `_hex`
- documentation parity for the Oklab alpha literal claims
- the intended scope of the literal family as a whole

The suite is therefore not yet strong enough to keep the literal layer semantically honest as it grows.

## Validated Evidence

The following evidence was validated locally during this audit:

- `50'050'120_oklch` threw `colorcpp: channel value out of range`, confirming that the current chroma scaling does
  not match the documented `0.0-0.4` mapping
- `"#GG0000"_hex` produced `0,0,0,255`, confirming that invalid hexadecimal digits are currently coerced to zero
  rather than rejected
- source inspection confirmed that `include/colorcpp/io/literals/oklab.hpp` documents `_oklaba` and `_oklcha`
  examples without implementing those suffixes

## Design Goals

The revised literal layer should:

1. reject malformed runtime string input rather than coercing it
2. align every documented suffix with a real implementation
3. make the model-coverage policy explicit
4. preserve constexpr-friendly authoring for supported numeric suffixes
5. expand only after the current surface is trustworthy

## Recommended Direction

### Phase 1: Correctness and truth alignment

The first phase should repair the surface that already exists.

Recommended changes:

- fix `_oklch` chroma scaling to match `core::oklch_t`
- either implement `_oklaba` / `_oklcha` or remove the alpha-form documentation completely
- make `_hex` reject invalid characters with a clear exception path
- add tests for all three behaviors

This is mandatory before any expansion work.

### Phase 2: Decide the coverage policy

The project should choose one explicit literal-surface policy.

Recommended policy:

- keep literals focused on common authoring spaces rather than mirroring every core color model automatically
- document that policy clearly
- treat any future additions such as LAB/LCH or Display P3 literals as deliberate feature work, not as gap-filling by
  implication

This approach keeps the surface smaller and more coherent than a blanket one-literal-per-type expansion.

### Phase 3: Optional targeted expansion

If the project wants broader compile-time authoring coverage after Phase 1 and Phase 2:

- consider adding LAB/LCH literals first because they are conceptually close to the existing Oklab/OkLCH family
- consider Display P3 only if there is a real authoring use case in examples, docs, or higher-level APIs
- avoid adding XYZ or linear-space literals unless a compelling ergonomics case emerges

The guiding rule should be usefulness, not parity for parity's sake.

## Out Of Scope

The following should stay out of the first cleanup wave:

- CSS named color literal work
- locale-aware or flexible free-form string parsing beyond `_hex`
- adding literals for every public type only to achieve surface symmetry
- a broader rewrite of core color constructors

## Success Criteria

The literal layer is successful when:

- documented suffixes all exist and behave as described
- malformed `_hex` strings are rejected
- `_oklch` values map to the real `core::oklch_t` range without surprise exceptions
- the coverage policy for supported models is written down in docs
- tests protect correctness and doc/API parity rather than only happy-path examples

## Suggested Successor Plans

This design should be followed by:

- a literals correctness and doc-alignment implementation plan
- optionally, a separate literals expansion plan if the project later chooses to broaden model coverage
