# ColorCpp

A modern, header-only C++ library for compile-time color manipulation and conversion between different color spaces.

## Features

- **Compile-time color operations**: All color conversions and manipulations happen at compile time
- **Multiple color spaces**: RGB, HSV, HSL, and CMYK support
- **Type-safe color literals**: Modern C++ user-defined literals for colors
- **Template-based design**: Zero-runtime overhead with compile-time validation
- **Comprehensive color palettes**: Built-in web-safe and named colors
- **Advanced blending operations**: Linear interpolation and standard graphics blending modes

## Color Spaces

### RGB (Red-Green-Blue)
- 8-bit integer RGB (`rgba8_t`) with values 0-255
- Floating-point RGB (`rgbaf_t`) with values 0.0-1.0
- Template-based RGB with compile-time validation

### HSV (Hue-Saturation-Value)
- Integer HSV with hue 0-359°, saturation/value 0-100%
- Floating-point HSV with hue 0-360°, saturation/value 0.0-1.0
- Natural color manipulation in cylindrical coordinates

### HSL (Hue-Saturation-Lightness)
- Similar to HSV but with lightness instead of value
- Better for creating lighter/darker color variants
- Template-based with compile-time validation

### CMYK (Cyan-Magenta-Yellow-Key)
- Professional printing color space
- Integer and floating-point variants
- Template-based with compile-time validation

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

// Create HSV colors
constexpr auto orange = hsv_int<30, 100, 100>;
constexpr auto purple = 300100100_hsv;  // H=300, S=100, V=100

// Create HSL colors
constexpr auto cyan = hsl_int<180, 100, 50>;
constexpr auto magenta = 300100050_hsl;  // H=300, S=100, L=50
```

### Color Conversion

```cpp
// Convert between color spaces
constexpr auto hsv_red = to_hsv(red);
constexpr auto rgb_blue = to_rgb(blue_hsv);

// Automatic type conversion
constexpr auto converted = convert<rgba8_t>(hsv_red);
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

## API Reference

### Core Types

- `basic_rgb<T, Scale>` - Template RGB color type
- `basic_hsv<T, Scale>` - Template HSV color type  
- `basic_hsl<T, Scale>` - Template HSL color type
- `basic_cmyk<T, Scale>` - Template CMYK color type

### Conversion Functions

- `to_hsv<T, Scale>(color)` - Convert to HSV
- `to_hsl<T, Scale>(color)` - Convert to HSL
- `to_rgb<T, Scale>(color)` - Convert to RGB
- `to_cmyk<T, Scale>(color)` - Convert to CMYK
- `convert<TargetType>(source)` - Generic conversion

### Operations

- `lighten(color, percent)` - Lighten color
- `saturate(color, percent)` - Saturate color
- `hue_shift(color, degrees)` - Shift hue
- `blend(color1, color2, ratio)` - Linear interpolation
- `multiply(color1, color2)` - Multiply blending
- `screen(color1, color2)` - Screen blending
- `overlay(color1, color2)` - Overlay blending

### Literals

- `_rgb` / `_hex` - RGB color literals
- `_hsv` - HSV color literals
- `_hsl` - HSL color literals
- `_hsv_hue` - HSV hue shortcuts
- `_hsl_hue` - HSL hue shortcuts

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Author

Merlot.Qi - merlotrain.mc@gmail.com

## Acknowledgments

- Inspired by modern C++ template metaprogramming techniques
- Built on standard color theory and conversion algorithms
- Designed for compile-time performance and type safety
