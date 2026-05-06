# Operations Audit And Roadmap Design

Date: 2026-05-06

## Summary

This document defines a repository-wide audit and evolution plan for the `include/colorcpp/operations` layer.
It covers `compare`, `conversion`, `random`, `blend`, `interpolate`, and `palette`, plus the documentation,
examples, debug helpers, and tests that collectively define the public contract of those modules.

The main conclusion is that `operations` is not blocked by broad correctness failures. The currently built
`operations` test subset reviewed during this audit passed locally, and several modules are already usable. The
larger problem is semantic drift: public descriptions, helper layers, and tests often imply stronger guarantees
than the implementation actually provides. The highest-value work is therefore to align truth first, converge the
foundation modules second, and only then expand the surface area.

## Scope

This design covers:

- Public headers under `include/colorcpp/operations`
- Supporting docs under `docs/reference` and `.worktrees/ai-superpowers/docs/reference`
- README and examples that describe or demonstrate `operations`
- Debug and introspection helpers that shape the conversion story
- Existing unit tests and their proof strength

This design does not cover:

- Rewriting unrelated `algorithms` modules outside what `operations` depends on
- Large API expansion work in the same phase as semantic cleanup
- Immediate code changes; this document is a design and sequencing artifact

## Inputs Reviewed

The design is based on the current repository state, including:

- `include/colorcpp/operations/*`
- `include/colorcpp/operations/conversion/{cast_impl,graph,traits,debug,color_space_registry}.hpp`
- `include/colorcpp/operations/compare/*`
- `include/colorcpp/operations/random/*`
- `include/colorcpp/operations/blend/*`
- `include/colorcpp/operations/interpolate/*`
- `include/colorcpp/operations/palette/*`
- `docs/reference/{compare,conversion,random,blend,interpolate,palette}.rst`
- `.worktrees/ai-superpowers/docs/reference/{compare,conversion,random}.rst`
- `tests/operations/*`
- `README.md`

## Audit Framework

Each module is evaluated against five dimensions:

1. Semantic truthfulness
2. API coherence
3. Architectural convergence
4. Test proof strength
5. Extension readiness

Each module is then assigned one of four states:

- `Stable`
- `Usable But Underspecified`
- `Transitional`
- `Misleading Surface`

## Current State Overview

| Module | State | Why |
| --- | --- | --- |
| `random` | `Stable / Usable But Underspecified` | Core behavior is relatively honest and recent constrained generators are directionally solid, but some guarantees remain underspecified. |
| `compare` | `Usable But Underspecified` | Useful helpers exist, but they do not yet form a complete comparison layer and public wording overstates clarity. |
| `conversion` | `Transitional` | Main dispatch is graph-driven, while docs, traits, debug helpers, and tests still carry hub-era public semantics. |
| `blend` | `Misleading Surface` | Public docs claim linearized sRGB behavior that the visible implementation does not actually enforce. |
| `interpolate` | `Usable But Underspecified` | Rich feature set exists, but the surface is organized more by helper count than by explicit semantic model. |
| `palette` | `Misleading Surface / Usable But Underspecified` | Structural palette APIs and design-flavored heuristics are mixed together under one story. |

## Cross-Cutting Findings

### 1. Public contract drift is the dominant problem

The repo already has many passing tests and several usable APIs. The larger issue is that public descriptions and
helper layers often say something stronger or cleaner than the implementation actually does. Examples include:

- `compare` documentation describes perceptual comparison in Oklab terms, but `perceptual_policy` currently calls
  `algorithms::delta_e::delta_e_2000()`, which converts through `core::cielab_t`.
- `blend` documentation says blending is performed in linearized sRGB, while `blend::blend()` visibly converts both
  inputs to `core::rgbaf_t` and computes there.
- `conversion` documentation still describes a four-level routing system with hub fallback, while `color_cast` now
  dispatches through graph availability and errors otherwise.

### 2. Foundation ambiguity leaks upward

`blend`, `interpolate`, and parts of `palette` depend on implicit decisions about working space, alpha handling,
and conversion semantics. As long as `compare` and `conversion` are underspecified, upper-layer modules will keep
inheriting that ambiguity.

### 3. Tests are often green but do not prove public claims

The current tests are useful as regression checks, but many of them prove only that helpers run and return values in
reasonable ranges. They less often prove that:

- the documented working space is real
- a named perceptual policy maps to a clearly described metric
- a route explanation matches the actual conversion engine
- heuristic APIs are presented as heuristics rather than standards-faithful systems

### 4. Presentation artifacts are part of the architecture

README, docs, examples, and debug helpers are not secondary polish items here. They are part of the user-facing
model of `operations`, and several of the current design gaps are present precisely because those layers are out of
sync with the implementation.

## Module Audits

### Compare

#### Current shape

`compare` currently exposes:

- `exact_equal`
- `epsilon_equal`
- `relative_equal`
- `perceptual_equal`
- `equal()` and `equal<Policy>()`

This is a policy-based equality surface for same-type comparisons.

#### Main shortcomings

- The surface is narrower than its naming suggests. It is mostly equality helpers, not a full comparison layer.
- Cross-type comparison is not part of the primary API story. The existing test converts manually before comparing.
- The perceptual path hides the metric choice behind a generic name.
- Equality and distance are coupled conceptually but separated awkwardly in practice across `operations::compare`
  and `algorithms::delta_e`.

#### Doc and contract gaps

- `docs/reference/compare.rst` says perceptual comparison converts to Oklab internally, but the implementation uses
  `delta_e_2000()`, which works through CIELAB.
- The docs recommend policies by use case, but the API does not make metric identity and threshold meaning explicit
  enough to support those recommendations confidently.

#### Test gaps

- Tests focus on same-type channel comparisons.
- There is no primary public test for cross-type `compare` behavior because that behavior is not yet first-class.
- There is no contract test that binds the perceptual policy to a named metric and threshold interpretation.

#### Recommended direction

Evolve `compare` from policy-based same-type equality helpers into a comparison facade over explicit metrics and
tolerances:

- Keep exact and numeric closeness helpers
- Add a first-class cross-type comparison path
- Make perceptual metric choice explicit in names or types
- Separate boolean predicates from distance-returning APIs

#### Priority

High. `compare` is foundational and currently too underspecified to support clean extension.

### Conversion

#### Current shape

`color_cast` now dispatches through graph reachability in `cast_impl.hpp`, using registered edges and compile-time
shortest-path routing. Graph nodes are defined in `graph.hpp`, with `additional_color_nodes` as the user extension
point.

At the same time:

- `traits.hpp` still models recursive hub-based costs
- `color_space_registry.hpp` documents hub-tree routing as the conversion priority model
- `debug.hpp` exposes both graph and hub-era route concepts
- docs and tests still speak in mixed graph/hub language

#### Main shortcomings

- There is no single public source of truth for how conversion works.
- Old hub vocabulary survives in places that users are expected to read and trust.
- Debug and introspection helpers describe a richer or different routing story than `color_cast` itself uses.
- Some tests validate hub-derived cost helpers even though the execution path is graph-first and graph-only at the
  main dispatch layer.

#### Doc and contract gaps

- `docs/reference/conversion.rst` still documents a four-level routing architecture with hub fallback.
- `color_space_registry.hpp` and `traits.hpp` read like live public routing rules, not compatibility leftovers.
- Public examples imply a cleaner and more uniform route model than the source tree currently provides.

#### Test gaps

- Existing tests do cover useful path selection facts, especially weighted graph routing.
- They do not yet fully separate "actual dispatch behavior" from "legacy explanatory helpers."
- There is not yet a clean contract suite that proves the one public routing model users are meant to rely on.

#### Recommended direction

Choose one public model and converge everything around it.

Recommended choice: `graph-only public routing model`.

Under that model:

- `color_cast` remains graph-driven
- registered edges and weights remain the primary vocabulary
- hub concepts may remain internally as authoring guidance or graph-construction intuition, but they stop being part
  of the public dispatch contract
- `traits.hpp`, `debug.hpp`, docs, and tests are rewritten to describe graph semantics consistently

Alternative, lower-confidence option: retain hub routing as a formal compatibility layer. This is less attractive
because it keeps two public mental models alive.

#### Priority

Highest in the whole `operations` stack. Every upper-layer module depends on reliable conversion semantics.

### Random

#### Current shape

`random` now includes:

- basic per-channel generators
- HSL/HSV/HWB generators
- golden-angle and harmony generators
- constrained luminance and contrast generators

The documentation around thread safety and perceptual-space caveats is comparatively honest.

#### Main shortcomings

- The module is stronger than the others on truthfulness, but its guarantees are still not fully productized.
- Constrained generators rely on rejection sampling and gamut mapping, yet exhaustion behavior and quality guarantees
  are not framed as contract-level semantics.
- The surface mixes "any channel box" sampling with perceptual-space constrained sampling without a stronger taxonomy.

#### Doc and contract gaps

- Docs are relatively good, especially around thread safety and gamut caveats.
- The main missing piece is a clearer product model: basic random, constrained random, and perceptual random should be
  treated as distinct capability families.

#### Test gaps

- Tests check reproducibility, basic constraints, and option validation.
- They do not yet prove stronger properties such as distribution quality, gamut-map policy invariants, or exhaustion
  semantics under impossible constraints.
- The current local build directory also shows stale CTest discovery state for `test_random_constrained`, which is a
  reminder that build/test synchronization should be treated as part of contract hygiene.

#### Recommended direction

Keep the current implementation strategy, but clarify the module taxonomy:

- basic random sampling
- constrained random sampling
- perceptual or gamut-aware random sampling

Future extensions should build on that taxonomy rather than adding more ad hoc generators.

#### Priority

Lower than `conversion`, `compare`, `blend`, and `palette`. This module is relatively close to being extendable.

### Blend

#### Current shape

`blend` offers separable and non-separable blend modes with optional SIMD acceleration for selected modes.

#### Main shortcomings

- The documented working-space story does not match the visible implementation.
- The module is presented as standards-aligned compositing, but the public contract does not clearly separate:
  standards-faithful behavior, engineering approximation, and current implementation scope.
- Space choice, alpha model, and non-separable mode semantics deserve to be first-class contract decisions.

#### Doc and contract gaps

- `docs/reference/blend.rst` says blending is performed in linearized sRGB.
- `include/colorcpp/operations/blend/blend.hpp` converts through `core::rgbaf_t` and computes directly there.
- The docs therefore currently overstate the physical or standard-faithful semantics of the result.

#### Test gaps

- Tests verify numeric formulas against the current implementation.
- They do not prove linear-space behavior because the implementation does not actually expose such behavior.
- There is no contract suite that distinguishes standard-aligned claims from current approximations.

#### Recommended direction

Make working-space policy explicit.

The module should eventually expose or clearly document one of these directions:

- a true linear-sRGB compositing default
- an explicit legacy/display-space blend mode family
- a policy-based choice of working space

The important design requirement is not "more blend modes." It is "truthful and explicit compositing semantics."

#### Priority

High, after `conversion` and `compare`. The gap between contract and implementation is material.

### Interpolate

#### Current shape

`interpolate` exposes many helpers:

- RGB, HSL, HSV, Oklab, Oklch, Lab, and LCh interpolation
- multi-stop helpers
- easing helpers
- spline and path helpers
- hue-locked and chroma-preserving variants

#### Main shortcomings

- The surface is feature-rich but semantically flat.
- Users pick helpers by function count rather than by a clearly explained model of space, curve type, alpha handling,
  and gamut behavior.
- Several helpers interpolate color in one space but recover alpha through `rgbaf_t`, which is valid as an
  implementation choice but insufficiently surfaced as contract.

#### Doc and contract gaps

- Docs say what helpers exist, but the semantic grouping is still weak.
- The module should help users answer "what kind of interpolation am I choosing?" before "which function name do I
  call?"

#### Test gaps

- Tests cover many helpers and are a strong regression net.
- They still skew toward endpoint and shape sanity rather than a more explicit semantic matrix.
- The current docs do not promise enough detail for the tests to serve as proof of a richer public model.

#### Recommended direction

Reframe `interpolate` around semantic axes:

- working space
- curve family
- hue policy
- alpha policy
- gamut policy

The public API can keep convenience helpers, but the documentation and grouping should make those axes explicit.

#### Priority

Medium. Strong candidate for cleanup after foundation convergence and `blend` truth alignment.

### Palette

#### Current shape

`palette` currently groups together:

- scales and families
- harmony schemes
- rainbow helpers
- Material-style palette generation
- semantic theme generation

#### Main shortcomings

- Structural palette generation and heuristic design-system generation are mixed together.
- Some APIs are formal and explainable; others are style-driven heuristics.
- The public story does not distinguish those categories strongly enough.
- "Material Design 3" and theme wording currently imply more fidelity than the visible HSL-shift heuristics justify.

#### Doc and contract gaps

- The reference docs present Material and theme helpers in the same high-confidence voice as structural palette tools.
- The implementation in `material.hpp` and `theme.hpp` is heuristic and lightweight, not a standards-faithful design
  token system.
- The reference page also includes future TODOs inline, which blurs current capability and desired expansion.

#### Test gaps

- Tests confirm output sizes and some concrete values.
- They do not establish a stronger distinction between formal scheme correctness and heuristic design outputs.
- The current test style is consistent with the current lightweight implementation, but not with the stronger wording
  in the docs.

#### Recommended direction

Split the public product story into two lines:

- structural palette APIs: schemes, families, scales, rainbow
- design-flavored APIs: material, theme, other heuristics

The second line should remain available, but it should be explicitly documented as heuristic and evolvable.

#### Priority

Medium-high. This is one of the clearer places where user expectations can drift away from the real implementation.

## Target Architecture

The long-term target is a four-layer `operations` model.

### Layer 1: Foundation

Modules:

- `conversion`
- `compare`

Responsibilities:

- reliable cross-type color migration
- explicit metric-based sameness and difference semantics

Target state:

- `conversion` is the single trusted routing model
- `compare` is the single trusted comparison facade

### Layer 2: Transform

Modules:

- `blend`
- `interpolate`

Responsibilities:

- controlled combination and transition between colors

Target state:

- every transform API states or clearly inherits its working space
- alpha handling is explicit
- gamut consequences are documented

### Layer 3: Generate / Curate

Modules:

- `random`
- `palette`

Responsibilities:

- generating candidate colors
- organizing them into useful sets

Target state:

- formal generation tools are clearly separated from style heuristics
- constrained and perceptual generators have explicit contracts

### Layer 4: Presentation Layer

Artifacts:

- README
- reference docs
- examples
- debug/introspection helpers
- tests

Responsibilities:

- state the truth
- demonstrate the truth
- prove the truth

Target state:

- no public wording exceeds implementation reality
- examples mirror the documented semantics
- tests prove the advertised contract instead of only smoke-testing behavior

## Module-Specific Target States

### Compare target state

Move from "policy-based same-type equality helpers" to "typed comparison facade over explicit metrics and tolerances."

Desired characteristics:

- cross-type comparison is first-class
- metric identity is explicit
- boolean sameness and numeric distance are separated cleanly
- threshold meaning is documented per metric

### Conversion target state

Move from "graph-only implementation plus hub-era public vocabulary" to "single-source-of-truth graph routing model."

Desired characteristics:

- `color_cast`, docs, debug helpers, and tests describe the same model
- registered edges and costs are the primary routing vocabulary
- hub concepts are either internalized or explicitly downgraded to compatibility lore

### Blend target state

Move from "available blend mode collection" to "working-space-explicit compositing API."

Desired characteristics:

- default compositing semantics are truthful
- standard-aligned and approximate behaviors are not conflated
- SIMD acceleration is a performance detail, not a contract substitute

### Interpolate target state

Move from "many interpolation helpers" to "semantically organized interpolation system."

Desired characteristics:

- helper families are grouped by space and curve semantics
- hue, alpha, and gamut choices are visible to users
- convenience wrappers still exist, but their defaults are documented plainly

### Random target state

Preserve the current core while organizing it into capability families:

- basic random
- constrained random
- perceptual or gamut-aware random

Desired characteristics:

- no large rewrite required before extension
- stronger contracts for constrained-generator failure and quality semantics

### Palette target state

Move from "mixed formal and heuristic palette toolbox" to "two explicit product lines."

Desired characteristics:

- structural palette APIs are clearly explainable
- heuristic theme and material APIs are clearly labeled as heuristic
- future expansions land in the right category from the start

## Roadmap

### Phase 0: Truth Alignment

Goal:

Align public narrative with actual behavior and stop further semantic drift.

Work themes:

- audit README, reference docs, examples, debug helpers, and tests
- classify statements as correct, outdated, overstated, or unclear
- add concise "current semantics" language per module

Exit criteria:

- no known top-level doc says A while code does B
- each module has a truthful short-form contract
- later phases can assume aligned public vocabulary

### Phase 1: Foundation Convergence

Goal:

Converge `compare` and `conversion` into clean foundation modules.

Work themes:

- define the public comparison model
- decide and enforce the public conversion model
- rework tests to prove the actual contract, not legacy mixed stories

Exit criteria:

- `compare` can answer metric, threshold, and cross-type questions clearly
- `conversion` has one routing story across dispatch, docs, debug, and tests
- new extension work can rely on stable foundation semantics

### Phase 2: Transform And Curate Semantics Cleanup

Goal:

Clean up `blend`, `interpolate`, and `palette` around explicit semantic boundaries.

Work themes:

- working-space truthfulness in `blend`
- semantic regrouping in `interpolate`
- structural vs heuristic separation in `palette`

Exit criteria:

- users can tell what space a transform uses
- heuristic palette APIs are not documented like standards-faithful systems
- transform tests prove semantic properties, not just bounded outputs

### Phase 3: Stable Extension Surface

Goal:

Expand only after the lower layers are truthful and coherent.

Possible extensions:

- richer compare metrics and distance result types
- improved conversion diagnostics and route visualization
- richer perceptual random generation
- accessible and formal palette expansion

Exit criteria:

- new APIs inherit a stable semantic model
- each extension belongs clearly to a layer
- feature growth no longer depends on optimistic documentation

## Prioritization

### Phase priority

1. `Phase 0`
2. `Phase 1`
3. `Phase 2`
4. `Phase 3`

### Module priority

1. `conversion`
2. `compare`
3. `blend`
4. `palette`
5. `interpolate`
6. `random`

This ordering reflects semantic risk, not raw code quality. `random` is last because it is comparatively close to
being safely extensible once the broader contract cleanup is in place.

## Risks And Decision Points

### Decision 1: Public conversion model

The most important architectural decision is whether the public story becomes:

- graph-only routing
- graph routing plus formal hub compatibility layer

This design recommends graph-only public semantics, with any hub concepts either internalized or clearly marked as
non-dispatch explanatory machinery.

### Decision 2: Compare ownership boundary

The design assumes `operations::compare` should own user-facing sameness and closeness APIs, while
`algorithms::delta_e` remains the lower-level metric toolbox.

### Decision 3: Heuristic API labeling

The design assumes heuristic theme and Material-style helpers should stay in the repo, but they must stop presenting
themselves as stronger or more standards-faithful than they are.

## Handoff To Planning

The next planning document should treat this work as a sequence of focused implementation plans rather than one giant
refactor. The recommended first implementation plan is:

- `Phase 0 + Phase 1`
- with `conversion` and `compare` as the highest-priority modules

That first plan should define concrete file edits, contract tests, and documentation updates required to make the
foundation story truthful and coherent.
