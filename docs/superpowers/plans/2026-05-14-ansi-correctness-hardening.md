# ANSI Correctness Hardening Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Harden ANSI IO helpers so edge behavior is explicit, stream formatting is preserved, and docs/tests match the real terminal preview contract.

**Architecture:** Keep `include/colorcpp/io/ansi.hpp` header-only and lightweight. Add a tiny internal stream-state guard in the `details` namespace, define explicit gradient semantics for `steps <= 1`, and add one safe ergonomics overload using `std::span` while preserving pointer+count API compatibility.

**Tech Stack:** C++20, CMake, GoogleTest

---

### Task 1: Lock In Boundary And Stream-Hygiene Contracts

**Files:**
- Modify: `tests/io/test_ansi.cpp`
- Test: `tests/io/test_ansi.cpp`

- [ ] **Step 1: Write failing tests for gradient boundaries**

```cpp
TEST(Ansi, PrintGradientZeroStepsProducesOnlyNewline) {
  std::ostringstream os;
  core::rgba8_t black{0, 0, 0, 255};
  core::rgba8_t white{255, 255, 255, 255};
  print_gradient(os, black, white, 0);
  EXPECT_EQ(os.str(), "\n");
}

TEST(Ansi, PrintGradientOneStepUsesStartColor) {
  std::ostringstream os;
  core::rgba8_t black{0, 0, 0, 255};
  core::rgba8_t white{255, 255, 255, 255};
  print_gradient(os, black, white, 1);
  const std::string output = os.str();
  EXPECT_NE(output.find("\033[48;2;0;0;0m"), std::string::npos);
  EXPECT_EQ(output.find("\033[48;2;255;255;255m"), std::string::npos);
}
```

- [ ] **Step 2: Write failing tests for stream-state restoration**

```cpp
TEST(Ansi, PrintColorRestoresFormattingState) {
  std::ostringstream os;
  os << std::scientific << std::setprecision(3) << std::setfill('_');

  core::rgba8_t red{255, 0, 0, 255};
  print_color(os, red, "red");
  os << "|" << 1.25;

  EXPECT_NE(os.str().find("1.250e+00"), std::string::npos);
}

TEST(Ansi, PrintContrastRestoresFormattingState) {
  std::ostringstream os;
  os << std::scientific << std::setprecision(3) << std::setfill('_');

  core::rgba8_t white{255, 255, 255, 255};
  core::rgba8_t black{0, 0, 0, 255};
  print_contrast(os, white, black);
  os << "|" << 1.25;

  EXPECT_NE(os.str().find("1.250e+00"), std::string::npos);
}
```

- [ ] **Step 3: Run tests to verify RED**

Run: `ctest --test-dir build -R test_ansi --output-on-failure`
Expected: FAIL in newly added boundary/stream-state tests.

- [ ] **Step 4: Commit test-only RED state (optional local checkpoint)**

```bash
git add tests/io/test_ansi.cpp
git commit -m "test: add ansi boundary and stream-state regression tests"
```

### Task 2: Implement Minimal ANSI Header Fixes

**Files:**
- Modify: `include/colorcpp/io/ansi.hpp`
- Test: `tests/io/test_ansi.cpp`

- [ ] **Step 1: Add stream-state guard in `details` namespace**

```cpp
struct ostream_state_guard {
  explicit ostream_state_guard(std::ostream& os)
      : os_(os), flags_(os.flags()), precision_(os.precision()), fill_(os.fill()) {}
  ~ostream_state_guard() {
    os_.flags(flags_);
    os_.precision(precision_);
    os_.fill(fill_);
  }

 private:
  std::ostream& os_;
  std::ios::fmtflags flags_;
  std::streamsize precision_;
  char fill_;
};
```

- [ ] **Step 2: Use guard in formatter-mutating helpers**

```cpp
// at top of each function body that mutates formatting
const details::ostream_state_guard guard(os);
```

Functions: `print_color`, `print_color_verbose`, `print_contrast`.

- [ ] **Step 3: Define explicit gradient behavior for small step counts**

```cpp
if (steps <= 0) {
  os << "\n";
  return;
}
if (steps == 1) {
  os << bg(ra.r(), ra.g(), ra.b()) << "  " << reset() << "\n";
  return;
}
```

- [ ] **Step 4: Run tests to verify GREEN**

Run: `ctest --test-dir build -R test_ansi --output-on-failure`
Expected: PASS for `test_ansi`.

- [ ] **Step 5: Commit implementation**

```bash
git add include/colorcpp/io/ansi.hpp tests/io/test_ansi.cpp
git commit -m "fix: harden ansi gradient edge handling and stream hygiene"
```

### Task 3: Add Safe Container Ergonomics

**Files:**
- Modify: `include/colorcpp/io/ansi.hpp`
- Modify: `tests/io/test_ansi.cpp`
- Modify: `examples/ansi_example.cpp`
- Modify: `README.md`
- Modify: `docs/reference/ansi.rst`

- [ ] **Step 1: Add `std::span` overload for palette helper**

```cpp
template <typename Color>
void print_palette(std::ostream& os, std::span<const Color> colors, int swatch_width = 6) {
  print_palette(os, colors.data(), colors.size(), swatch_width);
}
```

- [ ] **Step 2: Add tests for span overload parity**

```cpp
TEST(Ansi, PrintPaletteSpanOverload) {
  std::array<core::rgba8_t, 3> colors = {
      core::rgba8_t{255, 0, 0, 255},
      core::rgba8_t{0, 255, 0, 255},
      core::rgba8_t{0, 0, 255, 255},
  };
  std::ostringstream os;
  print_palette(os, std::span<const core::rgba8_t>(colors));
  EXPECT_NE(os.str().find("\033[48;2;255;0;0m"), std::string::npos);
}
```

- [ ] **Step 3: Update examples/docs to use and explain the real model**

Required updates:
- explain terminal preview is from `rgba8_t` conversion
- explain alpha is numeric only (no transparency compositing)
- explain verbose HSL is derived from converted RGB
- include/use the new span overload in at least one example snippet

- [ ] **Step 4: Run targeted tests and docs build sanity check**

Run: `ctest --test-dir build -R test_ansi --output-on-failure`
Expected: PASS.

- [ ] **Step 5: Commit ergonomics + docs**

```bash
git add include/colorcpp/io/ansi.hpp tests/io/test_ansi.cpp examples/ansi_example.cpp README.md docs/reference/ansi.rst
git commit -m "feat: add ansi span palette overload and align docs with preview semantics"
```

### Task 4: Final Verification On Main

**Files:**
- Verify only (no required edits)

- [ ] **Step 1: Run full relevant test suites**

Run: `ctest --test-dir build --output-on-failure`
Expected: PASS with no regressions.

- [ ] **Step 2: Build examples**

Run: `cmake --build build --target ansi_example io_example`
Expected: Build succeeds.

- [ ] **Step 3: Manual smoke run for ANSI examples**

Run: `./build/examples/ansi_example | head -n 40`
Expected: ANSI sequences and readable text sections appear.

- [ ] **Step 4: Report completion status with evidence**

Include command outputs summary for tests/build/example run and list changed files.

## Plan Self-Review

- Spec coverage: includes gradient small-step semantics, stream-state restoration, docs contract alignment, and one container-friendly overload.
- Placeholder scan: no TODO/TBD placeholders remain.
- Consistency: all tasks target the same ANSI module and related docs/tests/examples without expanding scope.
