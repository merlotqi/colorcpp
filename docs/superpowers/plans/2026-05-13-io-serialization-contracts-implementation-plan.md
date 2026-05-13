# IO Serialization Contracts Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Tighten the `colorcpp::io::serialization` public contract on `ai/superpowers` so headers, options, docs, examples, and tests all describe the same currently supported adapter surface.

**Architecture:** Use TDD to turn the serialization module from an overstated pseudo-subsystem into an honest adapter toolkit. First add regression coverage for real JSON round trips, generic named-mode defaults, and MessagePack helper behavior. Then rename the generic format headers to adapter-oriented peers, remove the dead `include_color_space` option instead of pretending tagging exists, and update the aggregate header plus docs/examples to describe only compact JSON, generic named JSON, and lower-level MessagePack pack/unpack helpers.

**Tech Stack:** C++17, header-only `colorcpp`, GoogleTest/CTest, CMake, Sphinx docs, example programs under `examples/`.

---

## File Map

- Modify: `include/colorcpp/io/serialization.hpp`
  - Point the aggregate header at adapter-oriented subheaders.
- Rename: `include/colorcpp/io/serialization/json.hpp` -> `include/colorcpp/io/serialization/json_adapter.hpp`
  - Keep JSON adapter behavior but move it under a less collision-prone name.
- Rename: `include/colorcpp/io/serialization/msgpack.hpp` -> `include/colorcpp/io/serialization/msgpack_adapter.hpp`
  - Keep MessagePack helper behavior with a symmetric adapter-oriented name.
- Modify: `include/colorcpp/io/serialization/traits.hpp`
  - Remove the dead `include_color_space` option and tighten comments to the truthful compact/named contract.
- Modify: `tests/io/test_serialization.cpp`
  - Add concrete fake-adapter tests for JSON round trips, named defaults, named custom keys, and MessagePack helpers.
- Modify: `docs/reference/serialization.rst`
  - Remove tagged/binary overclaims and document the current adapter toolkit honestly.
- Modify: `README.md`
  - Align the serialization summary and examples with the actual supported contract.
- Modify: `examples/serialization_example.cpp`
  - Keep the JSON example truthful about generic default names and supported modes.
- Modify: `examples/msgpack_example.cpp`
  - Clarify that MessagePack support is lower-level helper-based rather than options-driven parity with JSON.

## Scope Guard

This branch intentionally does **not**:

- add semantic default channel-name traits such as `r/g/b`
- add type-tagged serialization or `color_space_id`-driven metadata
- add a top-level MessagePack API analogous to `to_json()`
- add any new binary serialization format

If those capabilities are still wanted after this cleanup lands, they should be planned as a second serialization metadata/tagging branch.

### Task 1: Verify The Current Serialization Baseline

**Files:**
- Read: `include/colorcpp/io/serialization.hpp`
- Read: `include/colorcpp/io/serialization/json.hpp`
- Read: `include/colorcpp/io/serialization/msgpack.hpp`
- Read: `include/colorcpp/io/serialization/traits.hpp`
- Read: `tests/io/test_serialization.cpp`
- Read: `docs/reference/serialization.rst`
- Read: `README.md`
- Read: `examples/serialization_example.cpp`
- Read: `examples/msgpack_example.cpp`

- [ ] **Step 1: Configure the branch-local build**

Run:

```bash
cmake -S /home/merlot/codes/colorspace/.worktrees/ai-superpowers -B /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build -DCOLORCPP_BUILD_TESTS=ON -DCOLORCPP_BUILD_EXAMPLES=ON
```

Expected: CMake configures successfully for the existing `ai/superpowers` worktree.

- [ ] **Step 2: Build the focused serialization targets**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build --target test_serialization serialization_example msgpack_example
```

Expected: the baseline serialization targets build successfully before adding new tests.

- [ ] **Step 3: Verify the inherited serialization baseline**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build -R '^test_serialization\.' --output-on-failure
```

Expected: PASS on the inherited baseline. If this fails, stop and report it before adding regression coverage.

### Task 2: Add Failing Contract Tests First

**Files:**
- Modify: `tests/io/test_serialization.cpp`

- [ ] **Step 1: Add concrete fake JSON adapter coverage**

Add a tiny in-test JSON value type plus a `json_adapter` specialization that supports:

- compact array output and round trip for `rgba8_t`
- named output with default generic keys `ch0`, `ch1`, ...
- named output with custom keys
- auto-detect `from_json()` for both array and object inputs

- [ ] **Step 2: Add concrete fake MessagePack helper coverage**

Add a tiny in-test packer/unpacker pair plus trait specializations that prove:

- `pack_color()` emits a compact array flow
- `pack_color_named()` emits a map flow with caller-provided keys
- `unpack_color()` round-trips compact values
- `unpack_color_named()` round-trips named values

- [ ] **Step 3: Update the options test to the intended truth-aligned API**

Replace the dead-option assertion with the new expected contract:

- `serialization_options` only exposes `format`
- default `format` remains `serialization_format::compact`

- [ ] **Step 4: Build and run the focused serialization tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build --target test_serialization
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build -R '^test_serialization\.' --output-on-failure
```

Expected: FAIL before production edits.

Expected failure shape:

- tests that include the future adapter header names fail until the rename is complete
- the options test fails until `include_color_space` is removed
- any new round-trip checks expose current gaps in concrete test coverage

### Task 3: Rename Headers And Tighten The Public Surface

**Files:**
- Modify: `include/colorcpp/io/serialization.hpp`
- Rename: `include/colorcpp/io/serialization/json.hpp` -> `include/colorcpp/io/serialization/json_adapter.hpp`
- Rename: `include/colorcpp/io/serialization/msgpack.hpp` -> `include/colorcpp/io/serialization/msgpack_adapter.hpp`
- Modify: `include/colorcpp/io/serialization/traits.hpp`

- [ ] **Step 1: Rename the JSON and MessagePack subheaders symmetrically**

Change the two format headers to:

- `include/colorcpp/io/serialization/json_adapter.hpp`
- `include/colorcpp/io/serialization/msgpack_adapter.hpp`

Keep file contents intact except for updated file comments that match the new names.

- [ ] **Step 2: Update the aggregate include surface**

Point `include/colorcpp/io/serialization.hpp` at the renamed adapter headers so downstream includes continue to work through the aggregate header.

- [ ] **Step 3: Remove the dead tagging option from `traits.hpp`**

Change `serialization_options` so it contains only:

```cpp
struct serialization_options {
  serialization_format format = serialization_format::compact;
};
```

Also update nearby comments so they no longer mention color-space metadata that does not exist.

- [ ] **Step 4: Rebuild and rerun the focused tests**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build --target test_serialization serialization_example msgpack_example
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build -R '^test_serialization\.' --output-on-failure
```

Expected: PASS once the rename and public-surface cleanup are correct.

### Task 4: Align Docs And Examples With The Real Contract

**Files:**
- Modify: `docs/reference/serialization.rst`
- Modify: `README.md`
- Modify: `examples/serialization_example.cpp`
- Modify: `examples/msgpack_example.cpp`

- [ ] **Step 1: Rewrite `docs/reference/serialization.rst` as an honest adapter-toolkit page**

Document only:

- aggregate header entry point
- `to_json()` / `from_json()` compact and generic named modes
- caller-provided custom names
- MessagePack helper entry points as lower-level pack/unpack adapters
- no tagged mode
- no binary completeness claim in this module

- [ ] **Step 2: Update README serialization examples**

Keep the README example small and truthful:

- mention adapter specialization as the integration point
- show compact JSON and optional custom names
- avoid claims of tagged or binary support in this module

- [ ] **Step 3: Fix the JSON example commentary**

Make the named-format example explicitly show that the default object keys are generic `ch0`, `ch1`, ... unless the caller passes custom names.

- [ ] **Step 4: Fix the MessagePack example commentary**

Make it clear that MessagePack support currently exposes packer/unpacker helper traits and helper functions, not a full options-based top-level serializer surface.

### Task 5: Full Verification Before Closeout

**Files:**
- Test: `tests/io/test_serialization.cpp`
- Test: `examples/serialization_example.cpp`
- Test: `examples/msgpack_example.cpp`

- [ ] **Step 1: Rebuild the touched targets from the clean contract-aligned tree**

Run:

```bash
cmake --build /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build --target test_serialization serialization_example msgpack_example
```

Expected: all touched targets build successfully.

- [ ] **Step 2: Run the focused serialization test suite**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build -R '^test_serialization\.' --output-on-failure
```

Expected: PASS.

- [ ] **Step 3: Run a broader safety slice for non-CSS IO**

Run:

```bash
ctest --test-dir /home/merlot/codes/colorspace/.worktrees/ai-superpowers/build -R '^test_(serialization|literals|binary_io)\.' --output-on-failure
```

Expected: PASS, showing this contract cleanup did not regress the two already-completed predecessor branches.
