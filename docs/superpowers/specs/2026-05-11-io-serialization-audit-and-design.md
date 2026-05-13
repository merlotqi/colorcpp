# IO Serialization Audit And Design

Date: 2026-05-11

## Summary

This document defines the audit and design direction for `include/colorcpp/io/serialization.hpp` and its JSON /
MessagePack adapter subheaders.

The current serialization layer is best understood as an adapter toolkit, not as a finished serialization subsystem.
The main problem is contract inflation: docs and examples describe tagged formats, semantic field names, binary
support, and broader completeness than the implementation actually delivers. The cleanest path is to align the public
contract with reality first, then add semantic naming and optional type tagging from a smaller, explicit foundation.

## Scope

This design covers:

- `include/colorcpp/io/serialization.hpp`
- `include/colorcpp/io/serialization/{details,traits,json,msgpack}.hpp`
- `tests/io/test_serialization.cpp`
- `examples/serialization_example.cpp`
- `examples/msgpack_example.cpp`
- `docs/reference/serialization.rst`
- the serialization sections in `README.md`

This design does not cover:

- library-specific adapter implementations for third-party JSON or MessagePack libraries
- wire-compatibility guarantees with any external schema standard
- CSS serialization or parsing

## Inputs Reviewed

The design is based on the current repository state, including:

- `include/colorcpp/io/serialization/*`
- `tests/io/test_serialization.cpp`
- `docs/reference/serialization.rst`
- `README.md`
- `examples/serialization_example.cpp`
- `examples/msgpack_example.cpp`

## Current State

The current layer provides:

- `serialization_format` and `serialization_options`
- a `color_space_id` enum plus `color_space_name()`
- a generic `json_adapter<Json>` specialization point
- JSON helpers for compact arrays and named objects
- MessagePack pack/unpack traits for array and map workflows
- channel extraction / application helpers that normalize integer channels to `double`

This is enough to support custom integrations, but it is not yet a complete or uniform public serialization story.

## Current State Assessment

Module state: `Transitional And Overstated`

Why:

- the adapter pattern is a solid low-dependency foundation
- JSON helpers are usable for compact and caller-named object output
- MessagePack support exists only as a lower-level pair of packer/unpacker hooks
- docs describe a larger and more self-describing contract than the code actually implements

## Confirmed Findings

### 1. `serialization_options::include_color_space` is currently dead configuration

`serialization_options` exposes:

- `format`
- `include_color_space`

The JSON and MessagePack serialization helpers currently branch on `format`, but they do not use
`include_color_space` anywhere. That means the public options type advertises a capability that does not exist.

This is not a documentation nit. It is dead public API surface.

### 2. The docs claim tagged and binary formats that the code does not implement

`docs/reference/serialization.rst` currently describes:

- compact array mode
- named object mode with semantic names such as `r`, `g`, `b`, `a`
- type-tagged mode with embedded color-space information
- binary serialization as a complete supported format

The codebase currently implements:

- compact array output
- named object output using either caller-provided names or default `ch0`, `ch1`, ...
- no built-in tagged JSON format
- no binary serializer implementation in this module

This is the strongest documentation drift in the non-CSS `io` stack.

### 3. Default named serialization is generic rather than semantic

When the caller asks for `serialization_format::named` without custom channel names, JSON serialization currently
emits generic keys:

- `ch0`
- `ch1`
- `ch2`

That is internally consistent, but it does not match the user-facing examples and docs that imply semantic names such
as `r`, `g`, `b`, `a`, `h`, `s`, `l`, and so on.

The current implementation is therefore lower-level than the docs suggest.

### 4. MessagePack is materially weaker than JSON

The MessagePack helpers provide:

- `pack_color()`
- `pack_color_named()`
- `unpack_color()`
- `unpack_color_named()`

They do not currently provide:

- a top-level options-based API analogous to `to_json()`
- a default semantic naming layer
- an integrated type-tagging story
- a clearly documented symmetry with the JSON surface

This is acceptable for a low-level toolkit, but it is not a `complete` parallel serialization surface.

### 5. `color_space_id` is incomplete and disconnected from actual serialization flow

The module defines a `color_space_id` enum, but it currently has several design gaps:

- it does not map from color types through any public trait
- it omits some public color variants such as linear Display P3 and float CMYK
- it is not integrated into JSON or MessagePack serialization paths

This makes it feel like the beginning of a richer metadata system rather than a finished one.

### 6. The tests validate helpers, not the documented contract

`tests/io/test_serialization.cpp` currently checks:

- enum values
- option defaults
- channel normalization helpers

It does not yet prove:

- real JSON adapter round trips
- named-format defaults
- color-space tagging behavior
- MessagePack surface parity
- the truth or falsity of the doc claims

The current suite is therefore much stronger as a traits smoke test than as a public contract proof.

## Validated Evidence

The following evidence was validated locally during this audit:

- repository search showed `include_color_space` is defined in `serialization_options` but has no implementation use
  in the JSON or MessagePack helpers
- `docs/reference/serialization.rst` explicitly claims `compact / named / tagged format`, `Type tagged mode`, and a
  `Binary | Complete` support row, while the current public headers only expose array/object JSON helpers and
  MessagePack pack/unpack helpers
- source inspection confirmed that `color_space_id` currently exists as an enum and naming helper, but is not wired
  into the active JSON or MessagePack serialization flow

## Design Goals

The revised serialization layer should:

1. present an honest public contract
2. converge JSON and MessagePack around the same conceptual options model
3. support semantic channel naming through explicit traits
4. use symmetric adapter-oriented header names that avoid generic third-party collisions
5. add color-space tagging only when the underlying metadata model is real
6. keep the core library dependency-free

## Recommended Direction

### Phase 1: Truth alignment and scope narrowing

The first phase should align docs, examples, and public names with what the code really is today.

Recommended changes:

- describe the current system as an adapter toolkit rather than a turnkey serializer
- remove or downgrade claims about tagged and binary support until those capabilities exist
- make the JSON named-mode default story explicit
- define the current MessagePack surface as lower-level rather than fully parallel
- rename generic format headers to adapter-oriented names to avoid collisions with third-party headers; specifically,
  `json.hpp` should become `json_adapter.hpp`, and `msgpack.hpp` should be renamed to a matching peer such as
  `msgpack_adapter.hpp`

This is the most urgent work because the documentation currently over-promises.

### Phase 2: Add semantic naming traits

Introduce a public trait layer for semantic metadata:

- `channel_names<Color>` or equivalent
- `color_space_of<Color>` or equivalent

That would allow named serialization to emit semantic keys by default and would provide the foundation for any future
type-tagged mode.

This phase is the correct place to connect `color_space_id` to actual types.

### Phase 3: Converge the JSON and MessagePack APIs

Once the metadata traits exist:

- add options-based MessagePack entry points
- make default naming behavior consistent across JSON and MessagePack
- decide whether tagged output belongs in both formats or only in JSON first

The goal is conceptual parity, not forced identical mechanics.

### Phase 4: Decide whether binary support belongs here

The docs currently claim binary completeness, but the code does not implement it.

A deliberate decision is needed:

- either add a real binary serialization submodule under `io::serialization`
- or remove the claim and keep binary structured I/O concerns separate from this adapter layer

The recommended default is to remove the claim until a concrete use case and format are specified.

## Out Of Scope

The following should stay out of the first cleanup wave:

- bundling third-party JSON libraries into core
- schema negotiation or versioned wire formats
- reflection-heavy or macro-heavy automatic adapter generation
- a speculative binary format without a clear consumer

## Success Criteria

The serialization layer is successful when:

- docs describe only capabilities that actually exist
- `include_color_space` is either implemented or removed
- semantic channel names come from explicit traits rather than from contradictory prose
- JSON and MessagePack are clearly positioned as either equivalent or intentionally layered differently
- tests prove real round-trip and naming behavior instead of only helper mechanics

## Suggested Successor Plans

This design should be followed by:

- a serialization contract-alignment implementation plan
- optionally, a second serialization metadata and tagging plan once the truthful baseline is in place
