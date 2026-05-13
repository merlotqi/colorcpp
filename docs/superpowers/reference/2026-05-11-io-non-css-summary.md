# Non-CSS IO Summary Reference

Date: 2026-05-11

## Purpose

This document is the landing-page summary for the non-CSS `colorcpp::io` audit work captured on the
`ai/superpowers` branch.

It covers:

- `ansi`
- `literals`
- `serialization`
- `binary_io`

It does not cover `colorcpp::io::css`, which was intentionally excluded from this review.

## Spec Index

- [`2026-05-11-io-ansi-audit-and-design.md`](../specs/2026-05-11-io-ansi-audit-and-design.md)
- [`2026-05-11-io-literals-audit-and-design.md`](../specs/2026-05-11-io-literals-audit-and-design.md)
- [`2026-05-11-io-serialization-audit-and-design.md`](../specs/2026-05-11-io-serialization-audit-and-design.md)
- [`2026-05-11-io-binary-io-audit-and-design.md`](../specs/2026-05-11-io-binary-io-audit-and-design.md)

## Cross-Module Assessment

| Module | State | Main problem | Priority |
| --- | --- | --- | --- |
| `ansi` | `Usable But Fragile` | edge-case math and stream-state hygiene are under-specified | Medium |
| `literals` | `Useful But Inconsistent` | confirmed correctness and validation bugs plus unclear coverage policy | High |
| `serialization` | `Transitional And Overstated` | docs and options promise more than the implementation provides | High |
| `binary_io` | `Useful But High-Risk` | confirmed correctness bugs and weak parser diagnostics | Highest |

## Confirmed Critical Findings

The following issues were confirmed during local inspection and targeted validation:

1. `binary_io::apply_color()` does not preserve integer RGB semantics and cannot honestly claim to work for any color
   type with `r()`, `g()`, `b()`.
2. `binary_io::apply(const lut3d&, ...)` overflows when `lut3d.size == 1`.
3. `_oklch` literal chroma scaling is incompatible with the `core::oklch_t` range.
4. `_hex` string literals accept invalid hexadecimal digits and coerce them to zero instead of rejecting input.
5. `serialization_options::include_color_space` is currently dead configuration.
6. `docs/reference/serialization.rst` claims tagged and binary support that the code does not currently implement.
7. `ansi::print_contrast()` leaks caller stream formatting state.

## Cross-Cutting Themes

Three patterns repeat across the non-CSS `io` modules:

### 1. Contract drift is more serious than raw feature count

The largest problem is not simply that features are missing. The larger problem is that several modules describe a
cleaner or broader public story than the implementation actually supports.

Examples:

- `serialization` docs describe tagged and binary support that is not implemented
- `binary_io` docs describe a more generic `apply_color()` contract than the implementation supports
- `literals` docs describe alpha Oklab/OkLCH suffixes that do not exist

### 2. The test suites are stronger on happy paths than on boundary contracts

Existing tests are useful and should be preserved, but the most important gaps are concentrated around:

- malformed input
- tiny or degenerate shapes
- doc/API parity
- stream or formatting hygiene

### 3. The best next work is mostly hardening and truth alignment

None of these modules should begin with broad surface expansion. The best early work is:

- fix confirmed correctness bugs
- narrow or clarify misleading contracts
- strengthen tests at the exact edges where the bugs were found

## Recommended Execution Order

1. `binary_io`
   Reason: it contains the highest-risk correctness and safety issues.
2. `literals`
   Reason: it has user-facing authoring bugs and weak invalid-input handling.
3. `serialization`
   Reason: the docs and options surface need truth alignment before new capability work.
4. `ansi`
   Reason: it is useful already, and the remaining work is mostly hardening rather than existential repair.

## Execution Sequence And Branch Names

The recommended implementation sequence is intentionally linear. Each branch should start from updated `main` after
the previous branch has been merged so that correctness hardening lands in the same order as the audit priority.

| Order | Module | Recommended branch name | Current status | Why this branch goes here |
| --- | --- | --- | --- | --- |
| 1 | `binary_io` | `feature/io-binary-io-hardening` | Completed | fixes the highest-risk correctness and safety bugs first |
| 2 | `literals` | `feature/io-literals-correctness` | Completed | repairs public authoring bugs after the LUT foundation work is stable |
| 3 | `serialization` | `feature/io-serialization-contracts` | Current next branch | aligns docs and contract surface after the lower-level correctness fixes land |
| 4 | `ansi` | `feature/io-ansi-hardening` | Pending | finishes with targeted hardening and stream-hygiene cleanup |

Current execution point:

- `feature/io-binary-io-hardening` is complete
- `feature/io-literals-correctness` is complete
- continue with `feature/io-serialization-contracts`
- keep `feature/io-ansi-hardening` as the final branch in this sequence

Execution rule:

1. finish and merge `feature/io-binary-io-hardening`
2. refresh local `main`
3. finish and merge `feature/io-literals-correctness`
4. refresh local `main`
5. create and finish `feature/io-serialization-contracts`
6. merge it
7. refresh local `main`
8. create and finish `feature/io-ansi-hardening`

## Suggested Plan Breakdown

The cleanest follow-up sequence is:

1. completed `binary_io` correctness hardening on `feature/io-binary-io-hardening`
2. completed `literals` correctness and doc-alignment on `feature/io-literals-correctness`
3. `serialization` contract-alignment plan on `feature/io-serialization-contracts`
4. `ansi` correctness hardening plan on `feature/io-ansi-hardening`
5. optional second-wave plans for diagnostics, metadata/tagging, and ergonomics

## Short Conclusion

The non-CSS `io` layer is not empty or unusable. It already contains several genuinely useful modules. The main
problem is that the public story is not equally mature everywhere.

The right near-term strategy is:

- stabilize `binary_io`
- repair and narrow `literals`
- make `serialization` honest before making it bigger
- harden `ansi` without overbuilding it

The linked module specs above are the source-of-truth detail documents for that work.
