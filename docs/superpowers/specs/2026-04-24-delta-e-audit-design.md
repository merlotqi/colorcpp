# Delta E Module Audit Design

## Overview

This document defines the design for a comprehensive audit of the `colorcpp` `delta_e` module.

The goal is to determine whether the current implementations are mathematically sound, sufficiently verified, clearly documented, and safe to recommend to users. The audit will not modify production code. It will produce an audit report plus prioritized remediation guidance.

All AI-generated artifacts for this work must live on the `ai/superpowers` branch. The code under review remains the `main` branch codebase.

## Goal

Produce a high-signal audit report that answers:

- Whether the `delta_e` module is currently trustworthy for normal use
- Which metrics or helper paths have the highest correctness risk
- Where tests, API semantics, or documentation are insufficient
- What should be fixed first, and why

## Scope

The audit covers these implementation units:

- `include/colorcpp/algorithms/delta_e/de76.hpp`
- `include/colorcpp/algorithms/delta_e/de94.hpp`
- `include/colorcpp/algorithms/delta_e/de2000.hpp`
- `include/colorcpp/algorithms/delta_e/cmc.hpp`
- `include/colorcpp/algorithms/delta_e/din99.hpp`
- `include/colorcpp/algorithms/delta_e/oklab.hpp`
- `include/colorcpp/algorithms/delta_e/simd.hpp`
- `include/colorcpp/algorithms/delta_e/helpers.hpp`
- `tests/algorithms/test_delta_e.cpp`
- `tests/algorithms/test_delta_e_simd.cpp`
- `docs/reference/delta_e.rst`

## Out of Scope

This audit does not:

- Refactor or repair the implementation
- Rework unrelated conversion code outside what is necessary to interpret `delta_e` behavior
- Re-benchmark the entire library
- Redefine project-wide API philosophy beyond findings directly relevant to `delta_e`

If the audit discovers issues that require code changes, those will be captured as follow-up recommendations rather than fixed immediately.

## Audit Sources Of Truth

The audit will use three sources of truth and compare them explicitly:

1. Repository implementation and tests
2. Public reference formulas and standard sample data where available
3. Public-facing repository documentation and usage guidance

Priority is given to primary or authoritative references when possible. In practice, this means:

- CIEDE2000: public Sharma/Wu/Dalal paper and public test data
- Oklab distance: Bjorn Ottosson's Oklab definition and W3C CSS Color references
- DE94 / CMC / DIN99: published formula definitions and reputable technical references where official standard text is not practically available

## Audit Approach

The audit will run in five stages.

### 1. Implementation Review

Each metric implementation will be read line by line to verify:

- Formula structure
- Parameter semantics
- Reference/comparison ordering for asymmetric metrics
- Numeric stability guards
- Internal precision choices
- Hidden assumptions in comments or defaults

### 2. Reference Comparison

Each implementation will be compared against public reference formulas and sample values.

Special emphasis goes to:

- `delta_e_2000()` against public CIEDE2000 sample data
- `delta_e_94()` and `delta_e_cmc()` asymmetry and default parameters
- `delta_e_din99()` formula variant correctness
- `delta_e_ok()` being accurately described as Euclidean Oklab distance rather than a standard CIE Delta E metric

### 3. Test Coverage Audit

Tests will be classified by what they actually prove.

The audit will distinguish between:

- Property tests such as symmetry, non-negativity, or monotonic intuition
- Golden numeric tests against published sample values
- Consistency checks between scalar and SIMD paths
- Weak tests that only confirm "runs" or "does not crash"

### 4. Engineering Audit

The audit will inspect:

- API clarity
- Risk of user misinterpretation
- Documentation accuracy
- Whether defaults imply domain-specific policy without being explicit
- Whether the SIMD path is validated in a way that justifies its presence

### 5. Priority And Follow-Up Guidance

All findings will be converted into a prioritized remediation list that can drive future fix work.

## Deliverables

The audit work will produce:

- A written audit report at `docs/superpowers/audits/2026-04-24-delta-e-audit.md`
- A prioritized findings table
- A recommended fix order
- Suggested follow-up work items suitable for later spec/plan cycles

## Report Structure

The audit report will use this structure:

### Executive Summary

Short summary of:

- Overall trust level
- Highest-risk areas
- Top recommended next actions

### Metric-By-Metric Findings

One section each for:

- `delta_e_76`
- `delta_e_94`
- `delta_e_2000`
- `delta_e_cmc`
- `delta_e_din99`
- `delta_e_ok`
- `delta_e_ok` SIMD path

Each section will contain:

- Implementation observations
- Reference comparison
- Test adequacy assessment
- Documentation/API concerns

### Cross-Cutting Findings

Shared problems that span multiple metrics, such as:

- Missing golden sample tests
- Ambiguous terminology
- Over-strong or under-precise documentation claims
- Inconsistent confidence levels across APIs

### Priority Table

Each finding will include:

- Finding statement
- Evidence
- Likely impact
- Suggested remediation
- Priority level

### Recommended Fix Order

Ordered sequence of highest-value next steps.

### Suggested Follow-Up Work

Potential fix batches or future spec topics.

## Priority Rules

Findings will be prioritized with these rules.

### P0

Use `P0` when there is strong evidence of a correctness defect that can directly produce wrong numerical results or materially misleading conclusions.

Examples:

- Clear mismatch against authoritative published sample data
- Formula implementation that is structurally wrong
- A bug that invalidates a metric's advertised behavior

### P1

Use `P1` when correctness is uncertain or fragile, even if not yet disproven.

Examples:

- Missing golden sample coverage for a sensitive formula
- Ambiguous parameter semantics that can easily cause misuse
- A formula variant that may be valid in some interpretations but is insufficiently justified or documented

### P2

Use `P2` for lower-severity engineering issues.

Examples:

- Documentation overstatement or underspecification
- Weak tests that should be strengthened
- SIMD maintenance cost not clearly justified by evidence

## Working Assumptions

- The audited code is the current `main` branch state
- The audit report itself is authored and stored on `ai/superpowers`
- The audit should favor evidence over opinion
- The audit should identify repairable issues, not merely criticize style

## Success Criteria

This design is successful if the resulting audit report:

- Clearly separates confirmed defects from plausible risks
- Uses published references where available
- Explains what current tests do and do not prove
- Produces an actionable fix priority list
- Can serve as a clean handoff into a later implementation-planning cycle

