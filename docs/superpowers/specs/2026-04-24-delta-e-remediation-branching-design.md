# Delta E Remediation Branching Design

## Overview

This document defines the execution design for follow-up `delta_e` remediation work after the completed audit.

The remediation will be split into two separate feature branches and worktrees:

- `feature/delta-e-validation`
- `feature/delta-e-fixes`

The goal of the split is to keep `main` clean, keep each feature branch single-purpose, and avoid mixing evidence-gathering with implementation repair.

All AI-generated planning artifacts for this work remain on `ai/superpowers`. The feature branches are reserved for production code, tests, and formal project documentation only.

## Goal

Deliver a two-stage remediation flow that:

- locks in trustworthy validation for `delta_e` behavior before implementation changes
- keeps the validation work independently reviewable and mergeable
- defers `delta_e_din99()` implementation repair until after the validation branch has been merged
- preserves a clean GitHub merge path for both stages

## Branch Strategy

The remediation uses two sequential feature branches.

### Stage 1

- Branch: `feature/delta-e-validation`
- Worktree: `.worktrees/delta-e-validation`
- Base branch: `main`

This branch exists only to strengthen tests and validation for the audited `delta_e` module while remaining green and mergeable on its own.

### Stage 2

- Branch: `feature/delta-e-fixes`
- Worktree: `.worktrees/delta-e-fixes`
- Base branch: updated `main` after `feature/delta-e-validation` is merged

This branch exists only to repair implementation issues already justified by the validation work, with `delta_e_din99()` as the primary target.

## Merge Model

Both feature branches are intended to be merged through GitHub, not by local direct merge to `main`.

The required merge order is:

1. Create and complete `feature/delta-e-validation`
2. Open and merge that branch through GitHub
3. Refresh local `main`
4. Create `feature/delta-e-fixes` from the updated `main`
5. Complete and merge `feature/delta-e-fixes` through GitHub

This order prevents speculative implementation repair from getting ahead of the validated evidence base.

## Scope

### In Scope For `feature/delta-e-validation`

- add published Sharma vector tests for `delta_e_2000()`
- add `delta_e_94()` reference-order contract tests
- add `delta_e_cmc()` reference-order contract tests
- add `delta_e_94()` and `delta_e_cmc()` numeric or conformance tests where those cases are compatible with current intended behavior and do not force unrelated implementation changes
- make minimal supporting test-only adjustments if required
- make minimal formal-doc updates only when needed to clarify the tested contract

### Out Of Scope For `feature/delta-e-validation`

- changing `include/colorcpp/algorithms/delta_e/din99.hpp`
- adding intentionally failing `DIN99` strict conformance tests that would leave the branch unmergeable
- broad doc rewrites based on audit conclusions
- unrelated cleanup or refactoring

### In Scope For `feature/delta-e-fixes`

- add strict `DIN99` vector tests that capture the intended published behavior
- observe and document the current `DIN99` failure mode through tests
- repair `include/colorcpp/algorithms/delta_e/din99.hpp`
- add any narrow regression coverage directly required by the repair
- update `docs/reference/delta_e.rst` so `DIN99` claims match the repaired and tested behavior

### Out Of Scope For `feature/delta-e-fixes`

- reopening the already-merged validation scope unless the `DIN99` repair truly requires it
- bundling unrelated `delta_e` improvements into the same branch
- moving AI planning documents onto feature branches

## Execution Rules

The following rules are part of the design and not optional implementation details.

### Validation Branch Rules

- the branch must stay green and mergeable
- all added tests must pass on the branch before merge
- the branch may strengthen contracts, but it must not depend on a later implementation repair to succeed
- if a proposed validation addition would intentionally turn the branch red, it belongs in `feature/delta-e-fixes` instead

### Fix Branch Rules

- the branch may begin with new `DIN99` tests that fail against the inherited implementation
- the branch must finish green
- implementation changes must be driven by the newly added `DIN99` evidence rather than speculative formula rewriting
- doc changes must stay tightly coupled to the repaired behavior

## Expected File Touches

### `feature/delta-e-validation`

Primary files:

- `tests/algorithms/test_delta_e.cpp`

Possible secondary files:

- `docs/reference/delta_e.rst`

### `feature/delta-e-fixes`

Primary files:

- `include/colorcpp/algorithms/delta_e/din99.hpp`
- `tests/algorithms/test_delta_e.cpp`

Possible secondary files:

- `docs/reference/delta_e.rst`

## Success Criteria

### Validation Branch Success

The validation branch is successful when:

- Sharma `delta_e_2000()` vector coverage is checked into the repository
- `delta_e_94()` and `delta_e_cmc()` reference-order semantics are protected by real assertions
- any added conformance coverage for `delta_e_94()` and `delta_e_cmc()` passes on the branch
- the branch remains clean, reviewable, and mergeable through GitHub without requiring later implementation repair

### Fix Branch Success

The fix branch is successful when:

- `DIN99` strict vector tests are checked in
- `delta_e_din99()` passes those tests
- prior `delta_e` behavior does not regress in the strengthened suite
- `docs/reference/delta_e.rst` no longer claims more certainty for `DIN99` than the implementation and tests support

## Risks And Tradeoffs

The main tradeoff in this design is that `DIN99` evidence and `DIN99` repair are intentionally separated across branches. That adds one more GitHub merge step, but it buys much clearer boundaries:

- the validation branch proves what the project can safely merge today
- the fix branch isolates the risky implementation work
- `main` never needs to absorb intentionally failing validation commits

This design deliberately favors clean history and review clarity over raw speed.

## Working Assumptions

- `main` should remain clean and mergeable at every stage
- GitHub is the merge authority for both feature branches
- `ai/superpowers` remains the home for AI specs, plans, and audits
- the existing audit report is the source of truth for why this remediation is needed
- `delta_e_din99()` is the only currently suspected implementation defect serious enough to justify a dedicated repair branch

## Successor Documents

This design is expected to be followed by:

- an implementation plan for `feature/delta-e-validation`
- later, after that branch is merged, a separate implementation plan for `feature/delta-e-fixes`
