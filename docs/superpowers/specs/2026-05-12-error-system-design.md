# Error System Design Specification

**Date:** 2026-05-12  
**Branch:** ai/superpowers  
**Status:** Design Phase  
**Implementation Branch:** feature/error-system

---

## Executive Summary

Introduce a **modern, standards-compliant error handling system** for colorcpp that replaces scattered `std::invalid_argument` and `std::out_of_range` exceptions with structured `std::error_code`-based exceptions. The system provides:

- **100% std library compatibility** via `std::system_error` integration
- **Broad error categories** (validation, I/O, logic) matching the current exception patterns
- **Zero breaking changes** for code catching `std::exception`
- **Foundation for future no-throw APIs** using `std::error_code`
- **Header-only implementation** with no runtime overhead when errors aren't thrown

---

## 1. Architecture

### 1.1 Design Philosophy

The error system implements a **three-layer architecture**:

1. **Exception Layer** — Simple exception hierarchy inheriting from `std::system_error`
2. **Error Code Layer** — Flat enum `colorcpp::errc` mapping error conditions to codes
3. **Category Layer** — Custom `std::error_category` subclass for code-to-message mapping

This approach balances **simplicity** (users just throw exceptions like before) with **standardization** (full compatibility with `std::error_code` and standard library error handling patterns).

### 1.2 Module Structure

```
include/colorcpp/error/
├── error.hpp              # Public API (includes all below)
├── exception.hpp          # Exception class definitions
├── errc.hpp              # Error code enumeration
└── category.hpp          # std::error_category implementation
```

**Integration point:** Add `error/` to the main `colorcpp.hpp` header so users can `#include <colorcpp/error/error.hpp>` or get it automatically with the main header.

---

## 2. Exception Hierarchy

### 2.1 Base Exception Class

```cpp
namespace colorcpp {

// Base exception wrapping std::system_error
class exception : public std::system_error {
public:
  // Constructor from errc code
  explicit exception(const std::string& message, errc code) noexcept;
  
  // Get the error code
  errc code() const noexcept;
  
  // Destructor
  virtual ~exception() = default;
};

}
```

**Rationale:**
- Inherits from `std::system_error` so users can catch with `catch (const std::system_error&)`
- Stores the `colorcpp::errc` code internally
- Can be converted to `std::error_code` via `std::system_error::code()`
- Message (from `what()`) is passed to base class

### 2.2 Derived Exception Classes

Three broad categories matching current exception patterns:

```cpp
namespace colorcpp {

// Validation errors: invalid argument values, out of range, type mismatches
class validation_error : public exception {
public:
  explicit validation_error(const std::string& message, errc code) noexcept;
};

// I/O errors: parsing failures, format errors, encoding issues
class io_error : public exception {
public:
  explicit io_error(const std::string& message, errc code) noexcept;
};

// Logic errors: missing data, invalid state, constraint violations
class logic_error : public exception {
public:
  explicit logic_error(const std::string& message, errc code) noexcept;
};

}
```

**Exception Mapping (current → new):**

| Current Exception | New Class | Notes |
|---|---|---|
| `std::invalid_argument` | `colorcpp::validation_error` | Parameter validation failures |
| `std::out_of_range` | `colorcpp::validation_error` | Range check failures |
| Custom validation | `colorcpp::validation_error` | Format checks, constraint violations |
| Parse errors | `colorcpp::io_error` | CSS parsing, hex decoding failures |
| Encoding errors | `colorcpp::io_error` | Serialization, format issues |
| State errors | `colorcpp::logic_error` | Missing required data, invalid operations |

---

## 3. Error Code System

### 3.1 Error Code Enumeration

```cpp
namespace colorcpp {

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

}
```

**Rationale:**
- Flat enum (not categorized by ranges in source, but ranges visible in numbering)
- Codes start at 1 (avoiding the default 0 which might be treated as "success")
- ~20–25 codes total (minimal, covers current exceptions)
- Extensible: new codes can be added without breaking existing code

### 3.2 std::error_category Implementation

Provide a custom `error_category` subclass:

```cpp
namespace colorcpp {

const std::error_category& error_category() noexcept;

// Also enable automatic conversion (ADL)
std::error_code make_error_code(errc e) noexcept;

}

// Specialization in std namespace for ADL (required by std::error_code)
namespace std {
  template <>
  struct is_error_code_enum<colorcpp::errc> : std::true_type {};
}
```

**Behavior:**
- `error_category()::name()` returns `"colorcpp"`
- `error_category()::message(value)` returns human-readable message for each code
- Allows `std::error_code(colorcpp::errc::value)` to work automatically
- Users can catch as `catch (const std::system_error& e)` and check `e.code() == colorcpp::errc::invalid_argument`

---

## 4. Usage Patterns

### 4.1 Throwing Errors (Implementation)

**Old pattern:**
```cpp
if (position < 0 || position > 1) {
  throw std::invalid_argument("colorcpp: stop position must be in [0, 1]");
}
```

**New pattern:**
```cpp
if (position < 0 || position > 1) {
  throw colorcpp::validation_error(
    "stop position must be in [0, 1]",
    colorcpp::errc::out_of_range
  );
}
```

### 4.2 Catching Errors (User Code)

**Pattern 1: Catch specific colorcpp error**
```cpp
try {
  auto color = parse_hex_color("#XYZ");
}
catch (const colorcpp::io_error& e) {
  std::cerr << "Parse failed: " << e.what() << "\n";
  std::cerr << "Error code: " << e.code().value() << "\n";
}
```

**Pattern 2: Catch with std library integration**
```cpp
try {
  auto color = parse_hex_color("#XYZ");
}
catch (const std::system_error& e) {
  if (e.code().category() == colorcpp::error_category()) {
    std::cerr << "colorcpp error: " << e.what() << "\n";
  }
}
```

**Pattern 3: Inspect error code**
```cpp
try {
  auto color = gradient.sample(1.5);
}
catch (const colorcpp::validation_error& e) {
  if (e.code() == colorcpp::errc::out_of_range) {
    std::cerr << "Out of range error\n";
  }
}
```

**Pattern 4: Backward compatible (existing code)**
```cpp
try {
  auto color = gradient.sample(1.5);
}
catch (const std::exception& e) {
  std::cerr << "Error: " << e.what() << "\n";
}
// Still works! colorcpp::validation_error → std::system_error → std::exception
```

---

## 5. Migration Strategy

### 5.1 Phase 1: Create Error Module

- Implement `exception.hpp`, `errc.hpp`, `category.hpp`
- Add `error.hpp` as public API header
- Create comprehensive unit tests for error codes and category
- No changes to existing code yet

**Deliverable:** Error module compiles, tests pass, ready for migration

### 5.2 Phase 2: Replace Exceptions

Update exception-throwing locations **module by module**:

1. **Priority 1 (High Impact):**
   - `algorithms/gradient/` — 20+ throw sites
   - `io/css/` — parsing errors
   - `io/literals/` — hex/literal parsing

2. **Priority 2 (Medium Impact):**
   - `core/color_base.hpp` — range validation
   - `algorithms/` other modules

3. **Priority 3 (Low Impact):**
   - Less-used validation paths
   - Future extensions

**Per-location change:**
- Replace `throw std::invalid_argument("msg")` with `throw colorcpp::validation_error("msg", errc::invalid_argument)`
- Replace `throw std::out_of_range("msg")` with `throw colorcpp::validation_error("msg", errc::out_of_range)`
- Assign appropriate error code matching the condition

**Tests:** Update existing test expectations to catch `colorcpp::validation_error` instead of `std::invalid_argument` (or keep as `std::exception` for backward compat tests)

### 5.3 Phase 3: No-Throw APIs (Future)

After core migration, optionally add:
```cpp
// Future: return-based error handling
std::optional<Color> parse_hex_color(std::string_view hex) noexcept;
std::error_code ec = /* ... */;

// Future: callback-based error reporting
void on_error(const std::error_code& ec) noexcept { /* ... */ };
```

---

## 6. Breaking Changes & Compatibility

### 6.1 What Breaks

- **Code explicitly catching `std::invalid_argument`** or `std::out_of_range` will not catch the new exceptions
  - **Impact:** Low — most users catch `std::exception`
  - **Mitigation:** Documented in CHANGELOG, easy fix: catch `colorcpp::validation_error` instead

### 6.2 What Doesn't Break

- ✅ Code catching `std::exception` — still works
- ✅ Code catching `std::system_error` — still works
- ✅ Exception messages — preserved exactly
- ✅ Header-only compilation — unaffected
- ✅ ABI — no changes (header-only)

---

## 7. Testing Strategy

### 7.1 Unit Test Coverage

**Test categories:**

1. **Error Code Correctness**
   - Each error code maps to correct string message
   - Code value matches enum definition
   - All enum values have messages defined

2. **Exception Throwing**
   - Each exception type can be thrown and caught
   - Exception message preserved in `what()`
   - Code accessible via `.code()`

3. **std Integration**
   - `std::error_code(errc::value)` works
   - Category comparison: `e.code().category() == colorcpp::error_category()`
   - Can catch as `std::system_error`

4. **Migration Coverage**
   - Each throw site validates correct exception type and code are used
   - Old error messages remain unchanged (string comparison)

5. **Backward Compatibility**
   - Catching `std::exception` still works
   - No regression in existing exception handling tests

**Example test:**
```cpp
TEST(ErrorSystem, ValidationErrorOutOfRange) {
  try {
    throw colorcpp::validation_error(
      "value out of range",
      colorcpp::errc::out_of_range
    );
  }
  catch (const std::system_error& e) {
    EXPECT_EQ(e.code(), colorcpp::errc::out_of_range);
    EXPECT_EQ(e.code().category(), colorcpp::error_category());
    EXPECT_STREQ(e.what(), "value out of range");
  }
}
```

### 7.2 Integration Tests

- Parse operations with invalid input → check `io_error` and error code
- Gradient operations with invalid parameters → check `validation_error` codes
- Existing test suites still pass with updated exception types

---

## 8. Implementation Notes

### 8.1 Header-Only Considerations

- Exception classes are simple (minimal inline code)
- `std::error_category` implementation goes inline in `category.hpp`
- No template instantiation bloat
- Message strings are const data (minimal binary impact)

### 8.2 Error Messages

**Current messages** (from grep analysis):
- "colorcpp: multi_lerp requires at least 2 colour stops"
- "colorcpp: stop position must be in [0, 1]"
- "colorcpp: gradient has no stops"
- etc.

**New approach:**
- Keep message prefix "colorcpp: " (users recognize it)
- Trim "colorcpp: " from message stored if desired (category name provides context)
- **Decision:** Preserve as-is for zero behavioral change

### 8.3 Dependency Management

- Standard library only: `<system_error>`, `<stdexcept>`, `<string>`
- No external dependencies
- C++17+ (matches library requirements)

---

## 9. Success Criteria

- [ ] Error module compiles and tests pass
- [ ] All exceptions replaced in 3 core modules (gradient, I/O, core)
- [ ] Backward compatibility verified (code catching `std::exception` still works)
- [ ] std::error_code integration works as documented
- [ ] Error messages unchanged from original exceptions
- [ ] No performance regression (header-only, zero cost if not thrown)
- [ ] Documentation updated with new exception types

---

## 10. Future Extensions

1. **Error recovery helpers** — Functions to suggest fixes based on error code
2. **Contextual errors** — Attach parameter values to error (e.g., "received 1.5, expected [0, 1]")
3. **Error logging integration** — Hooks for logging frameworks
4. **Error chains** — Propagate root cause through multiple layers (C++17 exception chaining)

---

## Appendix: Complete Exception Map

| Current Location | Current Exception | New Class | Error Code | Message |
|---|---|---|---|---|
| gradient/stepped.hpp:33 | `std::invalid_argument` | `validation_error` | `invalid_argument` | "stepped gradient levels must be at least 1" |
| gradient/linear.hpp:174 | `std::invalid_argument` | `validation_error` | `invalid_argument` | "scale factor must be positive" |
| io/literals/rgb.hpp:54 | `std::out_of_range` | `validation_error` | `out_of_range` | "_rgb value exceeds 0xFFFFFF (24-bit limit)" |
| io/css/named_colors.hpp:220 | `std::invalid_argument` | `io_error` | `parse_error` | "unknown named color '...'" |
| core/color_base.hpp:91 | `std::out_of_range` | `validation_error` | `out_of_range` | "channel value out of range" |

(Complete mapping to be generated during implementation)

---

## Sign-Off

**Design Review:** Approved ✓  
**Next Step:** Write implementation plan and execute Phase 1 (error module creation)
