# IO Binary IO Audit And Design

Date: 2026-05-11

## Summary

This document defines the audit and design direction for `include/colorcpp/io/binary_io.hpp` and its LUT / `.cube`
subheaders.

The `binary_io` module has a solid happy-path core: simple LUT data structures, working `.cube` read/write support,
and straightforward interpolation helpers. The problem is that the generic application contract and edge-case safety
do not yet match the public wording. Two correctness issues were confirmed locally, and the parser surface still
collapses all read failures into a low-information `std::monostate` result.

## Scope

This design covers:

- `include/colorcpp/io/binary_io.hpp`
- `include/colorcpp/io/binary_io/{details,lut,cube}.hpp`
- `tests/io/test_binary_io.cpp`
- `docs/reference/binary_io.rst`
- the binary I/O section in `README.md`

This design does not cover:

- CSS color parsing
- image file formats
- arbitrary binary blob serialization
- GPU LUT runtime integration

## Inputs Reviewed

The design is based on the current repository state, including:

- `include/colorcpp/io/binary_io/*`
- `tests/io/test_binary_io.cpp`
- `docs/reference/binary_io.rst`
- `README.md`
- the current LUT-related examples and comments

## Current State

The module currently provides:

- `lut1d`
- `lut3d`
- `apply(lut, r, g, b)` overloads for `lut1d` and `lut3d`
- `apply_color(lut, color)` overloads that operate on any color type with `r()`, `g()`, `b()`
- `.cube` read/write helpers with 1D / 3D auto-detection

That is enough for a basic LUT workflow, but several parts of the contract are stronger in prose than they are in
actual behavior.

## Current State Assessment

Module state: `Useful But High-Risk`

Why:

- core data structures are easy to understand
- `.cube` happy-path round trips are already covered
- generic application and boundary behavior are not safe enough yet
- parser diagnostics are too weak for a format-focused I/O layer

## Confirmed Findings

### 1. `apply_color()` does not uphold its “any color type” contract

The public docs say that `apply_color()` works for any color type with `r()`, `g()`, and `b()` accessors. The
implementation currently reads and writes those channels directly using normalized float LUT values.

Local validation confirmed the mismatch:

- applying an identity `lut1d` to `core::rgba8_t{255, 0, 0, 255}` produced `1, 0, 0, 255`

That means the helper is not generic over channel representation. It implicitly assumes normalized float-like RGB
channels even though the docs say otherwise.

### 2. `lut3d` interpolation overflows when `size == 1`

`apply(const lut3d&, ...)` computes a clamped base index using `sz - 2` when the input index lands at the end of the
grid. When `size == 1`, that path underflows and leads to invalid indexing.

Local validation under AddressSanitizer reproduced a heap-buffer-overflow for a `1x1x1` LUT.

This is a real safety bug.

### 3. `.cube` parsing is low-information on failure

`cube::read()` returns:

- `lut1d`
- `lut3d`
- `std::monostate`

Malformed lines, invalid directives, insufficient data, and unsupported shape combinations all collapse into the same
no-detail failure result. The parser also tends to ignore malformed directive content silently and continue.

This is acceptable for a quick internal helper, but it is thin for a public file-format module.

### 4. The docs overstate the genericity of LUT application

`docs/reference/binary_io.rst` currently describes:

- `binary_io::apply_color(lut, color)` — apply LUT to any color type

That statement is too strong given the current implementation. The helper either needs a narrower contract or a more
explicit conversion-based implementation.

### 5. The module shape implies broader format ambitions than the code delivers

`lut.hpp` describes LUT data structures used by industry-standard formats and mentions formats such as `.csp` in the
header comments, while the actual parser/writer surface currently supports only DaVinci Resolve `.cube`.

This is not a bug by itself, but it does create a public impression of broader format readiness than the code
provides.

### 6. The tests miss the highest-risk file and interpolation edges

`tests/io/test_binary_io.cpp` does a useful job covering:

- valid 1D and 3D construction
- basic identity interpolation
- `.cube` read/write round trips

It does not yet cover:

- `lut3d.size == 1`
- `apply_color()` with integer RGB channel types
- malformed directives or incomplete data blocks
- failure diagnostics because the module currently has none

The strongest bugs are therefore exactly the ones the suite does not currently pin down.

## Validated Evidence

The following evidence was validated locally during this audit:

- applying an identity `lut1d` through `apply_color()` to `core::rgba8_t{255, 0, 0, 255}` produced `1,0,0,255`,
  confirming the generic color contract mismatch
- running a focused AddressSanitizer check against a `1x1x1` `lut3d` reproduced a heap-buffer-overflow inside
  `apply(const lut3d&, ...)`
- source inspection confirmed that `.cube` parser failures currently collapse to `std::monostate` without structured
  error detail

## Design Goals

The revised binary I/O layer should:

1. make LUT application semantics explicit and safe
2. define boundary behavior for tiny LUTs
3. provide enough parse diagnostics for format debugging
4. keep the happy-path API simple
5. avoid promising future format breadth before the foundation is solid

## Recommended Direction

### Phase 1: Correctness hardening

The first phase should eliminate the two confirmed correctness bugs.

Recommended changes:

- define safe `size == 1` behavior for `lut3d`
- narrow `apply_color()` to the channel model it actually supports, or rename/split it so the contract is honest
- add focused tests for both behaviors

This is the highest-priority work in the module.

### Phase 2: Contract truth alignment

Update the docs and comments so they no longer imply broader guarantees than the implementation provides.

Recommended changes:

- state whether `apply_color()` is for normalized RGB-like types only
- remove or soften future-format wording that suggests `.csp`-level support
- keep `.cube` as the explicit and only format promise until more formats exist

### Phase 3: Parser diagnostics

Once correctness is stabilized, improve the file-format experience:

- return structured error information rather than only `std::monostate`
- capture at least enough detail to distinguish open failure, directive error, and data-shape mismatch
- add malformed-input tests that prove those diagnostics

This phase is important, but it should follow the correctness hardening rather than precede it.

## Out Of Scope

The following should stay out of the first cleanup wave:

- adding multiple new LUT file formats
- building a full format registry
- GPU upload helpers or image-processing pipelines
- speculative color-management features unrelated to `.cube`

## Success Criteria

The binary I/O layer is successful when:

- `lut3d` application is safe for `size == 1`
- `apply_color()` has an honest and tested contract
- `.cube` parser failures provide actionable diagnostic information
- docs promise only `.cube` support unless more formats are actually implemented
- tests cover the highest-risk interpolation and parser edges

## Suggested Successor Plans

This design should be followed by:

- a binary I/O correctness hardening implementation plan
- optionally, a second parser diagnostics and format-roadmap implementation plan
