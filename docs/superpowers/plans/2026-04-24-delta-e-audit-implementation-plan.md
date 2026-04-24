# Delta E Audit Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Produce a reference-backed audit report for the `delta_e` module, with prioritized remediation guidance, while keeping all generated documents on the `ai/superpowers` branch.

**Architecture:** Perform the audit against the current `main` worktree source snapshot at `/home/merlot/codes/colorspace`, but write all artifacts in the `ai/superpowers` worktree at `/home/merlot/codes/colorspace/.worktrees/ai-superpowers`. Build evidence in layers: focused local test verification, formula/source inspection, published reference comparisons, then convert the findings into a structured audit report with explicit `P0/P1/P2` prioritization.

**Tech Stack:** Markdown, C++20 headers, CMake/CTest, temporary standalone C++ probe programs, published Delta E reference material.

---

## File Map

- Create: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`
- Create: `docs/superpowers/plans/2026-04-24-delta-e-audit-implementation-plan.md`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de76.hpp:1-35`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de94.hpp:1-59`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de2000.hpp:1-127`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/cmc.hpp:1-104`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/din99.hpp:1-109`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/oklab.hpp:1-50`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/simd.hpp:1-66`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/helpers.hpp:1-62`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp:1-258`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp:1-67`
- Read from main worktree: `/home/merlot/codes/colorspace/docs/reference/delta_e.rst:1-72`

### Task 1: Create The Audit Report Scaffold And Baseline Verification

**Files:**
- Create: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp`

- [ ] **Step 1: Write the audit report scaffold**

Create `docs/superpowers/audits/2026-04-24-delta-e-audit.md` with this exact initial content:

```md
# Delta E Module Audit

## Scope

This report audits the current `delta_e` module implementation from the `main` worktree and stores the resulting analysis on the `ai/superpowers` branch.

Audited source areas:

- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de76.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de94.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de2000.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/cmc.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/din99.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/oklab.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/simd.hpp`
- `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/helpers.hpp`
- `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp`
- `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp`
- `/home/merlot/codes/colorspace/docs/reference/delta_e.rst`

## Baseline Verification

## Executive Summary

## Metric-By-Metric Findings

### `delta_e_76`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_94`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_2000`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_cmc`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_din99`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_ok`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_ok` SIMD path

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

## Cross-Cutting Findings

## Priority Table

| ID | Priority | Finding | Evidence | Impact | Suggested remediation |
|----|----------|---------|----------|--------|-----------------------|

## Recommended Fix Order

## Suggested Follow-Up Work
```

- [ ] **Step 2: Build and run the focused delta_e tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/build --target test_delta_e test_delta_e_simd
ctest --test-dir /home/merlot/codes/colorspace/build -R '^(test_delta_e|test_delta_e_simd)\.' --output-on-failure
```

Expected: PASS for the focused `delta_e` and `delta_e_simd` suites. If either command fails, stop the audit and report the failure instead of drafting findings.

- [ ] **Step 3: Replace the Baseline Verification section with executed evidence**

Replace the `## Baseline Verification` section with this exact text after the focused test run passes:

```md
## Baseline Verification

Focused local verification passed before the deeper audit began.

Commands run:

- `cmake --build /home/merlot/codes/colorspace/build --target test_delta_e test_delta_e_simd`
- `ctest --test-dir /home/merlot/codes/colorspace/build -R '^(test_delta_e|test_delta_e_simd)\.' --output-on-failure`

Observed result:

- The focused `delta_e` and `delta_e_simd` tests passed in the current local build before formula review and reference comparison.
```

- [ ] **Step 4: Commit the scaffold and baseline**

```bash
git add docs/superpowers/audits/2026-04-24-delta-e-audit.md
git commit -m "docs: add delta_e audit report scaffold"
```

### Task 2: Audit `delta_e_76`, `delta_e_94`, And `delta_e_2000`

**Files:**
- Modify: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de76.hpp:1-35`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de94.hpp:1-59`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de2000.hpp:1-127`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp:11-145`
- Read from main worktree: `/home/merlot/codes/colorspace/docs/reference/delta_e.rst:12-72`

- [ ] **Step 1: Read the three metric implementations and current tests**

Run:

```bash
sed -n '1,120p' /home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de76.hpp
sed -n '1,140p' /home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de94.hpp
sed -n '1,220p' /home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/de2000.hpp
sed -n '1,160p' /home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp
sed -n '1,120p' /home/merlot/codes/colorspace/docs/reference/delta_e.rst
```

Expected: the output shows that `delta_e_76` is straight Euclidean LAB, `delta_e_94` documents asymmetry, and `delta_e_2000` uses double precision internally.

- [ ] **Step 2: Compile and run a standalone CIEDE2000 probe against published Sharma sample values**

Run:

```bash
cat <<'EOF' >/tmp/delta_e_2000_probe.cpp
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include <colorcpp/colorcpp.hpp>

int main() {
  using colorcpp::algorithms::delta_e::delta_e_2000;
  using colorcpp::core::cielab_t;

  struct Sample {
    cielab_t a;
    cielab_t b;
    double expected;
  };

  const std::vector<Sample> samples = {
      {{50.0000f, 2.6772f, -79.7751f}, {50.0000f, 0.0000f, -82.7485f}, 2.0425},
      {{50.0000f, 3.1571f, -77.2803f}, {50.0000f, 0.0000f, -82.7485f}, 2.8615},
      {{50.0000f, 2.8361f, -74.0200f}, {50.0000f, 0.0000f, -82.7485f}, 3.4412},
      {{50.0000f, -1.3802f, -84.2814f}, {50.0000f, 0.0000f, -82.7485f}, 1.0000},
      {{50.0000f, -1.1848f, -84.8006f}, {50.0000f, 0.0000f, -82.7485f}, 1.0000},
      {{50.0000f, -0.9009f, -85.5211f}, {50.0000f, 0.0000f, -82.7485f}, 1.0000},
  };

  std::cout << std::fixed << std::setprecision(4);
  for (std::size_t i = 0; i < samples.size(); ++i) {
    const double actual = delta_e_2000(samples[i].a, samples[i].b);
    const double diff = std::abs(actual - samples[i].expected);
    std::cout << i << " actual=" << actual << " expected=" << samples[i].expected << " abs_diff=" << diff << "\n";
  }
}
EOF
c++ -std=c++20 -I/home/merlot/codes/colorspace/include /tmp/delta_e_2000_probe.cpp -o /tmp/delta_e_2000_probe
/tmp/delta_e_2000_probe
```

Expected: every printed `abs_diff` is very small, comfortably below `0.0010`.

- [ ] **Step 3: Compare implementation structure against the public CIEDE2000 reference**

Use these public references during the comparison:

- `https://hajim.rochester.edu/ece/sites/gsharma/ciede2000/`
- `https://hajim.rochester.edu/ece/sites/gsharma/ciede2000/dataNprograms/ciede2000testdata.txt`

Expected: the comparison confirms whether the implementation follows the published steps for `G`, adjusted `a'`, hue averaging, `T`, `R_C`, and `R_T`.

- [ ] **Step 4: Write the finished `delta_e_76`, `delta_e_94`, and `delta_e_2000` sections**

Replace those three metric sections in the audit report with complete prose using this exact subsection order and these exact subsection labels:

```md
### `delta_e_76`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_94`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_2000`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment
```

Requirements for the prose:

- State clearly whether a mismatch was found or not
- Distinguish structural formula review from sample-data confirmation
- Call out that `delta_e_94` is asymmetric and whether the docs/tests explain that well enough
- Call out whether `delta_e_2000` is strongly validated or only partially validated

- [ ] **Step 5: Commit the first metric batch**

```bash
git add docs/superpowers/audits/2026-04-24-delta-e-audit.md
git commit -m "docs: audit de76 de94 and de2000"
```

### Task 3: Audit `delta_e_cmc`, `delta_e_din99`, `delta_e_ok`, And The SIMD Path

**Files:**
- Modify: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/cmc.hpp:1-104`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/din99.hpp:1-109`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/oklab.hpp:1-50`
- Read from main worktree: `/home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/simd.hpp:1-66`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp:148-258`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp:1-67`
- Read from main worktree: `/home/merlot/codes/colorspace/docs/reference/delta_e.rst:19-72`

- [ ] **Step 1: Read the remaining implementations, tests, and docs**

Run:

```bash
sed -n '1,180p' /home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/cmc.hpp
sed -n '1,180p' /home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/din99.hpp
sed -n '1,120p' /home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/oklab.hpp
sed -n '1,160p' /home/merlot/codes/colorspace/include/colorcpp/algorithms/delta_e/simd.hpp
sed -n '148,320p' /home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp
sed -n '1,120p' /home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp
sed -n '19,80p' /home/merlot/codes/colorspace/docs/reference/delta_e.rst
```

Expected: the output shows default parameter semantics for `CMC`, the current DIN99 transformation, the Oklab JND threshold, and the SIMD opt-in gates.

- [ ] **Step 2: Verify the SIMD behavior claims with the current focused test binary**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/build -R '^test_delta_e_simd\.' --output-on-failure
```

Expected: PASS for the SIMD-focused tests, confirming the current path matches the scalar reference for the cases already encoded in the suite.

- [ ] **Step 3: Compare `delta_e_ok` and `delta_e_ok` SIMD semantics against public Oklab references**

Use these public references during the comparison:

- `https://bottosson.github.io/posts/oklab/`
- `https://www.w3.org/TR/css-color-4/#ok-lab`

Expected: the comparison confirms that `delta_e_ok` is Euclidean distance in Oklab, not a CIE-standard Delta E formula.

- [ ] **Step 4: Write the finished `delta_e_cmc`, `delta_e_din99`, `delta_e_ok`, and SIMD sections**

Replace those four metric sections in the audit report with complete prose using this exact subsection order and these exact subsection labels:

```md
### `delta_e_cmc`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_din99`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_ok`

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment

### `delta_e_ok` SIMD path

#### Implementation observations

#### Reference comparison

#### Test adequacy

#### Risk assessment
```

Requirements for the prose:

- State whether the implementation looks formula-faithful, merely plausible, or questionable
- State whether public numeric reference data was available or not
- State whether current tests prove scalar correctness, SIMD consistency, or only narrower properties
- State whether the docs overstate the confidence level for `delta_e_ok`

- [ ] **Step 5: Commit the second metric batch**

```bash
git add docs/superpowers/audits/2026-04-24-delta-e-audit.md
git commit -m "docs: audit cmc din99 and oklab delta_e"
```

### Task 4: Audit Cross-Cutting Risks And Build The Priority Table

**Files:**
- Modify: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp`
- Read from main worktree: `/home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp`
- Read from main worktree: `/home/merlot/codes/colorspace/docs/reference/delta_e.rst`

- [ ] **Step 1: Re-read the report and the source test/doc files as a whole**

Run:

```bash
sed -n '1,260p' docs/superpowers/audits/2026-04-24-delta-e-audit.md
sed -n '1,320p' /home/merlot/codes/colorspace/tests/algorithms/test_delta_e.cpp
sed -n '1,120p' /home/merlot/codes/colorspace/tests/algorithms/test_delta_e_simd.cpp
sed -n '1,120p' /home/merlot/codes/colorspace/docs/reference/delta_e.rst
```

Expected: the output makes it easy to spot module-wide gaps such as missing golden sample coverage, documentation overclaims, or uneven confidence levels between metrics.

- [ ] **Step 2: Write the Executive Summary and Cross-Cutting Findings**

Replace the empty `## Executive Summary` and `## Cross-Cutting Findings` sections with complete prose that:

- Names the highest-risk metric or metrics
- Separates confirmed mismatches from weaker confidence issues
- Calls out missing golden reference tests explicitly
- Explains whether the public docs currently match the implementation confidence level

- [ ] **Step 3: Fill the Priority Table with finished findings**

Replace the empty table body under `## Priority Table` with concrete rows using this exact column order:

```md
| ID | Priority | Finding | Evidence | Impact | Suggested remediation |
|----|----------|---------|----------|--------|-----------------------|
| F1 | P1 | Example finding title | File paths, test names, or published sample references | Short impact statement | Short remediation statement |
```

Requirements for the final rows:

- Use `P0` only for strong evidence of a real correctness defect
- Use `P1` for correctness risk, missing validation, or likely misuse hazards
- Use `P2` for lower-severity documentation, test-depth, or maintenance concerns
- Every row must cite evidence from either local source files, local test output, or published references

- [ ] **Step 4: Write the Recommended Fix Order and Suggested Follow-Up Work**

Replace the empty `## Recommended Fix Order` and `## Suggested Follow-Up Work` sections with complete prose that:

- Orders the fix work by value, not by file order
- Starts with the smallest set of actions that can most quickly confirm or disprove correctness
- Separates "add validation first" from "change implementation now"

- [ ] **Step 5: Commit the synthesized findings**

```bash
git add docs/superpowers/audits/2026-04-24-delta-e-audit.md
git commit -m "docs: prioritize delta_e audit findings"
```

### Task 5: Final Report Polish, Consistency Check, And Handoff

**Files:**
- Modify: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`

- [ ] **Step 1: Run a final report read-through**

Run:

```bash
sed -n '1,320p' docs/superpowers/audits/2026-04-24-delta-e-audit.md
```

Expected: the report reads as a finished document, not as notes or a draft outline.

- [ ] **Step 2: Remove weak or overstated language**

Search the report for weak qualifiers and overclaims. Run:

```bash
rg -n "seems|probably|maybe|possibly|appears to be correct|definitely correct|obviously|clearly" docs/superpowers/audits/2026-04-24-delta-e-audit.md
```

Expected: either no matches, or only wording that is deliberate and evidence-backed. Rewrite any sentence that sounds more certain or less certain than the evidence supports.

- [ ] **Step 3: Verify the final audit report file is the only intended deliverable change**

Run:

```bash
git status --short
```

Expected: only the audit report and this plan document are tracked changes on `ai/superpowers`. If extra changes appear, stop and cleanly explain them before proceeding.

- [ ] **Step 4: Commit the finished audit report**

```bash
git add docs/superpowers/audits/2026-04-24-delta-e-audit.md
git commit -m "docs: add delta_e audit report"
```

- [ ] **Step 5: Prepare the user-facing handoff summary**

Use this exact structure in the handoff message:

```md
## Audit Complete

- Report path: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`
- Highest priority findings: `P0/P1/P2` summary
- Recommended next action: one short sentence
```

## Self-Review

### Spec coverage

- The plan audits all metric implementations named in the spec: `de76`, `de94`, `de2000`, `cmc`, `din99`, `delta_e_ok`, and the SIMD path.
- The plan includes both repository-internal evidence gathering and public-reference comparison.
- The plan includes engineering review of tests, docs, API semantics, and prioritization.
- The plan produces the promised deliverable: a written audit report plus fix-priority guidance on `ai/superpowers`.

### Placeholder scan

- Checked for `TBD`, `TODO`, “implement later”, and similar placeholders in task steps.
- Every code-changing step includes exact markdown content or an exact structure to write.
- Every verification step includes an exact command and an expected result.

### Type consistency

- The plan consistently uses the names `delta_e_76`, `delta_e_94`, `delta_e_2000`, `delta_e_cmc`, `delta_e_din99`, `delta_e_ok`, and `delta_e_ok` SIMD path.
- The output audit file path is consistent across all tasks: `docs/superpowers/audits/2026-04-24-delta-e-audit.md`.
- The source of truth split is consistent throughout: inspect source from `/home/merlot/codes/colorspace`, write AI artifacts in `/home/merlot/codes/colorspace/.worktrees/ai-superpowers`.
