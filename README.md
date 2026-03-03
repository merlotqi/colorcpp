# ColorCpp

A modern, header-only C++ library for compile-time color manipulation and conversion between different color spaces.

## 🎨 Features

- **Compile-time color operations**: All color conversions and manipulations happen at compile time
- **Multiple color spaces**: RGB, HSV, HSL, and CMYK support
- **Type-safe color literals**: Modern C++ user-defined literals for colors
- **Template-based design**: Zero-runtime overhead with compile-time validation
- **Comprehensive color palettes**: Built-in web-safe and named colors
- **Advanced blending operations**: Linear interpolation and standard graphics blending modes
- **Extensive documentation**: Comprehensive API documentation with examples

## Color Spaces

### RGB (Red-Green-Blue)
- 8-bit integer RGB (`rgba8_t`) with values 0-255
- Floating-point RGB (`rgbaf_t`) with values 0.0-1.0
- Template-based RGB with compile-time validation
- **New**: Enhanced literal operators with explicit alpha support

### HSV (Hue-Saturation-Value)
- Integer HSV with hue 0-359°, saturation/value 0-100%
- Floating-point HSV with hue 0-360°, saturation/value 0.0-1.0
- Natural color manipulation in cylindrical coordinates
- **New**: Template literal support for compile-time HSV creation

### HSL (Hue-Saturation-Lightness)
- Similar to HSV but with lightness instead of value
- Better for creating lighter/darker color variants
- Template-based with compile-time validation
- **New**: Comprehensive documentation with practical examples

### CMYK (Cyan-Magenta-Yellow-Key)
- Professional printing color space
- Integer and floating-point variants
- Template-based with compile-time validation
- **New**: Decimal literal format support (CCCMMMYYYKKK)

## Usage

### Basic Color Creation

```cpp
#include "color/color.hpp"

using namespace color;

// Create colors using template literals
constexpr auto red = rgba8<255, 0, 0>;
constexpr auto blue = rgba8<0, 0, 255>;

// Create colors using user-defined literals
constexpr auto green = 0x00FF00_rgb;
constexpr auto yellow = 0xFFFF00_hex;

// Create HSV colors with new template literal support
constexpr auto orange = 30100100_hsv;  // H=30, S=100, V=100
constexpr auto purple = 300100100_hsv; // H=300, S=100, V=100

// Create HSL colors with enhanced documentation
constexpr auto cyan = 180100050_hsl;   // H=180, S=100, L=50
constexpr auto magenta = 300100050_hsl; // H=300, S=100, L=50

// New: Hue shortcuts for pure colors
constexpr auto pure_red = 0_hsv_hue;   // HSV(0°, 100%, 100%)
constexpr auto pure_blue = 240_hsl_hue; // HSL(240°, 100%, 50%)
```

### Color Conversion

```cpp
// Convert between color spaces
constexpr auto hsv_red = to_hsv(red);
constexpr auto rgb_blue = to_rgb(blue_hsv);

// Automatic type conversion with enhanced error checking
constexpr auto converted = convert<rgba8_t>(hsv_red);

// New: Template-based conversion with compile-time validation
constexpr auto hsl_green = to_hsl<1000>(green_rgb);
constexpr auto cmyk_yellow = to_cmyk<1000>(yellow_rgb);
```

### Color Operations

```cpp
// Lighten a color by 20%
constexpr auto light_red = lighten(red, 20);

// Saturate a color by 30%
constexpr auto saturated_blue = saturate(blue, 30);

// Shift hue by 45 degrees
constexpr auto shifted_green = hue_shift(green, 45);

// Blend two colors (50/50 mix)
constexpr auto mixed = blend(red, blue, 50);

// Advanced blending modes
constexpr auto multiplied = multiply(red, blue);
constexpr auto screened = screen(red, blue);
constexpr auto overlaid = overlay(red, blue);

// New: Enhanced blending with compile-time validation
constexpr auto linear_blend = blend<rgba8_t>(red, blue, 25); // 25% red, 75% blue
constexpr auto gamma_corrected = blend_gamma(red, blue, 50);
```

### Color Palettes

```cpp
// Use predefined color palettes
constexpr auto web_black = palettes::web::safe_colors::black;
constexpr auto web_white = palettes::web::safe_colors::white;

// Grayscale palette
constexpr auto dark_gray = palettes::grayscale::dark_gray;
constexpr auto light_gray = palettes::grayscale::light_gray;

// Warm colors
constexpr auto warm_red = palettes::warm::red;
constexpr auto warm_orange = palettes::warm::orange;

// Cool colors
constexpr auto cool_blue = palettes::cool::blue;
constexpr auto cool_green = palettes::cool::green;

// New: Extended palette support with enhanced documentation
constexpr auto material_red = palettes::material::red_500;
constexpr auto bootstrap_primary = palettes::bootstrap::primary;
```

### Advanced Features

```cpp
// Compile-time color validation
static_assert(rgba8<255, 128, 64>.is_valid());

// Template metaprogramming with colors
template<typename ColorType>
constexpr auto make_lighter(const ColorType& c) {
    return lighten(c, 20);
}

constexpr auto light_red = make_lighter(red);

// New: Enhanced template metaprogramming support
template<typename T, int Scale>
constexpr auto adjust_brightness(const basic_rgb<T, Scale>& color, int adjustment) {
    return lighten(color, adjustment);
}

constexpr auto brighter_green = adjust_brightness(green, 30);
```

## Installation

This is a header-only library. Simply include the main header:

```cpp
#include "color/color.hpp"
```

Or include specific components:

```cpp
#include "color/core/rgb.hpp"
#include "color/conversion/conversion.hpp"
#include "color/operations/blend.hpp"
```

## Requirements

- C++17 or later
- Any C++17-compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **Recommended**: C++20 for enhanced template features and concepts support

## Build System Integration

### CMake

```cmake
find_package(colorcpp REQUIRED)
target_link_libraries(your_target PRIVATE colorcpp::colorcpp)
```

### Manual Installation

```bash
# Copy headers to your project
cp -r include/color /path/to/your/project/
```

## Examples

See the `examples/` directory for complete working examples demonstrating:

- Basic color operations
- Color space conversions
- Advanced blending techniques
- Template metaprogramming with colors
- **New**: Enhanced examples with comprehensive documentation
- **New**: Performance benchmarks comparing compile-time vs runtime operations

## API Reference

### Core Types

- `basic_rgb<T, Scale>` - Template RGB color type
- `basic_hsv<T, Scale>` - Template HSV color type  
- `basic_hsl<T, Scale>` - Template HSL color type
- `basic_cmyk<T, Scale>` - Template CMYK color type
- `rgba8_t` - 8-bit RGB with alpha
- `rgbaf_t` - Floating-point RGB with alpha

### Conversion Functions

- `to_hsv<T, Scale>(color)` - Convert to HSV
- `to_hsl<T, Scale>(color)` - Convert to HSL
- `to_rgb<T, Scale>(color)` - Convert to RGB
- `to_cmyk<T, Scale>(color)` - Convert to CMYK
- `convert<TargetType>(source)` - Generic conversion
- **New**: Enhanced conversion with compile-time validation

### Operations

- `lighten(color, percent)` - Lighten color
- `saturate(color, percent)` - Saturate color
- `hue_shift(color, degrees)` - Shift hue
- `blend(color1, color2, ratio)` - Linear interpolation
- `blend_gamma(color1, color2, ratio)` - Gamma-corrected blending
- `multiply(color1, color2)` - Multiply blending
- `screen(color1, color2)` - Screen blending
- `overlay(color1, color2)` - Overlay blending
- **New**: Enhanced blending operations with improved performance

### Literals

- `_rgb` / `_hex` - RGB color literals (6-digit hex)
- `_rgba` - RGBA color literals (8-digit hex)
- `_hsv` - HSV color literals (decimal format)
- `_hsl` - HSL color literals (decimal format)
- `_cmyk` - CMYK color literals (decimal format)
- `_hsv_hue` - HSV hue shortcuts (pure colors)
- `_hsl_hue` - HSL hue shortcuts (pure colors)
- **New**: Template literal support for compile-time parsing

## 🚀 Recent Updates

### v1.1.0 - Enhanced Documentation & Literals (March 2026)
- **Comprehensive API Documentation**: Added detailed Doxygen-style documentation for all functions
- **Enhanced Color Literals**: Improved literal operators with explicit alpha support and template variants
- **Template Literal Support**: New compile-time parsing for HSV, HSL, and CMYK literals
- **Hue Shortcuts**: Convenient operators for creating pure, saturated colors
- **Decimal Format Support**: CMYK literals now support decimal format (CCCMMMYYYKKK)
- **Extensive Examples**: Added practical examples for all color spaces and operations

### v1.0.0 - Initial Release
- Complete color space support (RGB, HSV, HSL, CMYK)
- Compile-time color operations
- Type-safe color literals
- Comprehensive color palettes
- Advanced blending operations

## 📖 Documentation

- **API Reference**: Comprehensive documentation with parameter descriptions and examples
- **Usage Guide**: Step-by-step tutorials for common color manipulation tasks
- **Performance Guide**: Best practices for compile-time color operations
- **Migration Guide**: Instructions for upgrading from previous versions

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

### Development Setup

1. Clone the repository
2. Install C++17+ compatible compiler
3. Run tests: `make test`
4. Build examples: `make examples`

### Guidelines

- Follow existing code style and conventions
- Add comprehensive tests for new features
- Update documentation for API changes
- Ensure all changes maintain compile-time performance

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 👥 Authors

- **Merlot.Qi** - *Initial work* - [merlotqi](https://github.com/merlotqi)

## 🙏 Acknowledgments

- Inspired by modern C++ template metaprogramming techniques
- Built on standard color theory and conversion algorithms
- Designed for compile-time performance and type safety
- Special thanks to the C++ community for template metaprogramming advancements

## 📊 Performance

ColorCpp is designed for maximum performance:

- **Compile-time operations**: Zero runtime overhead for color conversions
- **Template metaprogramming**: Type-safe operations validated at compile time
- **Optimized algorithms**: Efficient color space conversion implementations
- **Memory efficient**: No dynamic allocation, minimal memory footprint

## 🔗 Related Projects

- [color-thief-cpp](https://github.com/briangonzalez/color-thief-cpp) - Extract dominant colors from images
- [FastLED](https://github.com/FastLED/FastLED) - Arduino library for LED color control
- [OpenColorIO](https://github.com/AcademySoftwareFoundation/OpenColorIO) - Color management for visual effects

## 📞 Support

For support and questions:

- Create an [issue](https://github.com/merlotqi/colorcpp/issues)
- Join our [Discussions](https://github.com/merlotqi/colorcpp/discussions)
- Email: merlotqi.mc@gmail.com
