# IO ANSI Audit And Design

Date: 2026-05-11

## Summary

This document defines the audit and design direction for `include/colorcpp/io/ansi.hpp`.

The `ansi` module is already useful for debugging, demos, and terminal visualization. The primary problem is not
missing breadth. The primary problem is that the happy path is stronger than the edge behavior and contract tests.
The highest-value work is therefore to harden correctness and stream hygiene first, then make small ergonomic
additions only where they clearly reduce friction.

## Scope

This design covers:

- `include/colorcpp/io/ansi.hpp`
- `tests/io/test_ansi.cpp`
- `examples/ansi_example.cpp`
- `examples/io_example.cpp`
- `docs/reference/ansi.rst`
- the ANSI examples and summary text in `README.md`

This design does not cover:

- CSS parsing and named color literals
- terminal capability detection or cross-platform console abstractions
- non-terminal renderers or image output

## Inputs Reviewed

The design is based on the current repository state, including:

- `include/colorcpp/io/ansi.hpp`
- `tests/io/test_ansi.cpp`
- `docs/reference/ansi.rst`
- `README.md`
- `examples/ansi_example.cpp`
- `examples/io_example.cpp`

## Current State

`ansi` is a single-header helper module that currently provides:

- raw ANSI sequence builders: `reset()`, `bold()`, `fg()`, `bg()`
- terminal visualization helpers: `print_swatch()`, `print_color()`, `print_color_verbose()`
- collection/presentation helpers: `print_palette()`, `print_gradient()`
- WCAG-oriented helpers: `relative_luminance()`, `contrast_ratio()`, `wcag_level()`, `print_contrast()`

The public model is intentionally lightweight. Any supported color type is converted to `core::rgba8_t` for display.
That keeps the surface approachable, but it also means the module is sensitive to edge-case formatting and output
policy because it mutates caller-owned streams directly.

## Current State Assessment

Module state: `Usable But Fragile`

Why:

- the common swatch/palette/contrast workflows are already straightforward
- unit tests cover the presence of expected escape sequences and some contrast values
- the module has almost no abstraction overhead
- boundary behavior and stream-state hygiene are underspecified and under-tested

## Confirmed Findings

### 1. `print_gradient()` performs unguarded `steps - 1` math

`print_gradient()` computes:

- `t = i / (steps - 1)`

This creates an unguarded zero denominator when `steps == 1`. The current implementation may still emit output for
that case, but it does so by traveling through undefined or non-portable floating-point behavior rather than through
an explicit contract.

This is a correctness problem, not just a documentation issue. A caller should not need to infer whether `0`, `1`,
or negative step counts are supported.

### 2. The formatted-output helpers do not preserve caller stream state

The helpers directly mutate caller stream flags and formatting settings:

- `print_color()` and `print_color_verbose()` switch numeric base and fill behavior
- `print_contrast()` sets `std::fixed` and `std::setprecision(1)`

Local validation confirmed the leak: after calling `print_contrast()` on a stream configured for scientific output,
appending `1.25` produced `1.2` rather than the caller's original formatting intent.

This makes the helpers awkward to compose in larger diagnostics or logs.

### 3. The public contract under-specifies output semantics

The docs correctly say that any color type is converted to `rgba8_t`, but they do not spell out several semantic
facts that matter to consumers:

- output is an sRGB-like terminal preview, not a faithful rendering of the original color model
- alpha is not rendered as transparency and is only shown numerically where applicable
- `print_color_verbose()` derives HSL from the converted RGB value, not from the original space

These are acceptable policies, but they should be explicit because users may otherwise assume richer color-space
awareness than the module actually provides.

### 4. The collection surface is narrower than the rest of the library

`print_palette()` accepts only `const Color*` plus `count`. That is workable, but it is a noticeably lower-level
surface than the rest of the library's modern type story.

This is not a correctness failure, but it is a real ergonomics gap:

- `std::array`, `std::vector`, and `std::span` users must drop down to pointer arithmetic
- tests and examples reinforce the pointer+count pattern instead of a safer non-owning view model

### 5. The tests prove presence, not contract boundaries

`tests/io/test_ansi.cpp` currently does a good job checking that escape codes and key substrings appear, but it does
not yet prove:

- behavior for `steps <= 1`
- stream-state restoration after formatter helpers run
- explicit policy for alpha handling in contrast and swatch previews
- behavior for zero-width or unusually small visual parameters

The test suite is therefore stronger as a regression net than as a public contract proof.

## Validated Evidence

The following evidence was validated locally during this audit:

- a focused runtime check showed that `print_contrast()` leaves the caller stream in fixed one-decimal formatting:
  after the helper ran, appending `1.25` produced `1.2`
- a focused runtime check with `steps == 1` showed that `print_gradient()` still emits output without any explicit
  boundary guard, confirming that the function currently relies on incidental arithmetic behavior instead of a defined
  one-step contract
- source inspection confirmed that the formatter helpers mutate stream flags directly and do not restore the original
  state before returning

## Design Goals

The revised module should:

1. remain lightweight and dependency-minimal
2. define edge behavior explicitly instead of relying on incidental floating-point outcomes
3. preserve caller stream formatting state
4. document the conversion-to-terminal-preview model truthfully
5. add ergonomics only where they keep the surface simple

## Recommended Direction

### Phase 1: Correctness hardening

Make the current helpers safe and explicit without changing the overall philosophy.

Recommended changes:

- define `print_gradient()` semantics for `steps == 0` and `steps == 1`
- add a small internal stream-state guard so helper functions restore flags, precision, and fill settings
- extend tests to cover stream hygiene and step-count boundaries

This is the highest-priority work in the module.

### Phase 2: Contract truth alignment

Update docs and examples so they describe the real rendering model:

- converted terminal preview rather than native-space rendering
- alpha displayed numerically rather than visually composited
- HSL in verbose mode derived from converted RGB

This is smaller than Phase 1, but it should happen in the same general cleanup window because the doc drift is part
of the public contract.

### Phase 3: Narrow ergonomics improvements

If the correctness pass lands cleanly, add one carefully chosen convenience layer:

- `std::span` overloads for palette-like helpers

This should stay intentionally small. `ansi` does not need to become a terminal UI subsystem.

## Out Of Scope

The following should stay out of the first cleanup wave:

- terminal feature detection
- automatic fallback for terminals without true-color support
- layout engines, legends, boxed tables, or full reporting UIs
- color-managed preview output beyond the current `rgba8_t` terminal model

## Success Criteria

The ANSI module is successful when:

- `print_gradient()` has explicit and tested semantics for small step counts
- formatter helpers restore caller stream state
- docs and examples describe the preview model truthfully
- at least one safe container-oriented palette overload exists, or the pointer+count policy is explicitly retained
  and documented
- tests prove the intended edge contracts rather than only substring presence

## Suggested Successor Plans

This design should be followed by:

- an `ansi` correctness hardening implementation plan
- optionally, a small `ansi` ergonomics and doc-alignment implementation plan if the cleanup is intentionally split
