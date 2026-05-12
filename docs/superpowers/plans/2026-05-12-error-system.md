# Error System Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a modern, std-compliant error handling system replacing scattered std::invalid_argument/std::out_of_range exceptions with structured std::error_code-based exceptions.

**Architecture:** Three-layer design (exception hierarchy → error code enumeration → std::error_category implementation) with full std::system_error integration. Header-only, zero runtime overhead when errors not thrown.

**Tech Stack:** C++17, standard library (`<system_error>`, `<stdexcept>`, `<string>`), existing colorcpp headers, Google Test framework for testing.

**Implementation Branch:** `feature/error-system`  
**Spec Location:** `docs/superpowers/specs/2026-05-12-error-system-design.md`

---

## File Structure

### New Files (Phase 1)
```
include/colorcpp/error/
├── error.hpp              # Public API header (includes all below)
├── exception.hpp          # Exception class definitions
├── errc.hpp              # Error code enumeration + metadata
├── category.hpp          # std::error_category implementation
└── category_impl.hpp     # Implementation details (not included directly by users)
```

### Modified Files (Phases 2-4)
- `include/colorcpp/colorcpp.hpp` — Add error module to main header
- `tests/test_error_system.cpp` — New comprehensive error tests
- `include/colorcpp/algorithms/gradient/stepped.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/linear.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/sequence.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/angular.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/box.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/diamond.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/radial.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/stops.hpp` — Phase 2 migration
- `include/colorcpp/algorithms/gradient/details.hpp` — Phase 2 migration
- `include/colorcpp/io/css/named_colors.hpp` — Phase 3 migration
- `include/colorcpp/io/literals/details.hpp` — Phase 3 migration
- `include/colorcpp/io/literals/rgb.hpp` — Phase 3 migration
- `include/colorcpp/core/color_base.hpp` — Phase 4 migration
- `include/colorcpp/operations/interpolate/color_lerp.hpp` — Phase 4 migration

---

## Phase 1: Create Error Module

### Task 1.1: Create errc.hpp (Error Code Enumeration)

**Files:**
- Create: `include/colorcpp/error/errc.hpp`

- [ ] **Step 1: Write header guards and namespace opening**

Create the file with C++ header guards and the colorcpp namespace:

```cpp
#pragma once

#include <system_error>

namespace colorcpp {

// Error code enumeration will go here

}
```

- [ ] **Step 2: Define error code enum**

Add the enum with three categories (validation 1-99, I/O 100-199, logic 200-299):

```cpp
enum class errc : int {
  // Validation errors (codes 1–99)
  invalid_argument = 1,
  out_of_range = 2,
  invalid_size = 3,
  invalid_position = 4,
  constraint_violation = 5,
  
  // I/O errors (codes 100–199)
  invalid_format = 101,
  parse_error = 102,
  encoding_error = 103,
  unsupported_format = 104,
  
  // Logic errors (codes 200–299)
  missing_data = 201,
  invalid_state = 202,
  operation_not_supported = 203,
};
```

- [ ] **Step 3: Add make_error_code helper**

Add the helper function that converts errc to std::error_code:

```cpp
// Forward declaration of error_category (defined in category.hpp)
const std::error_category& error_category() noexcept;

// Helper to create error_code from errc
inline std::error_code make_error_code(errc e) noexcept {
  return std::error_code(static_cast<int>(e), error_category());
}
```

- [ ] **Step 4: Add std::is_error_code_enum specialization**

Add the specialization so std::error_code recognizes colorcpp::errc:

```cpp
}

// Specialization in std namespace for ADL
namespace std {
  template <>
  struct is_error_code_enum<colorcpp::errc> : std::true_type {};
}
```

- [ ] **Step 5: Verify file compiles**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/error/errc.hpp`

Expected: No errors, file is syntactically correct

- [ ] **Step 6: Commit**

```bash
git add include/colorcpp/error/errc.hpp
git commit -m "error: add errc enum with validation, I/O, and logic error codes"
```

---

### Task 1.2: Create exception.hpp (Exception Hierarchy)

**Files:**
- Create: `include/colorcpp/error/exception.hpp`

- [ ] **Step 1: Write header guards and includes**

```cpp
#pragma once

#include <system_error>
#include <string>

namespace colorcpp {

// Forward declaration
enum class errc : int;

// Base exception class goes here

}
```

- [ ] **Step 2: Define base exception class**

Create the base class inheriting from std::system_error:

```cpp
class exception : public std::system_error {
protected:
  errc code_;
  
public:
  explicit exception(const std::string& message, errc code) noexcept;
  
  errc code() const noexcept { return code_; }
  
  virtual ~exception() = default;
};
```

- [ ] **Step 3: Define validation_error class**

```cpp
class validation_error : public exception {
public:
  explicit validation_error(const std::string& message, errc code) noexcept;
};
```

- [ ] **Step 4: Define io_error class**

```cpp
class io_error : public exception {
public:
  explicit io_error(const std::string& message, errc code) noexcept;
};
```

- [ ] **Step 5: Define logic_error class**

```cpp
class logic_error : public exception {
public:
  explicit logic_error(const std::string& message, errc code) noexcept;
};
```

Note: We'll add the implementations (constructors) in category_impl.hpp

- [ ] **Step 6: Verify file structure**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/error/exception.hpp`

Expected: Compilation succeeds (with undefined reference warnings, which is expected for now)

- [ ] **Step 7: Commit**

```bash
git add include/colorcpp/error/exception.hpp
git commit -m "error: add exception hierarchy (base, validation, io, logic)"
```

---

### Task 1.3: Create category_impl.hpp (Implementation Details)

**Files:**
- Create: `include/colorcpp/error/category_impl.hpp`

- [ ] **Step 1: Write implementation header guards**

```cpp
#pragma once

#include "errc.hpp"
#include "exception.hpp"
#include <system_error>
#include <string>

namespace colorcpp {
```

- [ ] **Step 2: Implement exception constructors**

Add constructor implementations that call std::system_error with make_error_code:

```cpp
// Base exception constructor
inline exception::exception(const std::string& message, errc code) noexcept
    : std::system_error(make_error_code(code), message), code_(code) {}

// Derived class constructors (same pattern)
inline validation_error::validation_error(const std::string& message, errc code) noexcept
    : exception(message, code) {}

inline io_error::io_error(const std::string& message, errc code) noexcept
    : exception(message, code) {}

inline logic_error::logic_error(const std::string& message, errc code) noexcept
    : exception(message, code) {}
```

- [ ] **Step 3: Create error_category class**

Implement a custom error_category subclass:

```cpp
class colorcpp_error_category : public std::error_category {
public:
  const char* name() const noexcept override {
    return "colorcpp";
  }
  
  std::string message(int ev) const override {
    auto code = static_cast<errc>(ev);
    switch (code) {
      // Validation errors (1-99)
      case errc::invalid_argument:
        return "invalid argument";
      case errc::out_of_range:
        return "value out of range";
      case errc::invalid_size:
        return "invalid size";
      case errc::invalid_position:
        return "invalid position";
      case errc::constraint_violation:
        return "constraint violation";
      
      // I/O errors (100-199)
      case errc::invalid_format:
        return "invalid format";
      case errc::parse_error:
        return "parse error";
      case errc::encoding_error:
        return "encoding error";
      case errc::unsupported_format:
        return "unsupported format";
      
      // Logic errors (200-299)
      case errc::missing_data:
        return "missing data";
      case errc::invalid_state:
        return "invalid state";
      case errc::operation_not_supported:
        return "operation not supported";
      
      default:
        return "unknown colorcpp error";
    }
  }
};
```

- [ ] **Step 4: Implement error_category() function**

Add the function to get the singleton category instance:

```cpp
inline const std::error_category& error_category() noexcept {
  static const colorcpp_error_category instance;
  return instance;
}
```

- [ ] **Step 5: Update make_error_code function**

Since category.hpp will include this file, update the forward declaration:

```cpp
}
```

Close the namespace.

- [ ] **Step 6: Verify implementation compiles**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -c -o /tmp/cat_impl.o include/colorcpp/error/category_impl.hpp`

Expected: Compiles successfully (header can be compiled to object)

- [ ] **Step 7: Commit**

```bash
git add include/colorcpp/error/category_impl.hpp
git commit -m "error: implement exception constructors and error_category"
```

---

### Task 1.4: Create category.hpp (Public Category Interface)

**Files:**
- Create: `include/colorcpp/error/category.hpp`

- [ ] **Step 1: Write header and includes**

```cpp
#pragma once

#include "errc.hpp"
#include "exception.hpp"
#include "category_impl.hpp"

namespace colorcpp {

// error_category() function is already defined in category_impl.hpp

}
```

- [ ] **Step 2: Verify it brings everything together**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/error/category.hpp`

Expected: No errors

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/error/category.hpp
git commit -m "error: add public category header"
```

---

### Task 1.5: Create error.hpp (Public API Header)

**Files:**
- Create: `include/colorcpp/error/error.hpp`

- [ ] **Step 1: Write public API header**

```cpp
#pragma once

#include "errc.hpp"
#include "exception.hpp"
#include "category.hpp"

// Public API: users include this one header to get everything
namespace colorcpp {

// All public types are already available:
// - exception
// - validation_error, io_error, logic_error
// - errc enum and error codes
// - error_category() function
// - make_error_code() function

}
```

- [ ] **Step 2: Verify it compiles**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/error/error.hpp`

Expected: No errors

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/error/error.hpp
git commit -m "error: add public error module header"
```

---

### Task 1.6: Update main header to include error module

**Files:**
- Modify: `include/colorcpp/colorcpp.hpp`

- [ ] **Step 1: Find the main header and inspect structure**

Run: `head -50 include/colorcpp/colorcpp.hpp`

Look for where submodules are included (likely algorithms/, core/, io/, operations/)

- [ ] **Step 2: Add error module include**

After the main guards and before the first module include, add:

```cpp
#include "colorcpp/error/error.hpp"
```

- [ ] **Step 3: Verify it compiles**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/colorcpp.hpp`

Expected: Compiles successfully

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/colorcpp.hpp
git commit -m "error: include error module in main colorcpp header"
```

---

### Task 1.7: Create comprehensive error system tests

**Files:**
- Create: `tests/test_error_system.cpp`

- [ ] **Step 1: Write test file header and includes**

```cpp
#include <gtest/gtest.h>
#include <colorcpp/error/error.hpp>
#include <system_error>
#include <sstream>

// All error system tests
```

- [ ] **Step 2: Test error code enum values**

```cpp
TEST(ErrorSystem, ErrcEnumValues) {
  EXPECT_EQ(static_cast<int>(colorcpp::errc::invalid_argument), 1);
  EXPECT_EQ(static_cast<int>(colorcpp::errc::out_of_range), 2);
  EXPECT_EQ(static_cast<int>(colorcpp::errc::invalid_format), 101);
  EXPECT_EQ(static_cast<int>(colorcpp::errc::parse_error), 102);
  EXPECT_EQ(static_cast<int>(colorcpp::errc::missing_data), 201);
  EXPECT_EQ(static_cast<int>(colorcpp::errc::invalid_state), 202);
}
```

- [ ] **Step 3: Test exception throwing and catching**

```cpp
TEST(ErrorSystem, ValidationErrorThrowAndCatch) {
  try {
    throw colorcpp::validation_error("test message", colorcpp::errc::invalid_argument);
  }
  catch (const colorcpp::validation_error& e) {
    EXPECT_STREQ(e.what(), "test message");
    EXPECT_EQ(e.code(), colorcpp::errc::invalid_argument);
  }
  catch (...) {
    FAIL() << "Exception not caught as validation_error";
  }
}

TEST(ErrorSystem, IoErrorThrowAndCatch) {
  try {
    throw colorcpp::io_error("parse failed", colorcpp::errc::parse_error);
  }
  catch (const colorcpp::io_error& e) {
    EXPECT_STREQ(e.what(), "parse failed");
    EXPECT_EQ(e.code(), colorcpp::errc::parse_error);
  }
  catch (...) {
    FAIL() << "Exception not caught as io_error";
  }
}

TEST(ErrorSystem, LogicErrorThrowAndCatch) {
  try {
    throw colorcpp::logic_error("invalid state", colorcpp::errc::invalid_state);
  }
  catch (const colorcpp::logic_error& e) {
    EXPECT_STREQ(e.what(), "invalid state");
    EXPECT_EQ(e.code(), colorcpp::errc::invalid_state);
  }
  catch (...) {
    FAIL() << "Exception not caught as logic_error";
  }
}
```

- [ ] **Step 4: Test std::system_error compatibility**

```cpp
TEST(ErrorSystem, SystemErrorCompatibility) {
  try {
    throw colorcpp::validation_error("test", colorcpp::errc::out_of_range);
  }
  catch (const std::system_error& e) {
    EXPECT_EQ(e.code(), colorcpp::errc::out_of_range);
    EXPECT_EQ(e.code().category(), colorcpp::error_category());
  }
  catch (...) {
    FAIL() << "Exception not caught as std::system_error";
  }
}
```

- [ ] **Step 5: Test backward compatibility (std::exception)**

```cpp
TEST(ErrorSystem, BackwardCompatibilityStdException) {
  try {
    throw colorcpp::validation_error("test", colorcpp::errc::invalid_argument);
  }
  catch (const std::exception& e) {
    EXPECT_STREQ(e.what(), "test");
  }
  catch (...) {
    FAIL() << "Exception not caught as std::exception";
  }
}
```

- [ ] **Step 6: Test error_category messages**

```cpp
TEST(ErrorSystem, ErrorCategoryMessages) {
  auto& category = colorcpp::error_category();
  EXPECT_STREQ(category.name(), "colorcpp");
  EXPECT_STREQ(category.message(static_cast<int>(colorcpp::errc::invalid_argument)).c_str(), 
               "invalid argument");
  EXPECT_STREQ(category.message(static_cast<int>(colorcpp::errc::parse_error)).c_str(),
               "parse error");
  EXPECT_STREQ(category.message(static_cast<int>(colorcpp::errc::missing_data)).c_str(),
               "missing data");
}
```

- [ ] **Step 7: Test make_error_code**

```cpp
TEST(ErrorSystem, MakeErrorCode) {
  auto ec = colorcpp::make_error_code(colorcpp::errc::invalid_format);
  EXPECT_EQ(ec.value(), static_cast<int>(colorcpp::errc::invalid_format));
  EXPECT_EQ(ec.category(), colorcpp::error_category());
}
```

- [ ] **Step 8: Test is_error_code_enum specialization**

```cpp
TEST(ErrorSystem, IsErrorCodeEnumSpecialization) {
  // This test verifies that std::error_code can be constructed from errc directly
  std::error_code ec(colorcpp::errc::constraint_violation);
  EXPECT_EQ(ec.value(), static_cast<int>(colorcpp::errc::constraint_violation));
  EXPECT_EQ(ec.category(), colorcpp::error_category());
}
```

- [ ] **Step 9: Verify test file is complete**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -fsyntax-only tests/test_error_system.cpp`

Expected: File is syntactically correct

- [ ] **Step 10: Build and run tests**

Run from the build directory:
```bash
cd /home/merlot/codes/colorspace/build
cmake ..
cmake --build . --target test_error_system
ctest -V
```

Expected: All error system tests pass

- [ ] **Step 11: Commit**

```bash
git add tests/test_error_system.cpp
git commit -m "error: add comprehensive error system unit tests"
```

---

## Phase 2: Migrate Gradient Module (High Priority)

### Task 2.1: Migrate gradient/stepped.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/stepped.hpp`

- [ ] **Step 1: Find all throw sites in the file**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/stepped.hpp`

Expected: Find all throw statements

- [ ] **Step 2: Replace first std::invalid_argument**

Change from:
```cpp
throw std::invalid_argument("colorcpp: stepped gradient levels must be at least 1");
```

To:
```cpp
throw colorcpp::validation_error(
  "stepped gradient levels must be at least 1",
  colorcpp::errc::invalid_argument
);
```

- [ ] **Step 3: Replace remaining std::invalid_argument occurrences**

Repeat for all matching throw sites in the file

- [ ] **Step 4: Verify file compiles**

Run: `cd /home/merlot/codes/colorspace && clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/stepped.hpp`

Expected: No compilation errors

- [ ] **Step 5: Commit**

```bash
git add include/colorcpp/algorithms/gradient/stepped.hpp
git commit -m "error: migrate stepped.hpp to use validation_error"
```

---

### Task 2.2: Migrate gradient/linear.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/linear.hpp`

- [ ] **Step 1: Find throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/linear.hpp`

- [ ] **Step 2: Replace all std::invalid_argument**

Replace each with colorcpp::validation_error, assigning appropriate error codes:
- "scale factor must be positive" → `errc::invalid_argument`
- "quantize levels must be at least 1" → `errc::invalid_argument`

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/linear.hpp`

Expected: No errors

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/algorithms/gradient/linear.hpp
git commit -m "error: migrate linear.hpp to use validation_error"
```

---

### Task 2.3: Migrate gradient/sequence.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/sequence.hpp`

- [ ] **Step 1: Find and replace throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/sequence.hpp`

Replace all std::invalid_argument with colorcpp::validation_error using appropriate error codes:
- "segment positions must be in [0, 1]" → `errc::out_of_range`
- "segment start must be less than end" → `errc::constraint_violation`
- "sequence gradient must have at least one segment" → `errc::invalid_size`
- "sequence segments must not overlap" → `errc::constraint_violation`

- [ ] **Step 2: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/sequence.hpp`

Expected: No errors

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/algorithms/gradient/sequence.hpp
git commit -m "error: migrate sequence.hpp to use validation_error"
```

---

### Task 2.4: Migrate gradient/angular.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/angular.hpp`

- [ ] **Step 1: Find and replace throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/angular.hpp`

Replace with colorcpp::validation_error and appropriate error codes

- [ ] **Step 2: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/angular.hpp`

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/algorithms/gradient/angular.hpp
git commit -m "error: migrate angular.hpp to use validation_error"
```

---

### Task 2.5: Migrate gradient/box.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/box.hpp`

- [ ] **Step 1: Find and replace throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/box.hpp`

- [ ] **Step 2: Replace with validation_error**

Use appropriate error codes for each condition

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/box.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/algorithms/gradient/box.hpp
git commit -m "error: migrate box.hpp to use validation_error"
```

---

### Task 2.6: Migrate gradient/diamond.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/diamond.hpp`

- [ ] **Step 1: Find and replace throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/diamond.hpp`

- [ ] **Step 2: Replace with validation_error**

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/diamond.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/algorithms/gradient/diamond.hpp
git commit -m "error: migrate diamond.hpp to use validation_error"
```

---

### Task 2.7: Migrate gradient/radial.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/radial.hpp`

- [ ] **Step 1: Find and replace throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/radial.hpp`

- [ ] **Step 2: Replace with validation_error**

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/radial.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/algorithms/gradient/radial.hpp
git commit -m "error: migrate radial.hpp to use validation_error"
```

---

### Task 2.8: Migrate gradient/stops.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/stops.hpp`

- [ ] **Step 1: Find and replace throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/stops.hpp`

Replace with appropriate error codes:
- "stop position must be in [0, 1]" → `errc::out_of_range`
- "stop index out of range" → `errc::out_of_range`

- [ ] **Step 2: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/stops.hpp`

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/algorithms/gradient/stops.hpp
git commit -m "error: migrate stops.hpp to use validation_error"
```

---

### Task 2.9: Migrate gradient/details.hpp

**Files:**
- Modify: `include/colorcpp/algorithms/gradient/details.hpp`

- [ ] **Step 1: Find and replace throw sites**

Run: `grep -n "throw std::" include/colorcpp/algorithms/gradient/details.hpp`

Replace with validation_error and appropriate codes:
- "gradient has no stops" → `errc::missing_data`
- "gradient must have at least one stop" → `errc::invalid_size`
- "stop position must be in [0, 1]" → `errc::out_of_range`
- "stop color contains non-finite values" → `errc::constraint_violation`

- [ ] **Step 2: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/algorithms/gradient/details.hpp`

- [ ] **Step 3: Commit**

```bash
git add include/colorcpp/algorithms/gradient/details.hpp
git commit -m "error: migrate details.hpp to use validation_error"
```

---

### Task 2.10: Run gradient tests to verify migrations

**Files:**
- No new files, just testing

- [ ] **Step 1: Build the project**

Run: `cd /home/merlot/codes/colorspace/build && cmake .. && cmake --build .`

Expected: Compiles without errors (note: some tests may fail if they expect old exception types)

- [ ] **Step 2: Check for test failures related to exceptions**

Run: `cd build && ctest -V 2>&1 | grep -A5 "FAIL\|error"`

- [ ] **Step 3: Update gradient tests if needed**

If tests are catching `std::invalid_argument`, update them to catch `colorcpp::validation_error` or keep as `std::exception` for backward compat

- [ ] **Step 4: Run tests again**

Run: `cd build && ctest -V`

Expected: All tests pass (or at least no new failures from exception type changes)

- [ ] **Step 5: Commit**

```bash
git commit -m "error: verify gradient module tests pass with new exceptions"
```

---

## Phase 3: Migrate I/O Module (High Priority)

### Task 3.1: Migrate io/css/named_colors.hpp

**Files:**
- Modify: `include/colorcpp/io/css/named_colors.hpp`

- [ ] **Step 1: Find throw sites**

Run: `grep -n "throw std::" include/colorcpp/io/css/named_colors.hpp`

Expected: Find parse_error throws

- [ ] **Step 2: Replace with io_error**

Change from:
```cpp
throw std::invalid_argument(std::string("colorcpp: unknown named color '") + std::string(str, len) + "'");
```

To:
```cpp
throw colorcpp::io_error(
  std::string("unknown named color '") + std::string(str, len) + "'",
  colorcpp::errc::parse_error
);
```

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/io/css/named_colors.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/io/css/named_colors.hpp
git commit -m "error: migrate named_colors.hpp to use io_error"
```

---

### Task 3.2: Migrate io/literals/details.hpp

**Files:**
- Modify: `include/colorcpp/io/literals/details.hpp`

- [ ] **Step 1: Find throw sites**

Run: `grep -n "throw std::" include/colorcpp/io/literals/details.hpp`

Expected: Find invalid_argument for hex string length

- [ ] **Step 2: Replace with io_error**

Change from:
```cpp
throw std::invalid_argument("colorcpp: invalid hex string length. Expected 3, 4, 6, or 8 characters.");
```

To:
```cpp
throw colorcpp::io_error(
  "invalid hex string length. Expected 3, 4, 6, or 8 characters.",
  colorcpp::errc::invalid_format
);
```

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/io/literals/details.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/io/literals/details.hpp
git commit -m "error: migrate literals/details.hpp to use io_error"
```

---

### Task 3.3: Migrate io/literals/rgb.hpp

**Files:**
- Modify: `include/colorcpp/io/literals/rgb.hpp`

- [ ] **Step 1: Find throw sites**

Run: `grep -n "throw std::" include/colorcpp/io/literals/rgb.hpp`

Expected: Find std::out_of_range for literal value limits

- [ ] **Step 2: Replace with validation_error**

Change from:
```cpp
throw std::out_of_range("colorcpp: _rgb value exceeds 0xFFFFFF (24-bit limit)");
```

To:
```cpp
throw colorcpp::validation_error(
  "_rgb value exceeds 0xFFFFFF (24-bit limit)",
  colorcpp::errc::out_of_range
);
```

Note: Use `validation_error` for out_of_range, `io_error` for format issues

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/io/literals/rgb.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/io/literals/rgb.hpp
git commit -m "error: migrate literals/rgb.hpp to use validation_error"
```

---

### Task 3.4: Run I/O tests to verify migrations

- [ ] **Step 1: Build and run I/O tests**

Run: `cd /home/merlot/codes/colorspace/build && ctest -V -R "io"` (or similar, depending on test naming)

Expected: Tests pass (or update exception catching as needed)

- [ ] **Step 2: Update I/O tests if needed**

If tests expect `std::invalid_argument` or `std::out_of_range`, update them

- [ ] **Step 3: Commit**

```bash
git commit -m "error: verify I/O module tests pass with new exceptions"
```

---

## Phase 4: Migrate Core Module (Medium Priority)

### Task 4.1: Migrate core/color_base.hpp

**Files:**
- Modify: `include/colorcpp/core/color_base.hpp`

- [ ] **Step 1: Find throw sites**

Run: `grep -n "throw std::" include/colorcpp/core/color_base.hpp`

Expected: Find std::out_of_range for channel validation

- [ ] **Step 2: Replace with validation_error**

Change from:
```cpp
throw std::out_of_range("colorcpp: channel value out of range");
```

To:
```cpp
throw colorcpp::validation_error(
  "channel value out of range",
  colorcpp::errc::out_of_range
);
```

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/core/color_base.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/core/color_base.hpp
git commit -m "error: migrate color_base.hpp to use validation_error"
```

---

### Task 4.2: Migrate operations/interpolate/color_lerp.hpp

**Files:**
- Modify: `include/colorcpp/operations/interpolate/color_lerp.hpp`

- [ ] **Step 1: Find throw sites**

Run: `grep -n "throw std::" include/colorcpp/operations/interpolate/color_lerp.hpp`

Expected: Find "requires at least 2 colour stops" errors

- [ ] **Step 2: Replace with validation_error**

Change from:
```cpp
throw std::invalid_argument("colorcpp: multi_lerp requires at least 2 colour stops");
```

To:
```cpp
throw colorcpp::validation_error(
  "multi_lerp requires at least 2 colour stops",
  colorcpp::errc::invalid_size
);
```

- [ ] **Step 3: Verify compilation**

Run: `clang++ -I include -std=c++17 -fsyntax-only include/colorcpp/operations/interpolate/color_lerp.hpp`

- [ ] **Step 4: Commit**

```bash
git add include/colorcpp/operations/interpolate/color_lerp.hpp
git commit -m "error: migrate color_lerp.hpp to use validation_error"
```

---

### Task 4.3: Run core tests to verify migrations

- [ ] **Step 1: Build and test core module**

Run: `cd /home/merlot/codes/colorspace/build && ctest -V`

Expected: All tests pass

- [ ] **Step 2: Look for any remaining std exception throws**

Run: `grep -r "throw std::" include/colorcpp --include="*.hpp"`

Expected: Should find few or none in migration targets

- [ ] **Step 3: Update any remaining tests**

If tests catch specific exception types, update them as needed

- [ ] **Step 4: Commit**

```bash
git commit -m "error: verify core and operations tests pass"
```

---

## Phase 5: Final Verification and Documentation

### Task 5.1: Comprehensive test run

- [ ] **Step 1: Clean build**

Run: `cd /home/merlot/codes/colorspace && rm -rf build && mkdir build && cd build`

- [ ] **Step 2: Configure with tests**

Run: `cmake -DENABLE_TESTS=ON ..`

- [ ] **Step 3: Build all**

Run: `cmake --build . -j4`

Expected: All builds succeed

- [ ] **Step 4: Run all tests**

Run: `ctest -V`

Expected: All tests pass (note: may need to update tests that catch old exception types)

- [ ] **Step 5: Commit if needed**

```bash
git commit -m "error: all tests passing after migrations"
```

---

### Task 5.2: Verify no remaining std exception throws in migrated code

- [ ] **Step 1: Find any remaining throw sites**

Run: `grep -r "throw std::invalid_argument\|throw std::out_of_range\|throw std::logic_error" include/colorcpp/algorithms include/colorcpp/io/css include/colorcpp/io/literals include/colorcpp/core include/colorcpp/operations/interpolate --include="*.hpp"`

Expected: Should find none (or very few if some paths were missed)

- [ ] **Step 2: If any found, update them**

Replace remaining throws with appropriate colorcpp exceptions

- [ ] **Step 3: Re-run tests**

Run: `cd build && ctest -V`

Expected: All tests pass

- [ ] **Step 4: Final commit**

```bash
git commit -m "error: clean up remaining std exception throws"
```

---

### Task 5.3: Update documentation

- [ ] **Step 1: Check for existing error documentation**

Run: `find docs -name "*.md" -o -name "*.rst" | xargs grep -l "exception\|error"`

- [ ] **Step 2: Update error handling documentation**

Add section to docs explaining:
- New exception hierarchy
- Error codes and what they mean
- How to catch colorcpp errors
- std::error_code compatibility

- [ ] **Step 3: Update README if needed**

Add note about error handling to the main README.md

- [ ] **Step 4: Commit**

```bash
git add docs/*
git add README.md
git commit -m "docs: document new error system and exception handling"
```

---

### Task 5.4: Create summary of changes

- [ ] **Step 1: Check commit log**

Run: `git log --oneline -20`

- [ ] **Step 2: Create migration summary**

List all files changed, number of throw sites migrated, breaking changes

- [ ] **Step 3: Document for users**

Create MIGRATION.md or update CHANGELOG explaining:
- What changed
- How users should update their code
- Exception catching patterns

- [ ] **Step 4: Commit**

```bash
git add CHANGELOG.md (or MIGRATION.md)
git commit -m "docs: add error system migration notes"
```

---

## Testing Checklist (Verify Before Declaring Success)

- [ ] Error module compiles standalone
- [ ] All error system unit tests pass
- [ ] All gradient module tests pass with new exceptions
- [ ] All I/O module tests pass with new exceptions
- [ ] All core module tests pass with new exceptions
- [ ] Code catching `std::exception` still works
- [ ] Code can catch `std::system_error`
- [ ] Error codes can be inspected programmatically
- [ ] Error messages match original exceptions (or intentionally changed)
- [ ] No warnings in migration commits
- [ ] Documentation reflects new error system

---

## Success Criteria (Phase Complete)

✅ All exception hierarchy classes implemented  
✅ All error codes defined and working  
✅ std::error_category fully integrated  
✅ All throw sites in gradient module migrated  
✅ All throw sites in I/O module migrated  
✅ All throw sites in core module migrated  
✅ All existing tests updated and passing  
✅ New error system tests comprehensive and passing  
✅ Backward compatibility verified (std::exception catching)  
✅ Documentation updated  
✅ Zero breaking changes for code catching std::exception  

---

## Notes for Implementation

1. **Header-only constraint:** All code must be in headers or inline in header implementations. No .cpp files.

2. **Error messages:** Keep original messages where possible for zero behavioral change. Remove "colorcpp: " prefix if it's now redundant (category name provides context).

3. **Testing:** After each phase, build clean and run full test suite to catch integration issues early.

4. **Git discipline:** Commit frequently (after each task step). Makes it easy to identify issues and revert if needed.

5. **Compilation:** Always verify with `clang++ -std=c++17` before committing to catch issues early.

---

## Implementation Rollback (If Needed)

If at any point the error system isn't working as expected:

```bash
# Reset to before error system changes
git checkout main -- include/colorcpp/error/
git reset --hard HEAD~<N>  # Where N is number of error commits

# Then cherry-pick only the working commits
```

---

**Next Steps After This Plan:**
1. Use subagent-driven-development skill to execute tasks in parallel where possible
2. After all tasks complete, create a pull request from feature/error-system → main
3. Request code review before merging
4. Merge and update all dependent branches
