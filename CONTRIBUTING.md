# Contributing to colorcpp

Thank you for considering contributing to colorcpp! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
- [Development Setup](#development-setup)
- [Coding Guidelines](#coding-guidelines)
- [Testing](#testing)
- [Documentation](#documentation)
- [Submitting Changes](#submitting-changes)
- [Feature Requests](#feature-requests)
- [Bug Reports](#bug-reports)

## Code of Conduct

We are committed to providing a welcoming and inclusive environment for all contributors. Please be respectful in all interactions and follow professional standards of conduct.

## How to Contribute

There are many ways to contribute to colorcpp:

- **Code contributions**: Implement new features, fix bugs, or improve existing code
- **Documentation**: Improve or expand the documentation
- **Examples**: Create new examples or improve existing ones
- **Testing**: Write or improve tests
- **Bug reports**: Report bugs with detailed information
- **Feature requests**: Suggest new features or improvements

## Development Setup

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15 or later
- Git

### Getting Started

1. **Fork the repository**
   ```bash
   git clone https://github.com/your-username/colorcpp.git
   cd colorcpp
   ```

2. **Set up development environment**
   ```bash
   # Create build directory
   mkdir build && cd build
   
   # Configure with CMake
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   
   # Build tests and examples
   make
   ```

3. **Run tests to verify setup**
   ```bash
   make test
   ```

4. **(Optional) Benchmarks** — enable `-DCOLORCPP_BUILD_BENCHMARKS=ON`, then build target `run_benchmarks` or run `benchmarks/colorcpp_benchmark` from the build tree. Google Benchmark is fetched automatically via `FetchContent`.

### Development Workflow

1. Create a new branch for your feature/fix:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. Make your changes following the [Coding Guidelines](#coding-guidelines)

3. Add tests for your changes

4. Update documentation if needed

5. Run the test suite to ensure nothing is broken

6. Commit your changes with a clear, descriptive message

7. Push to your fork and create a pull request

## Coding Guidelines

### General Principles

- **Compile-time performance**: All operations should be optimized for compile-time execution
- **Type safety**: Use strong typing and compile-time validation
- **Template metaprogramming**: Leverage modern C++ template features
- **Zero overhead**: No runtime overhead for compile-time operations
- **Consistency**: Follow existing code style and patterns

### Code Style

- Use C++17 standard or later
- Follow Google C++ Style Guide with these exceptions:
  - Use `constexpr` whenever possible
  - Prefer template metaprogramming for compile-time operations
  - Use snake_case for function and variable names
  - Use PascalCase for class and struct names
  - Use ALL_CAPS for constants and macros

### Template Design

- Use SFINAE for compile-time type checking
- Provide clear error messages for invalid template parameters
- Document template parameters and constraints
- Use concepts (C++20) when available for better error messages

### Documentation

- Use Doxygen-style comments for all public APIs
- Include parameter descriptions, return values, and examples
- Document template parameters and constraints
- Add usage examples for complex features

### Example Code Style

```cpp
/**
 * @brief Convert RGB color to HSV color space
 * @tparam T Color component type (int, float, etc.)
 * @tparam Scale Scale factor for HSV components
 * @param rgb Input RGB color
 * @return HSV color with specified scale
 * @note Hue range: 0-Scale, Saturation/Value range: 0-Scale
 * @example
 * constexpr auto hsv = to_hsv<1000>(rgba8<255, 128, 64>);
 */
template<typename T, int Scale>
constexpr auto to_hsv(const basic_rgb<T, Scale>& rgb) {
    // Implementation with compile-time validation
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
    static_assert(Scale > 0, "Scale must be positive");
    
    // Function implementation
}
```

## Testing

### Test Structure

Tests are located in the `tests/` directory and organized by feature:

- `tests/core/` - Core color type tests
- `tests/conversion/` - Color space conversion tests
- `tests/operations/` - Color operation tests
- `tests/literals/` - Literal operator tests
- `tests/palettes/` - Color palette tests

### Writing Tests

1. **Use Google Test framework**
2. **Test both compile-time and runtime behavior**
3. **Include edge cases and error conditions**
4. **Use descriptive test names**
5. **Test template instantiations**

### Example Test

```cpp
#include <gtest/gtest.h>
#include <colorcpp/colorcpp.hpp>

TEST(RGBToHSVTest, BasicConversion) {
    constexpr auto rgb = rgba8<255, 128, 64>;
    constexpr auto hsv = to_hsv(rgb);
    
    EXPECT_EQ(hsv.hue(), 25);      // Expected hue value
    EXPECT_EQ(hsv.saturation(), 75); // Expected saturation
    EXPECT_EQ(hsv.value(), 100);   // Expected value
}

TEST(RGBToHSVTest, EdgeCases) {
    constexpr auto black = rgba8<0, 0, 0>;
    constexpr auto white = rgba8<255, 255, 255>;
    
    // Test edge case behavior
    constexpr auto hsv_black = to_hsv(black);
    constexpr auto hsv_white = to_hsv(white);
    
    // Add appropriate assertions
}
```

### Running Tests

```bash
# Run all tests
make test

# Run specific test suite
./build/tests/core_tests

# Run with verbose output
./build/tests/core_tests --gtest_verbose
```

## Documentation

### API Documentation

All public APIs must have comprehensive Doxygen documentation:

- Function/class descriptions
- Parameter descriptions with types and constraints
- Return value descriptions
- Usage examples
- Template parameter documentation

### README Updates

When adding new features:

- Update the main README.md
- Add usage examples
- Document any breaking changes
- Update the API reference section

### Code Comments

- Use clear, descriptive comments for complex algorithms
- Document design decisions and trade-offs
- Explain non-obvious template metaprogramming
- Keep comments up-to-date with code changes

## Submitting Changes

### Pull Request Guidelines

1. **Create focused PRs**: Each PR should address a single feature or fix
2. **Write clear descriptions**: Explain what the PR does and why
3. **Include tests**: All new features must include tests
4. **Update documentation**: Update relevant documentation
5. **Follow the template**: Use the provided PR template

### Pull Request Template

```markdown
## Summary
Brief description of changes

## Test plan
- [ ] All existing tests pass
- [ ] New tests added for new functionality
- [ ] Documentation updated

## Documentation
- [ ] API documentation added/updated
- [ ] README updated if needed
- [ ] Examples added if applicable

## Breaking Changes
- [ ] No breaking changes
- [ ] Breaking changes documented in migration guide

## Test plan
Describe how to test the changes
```

### Review Process

1. **Automated checks**: All CI checks must pass
2. **Code review**: At least one maintainer must approve
3. **Testing**: All tests must pass
4. **Documentation**: Documentation must be complete

## Feature Requests

### Before Submitting

1. **Search existing issues**: Check if the feature has been requested before
2. **Consider implementation**: Think about how the feature would be implemented
3. **Check feasibility**: Ensure it aligns with the project goals

### Feature Request Template

```markdown
## Feature Description
Clear description of the feature

## Use Case
Explain when and why this feature would be useful

## Proposed API
Describe the proposed API or interface

## Alternatives Considered
List any alternative approaches considered

## Implementation Notes
Any implementation details or considerations
```

## Bug Reports

### Before Reporting

1. **Check latest version**: Ensure you're using the latest version
2. **Search existing issues**: Check if the bug has been reported before
3. **Minimal reproduction**: Create a minimal example that reproduces the bug

### Bug Report Template

```markdown
## Bug Description
Clear description of the bug

## Reproduction Steps
1. Step 1
2. Step 2
3. Expected vs actual behavior

## Environment
- Compiler: [e.g., GCC 11.2.0]
- OS: [e.g., Ubuntu 20.04]
- colorcpp version: [e.g., v1.1.0]

## Minimal Example
```cpp
// Minimal code that reproduces the bug
```

## Additional Context
Any additional information that might be helpful
```

## Getting Help

- **Discussions**: Use GitHub Discussions for questions and discussions
- **Issues**: Report bugs and feature requests through GitHub Issues
- **Email**: Contact the maintainers directly for urgent matters

## Recognition

Contributors will be recognized in:

- The README.md contributors section
- Release notes for significant contributions
- Special recognition for major features or improvements

Thank you for contributing to colorcpp! 🎨✨
