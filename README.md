# Colorcpp

A modern, header-only C++ library for compile-time color space conversions and operations. Built with C++20 features and template metaprogramming for zero-runtime overhead color manipulation.

## 🚀 Features

- **Compile-time Operations**: All color conversions and operations happen at compile time with zero runtime overhead
- **Multiple Color Spaces**: Support for RGB, HSV, HSL, and CMYK color spaces
- **Type Safety**: Strong type system with compile-time validation and constraints
- **Template Metaprogramming**: Advanced C++20 template features for maximum performance
- **Color Operations**: Built-in operations for blending, filtering, and adjustments
- **Color Palettes**: Predefined color palettes including web-safe colors
- **User-defined Literals**: Convenient literal operators for color creation
- **Header-only**: No external dependencies, easy integration

## 🎨 Supported Color Spaces

### RGB (Red, Green, Blue)
```cpp
#include <color.hpp>

using namespace color;

// Create RGB colors
auto red = core::rgb8<255, 0, 0>;
auto blue = core::rgbf<1000, 0, 0>;  // Floating point
```

### HSV (Hue, Saturation, Value)
```cpp
// Create HSV colors
auto hsv_red = core::hsv_int<0, 100, 100>;
auto hsv_blue = core::hsv_int<240, 100, 100>;
```

### HSL (Hue, Saturation, Lightness)
```cpp
// Create HSL colors
auto hsl_red = core::hsl_int<0, 100, 50>;
auto hsl_blue = core::hsl_int<240, 100, 50>;
```

### CMYK (Cyan, Magenta, Yellow, Key)
```cpp
// Create CMYK colors
auto cmyk_red = core::cmyk_int<0, 100, 100, 0>;
auto cmyk_blue = core::cmyk_int<100, 0, 0, 0>;
```

## 🔄 Color Conversions

Seamless compile-time conversions between all supported color spaces:

```cpp
#include <color.hpp>

using namespace color;

// RGB to HSV conversion
auto rgb_red = core::rgb8<255, 0, 0>;
auto hsv_red = conversion::rgb_to_hsv_t<rgb_red>;  // Compile-time conversion

// HSV to RGB conversion
auto hsv_blue = core::hsv_int<240, 100, 100>;
auto rgb_blue = conversion::hsv_to_rgb_t<hsv_blue>;

// HSL to CMYK conversion
auto hsl_green = core::hsl_int<120, 100, 50>;
auto cmyk_green = conversion::hsl_to_cmyk_t<hsl_green>;
```

## 🎛️ Color Operations

### Adjustments
```cpp
#include <color.hpp>

using namespace color;

// Lighten a color by 20%
auto original = core::rgb8<100, 150, 200>;
auto lighter = operations::lighten_t<original, 20>;

// Saturate a color by 30%
auto saturated = operations::saturate_t<original, 30>;

// Shift hue by 45 degrees
auto shifted = operations::hue_shift_t<original, 45>;
```

### Blending
```cpp
// Blend two colors with 50% ratio
auto color1 = core::rgb8<255, 0, 0>;  // Red
auto color2 = core::rgb8<0, 0, 255>;  // Blue
auto purple = operations::blend_t<color1, color2, 50>;

// Common blending modes
auto overlay = operations::blend_ops::overlay<color1, color2>;
auto screen = operations::blend_ops::screen<color1, color2>;
auto multiply = operations::blend_ops::multiply<color1, color2>;
```

### Filters
```cpp
// Convert to grayscale
auto color = core::rgb8<100, 150, 200>;
auto gray = operations::grayscale_t<color>;

// Invert colors
auto inverted = operations::invert_t<color>;

// Apply threshold
auto thresholded = operations::threshold_t<color, 128>;

// Posterize with 4 levels
auto posterized = operations::posterize_t<color, 4>;
```

## 🎨 Color Palettes

### Web Colors
```cpp
#include <color.hpp>

using namespace color;

// Use predefined web colors
auto alice_blue = palettes::web::safe_colors::alice_blue;
auto coral = palettes::web::safe_colors::coral;
auto dark_orange = palettes::web::safe_colors::dark_orange;

// Grayscale palette
auto medium_gray = palettes::web::grayscale::medium_gray;

// Warm colors
auto gold = palettes::web::warm::gold;
auto orange = palettes::web::warm::orange;

// Cool colors
auto navy = palettes::web::cool::navy;
auto teal = palettes::web::cool::teal;
```

### Named Colors
```cpp
// Access W3C standard colors
auto black = core::colors::black;
auto white = core::colors::white;
auto red = core::colors::red;
auto lime = core::colors::lime;  // Bright green
auto blue = core::colors::blue;
auto yellow = core::colors::yellow;
auto cyan = core::colors::cyan;
auto magenta = core::colors::magenta;

// Extended color names
auto maroon = core::colors::maroon;
auto olive = core::colors::olive;
auto navy = core::colors::navy;
auto purple = core::colors::purple;
auto teal = core::colors::teal;
auto silver = core::colors::silver;
auto gray = core::colors::gray;
```

## 🔧 User-defined Literals

Create colors directly in source code using literal operators:

```cpp
#include <color.hpp>

using namespace color::literals;

// RGB/HEX literals
auto red = 0xFF0000_rgb;
auto blue = 0x0000FF_hex;
auto green = 00FF00_rgb;

// HSV literals (format: H*10000 + S*100 + V)
auto hsv_red = 0100100_hsv;    // Hue=0, Sat=100, Value=100
auto hsv_blue = 240100100_hsv;  // Hue=240, Sat=100, Value=100

// HSL literals (format: H*10000 + S*100 + L)
auto hsl_red = 010050_hsl;     // Hue=0, Sat=100, Light=50
auto hsl_blue = 24010050_hsl;   // Hue=240, Sat=100, Light=50

// Simplified hue-only literals
auto hue_red = 0_hsv_hue;      // HSV: 0°, 100%, 100%
auto hue_blue = 240_hsl_hue;   // HSL: 240°, 100%, 50%
```

## 📦 Installation

This is a header-only library. Simply copy the `include/color` directory to your project and include the main header:

```cpp
#include <color.hpp>
```

### Requirements
- C++20 compatible compiler
- No external dependencies

### Build System Integration

#### CMake
```cmake
# Add the color library to your project
target_include_directories(your_target PRIVATE path/to/color/include)
```

#### Manual
```bash
# Copy headers to your project
cp -r include/color /path/to/your/project/
```

## 🧪 Examples

### Basic Usage
```cpp
#include <color.hpp>
#include <iostream>

int main() {
    using namespace color;
    
    // Create colors
    auto red = core::rgb8<255, 0, 0>;
    auto blue = core::rgb8<0, 0, 255>;
    
    // Convert RGB to HSV
    auto hsv_red = conversion::rgb_to_hsv_t<red>;
    
    // Blend colors
    auto purple = operations::blend_t<red, blue, 50>;
    
    // Apply operations
    auto lighter_red = operations::lighten_t<red, 20>;
    
    // Use literals
    auto green = 0x00FF00_rgb;
    auto cyan = 180100100_hsv;
    
    return 0;
}
```

### Advanced Example
```cpp
#include <color.hpp>

using namespace color;

// Create a color palette generator
template<typename BaseColor, int Steps>
struct color_palette {
    template<int Index>
    using color = operations::hue_shift_t<BaseColor, (360 * Index) / Steps>;
};

// Generate a 12-color palette from red
using red_palette = color_palette<core::rgb8<255, 0, 0>, 12>;

// Access palette colors
auto color1 = red_palette::color<0>;   // Red
auto color2 = red_palette::color<1>;   // Orange-red
auto color3 = red_palette::color<2>;   // Orange
// ... etc
```

## 🏗️ Architecture

The library is organized into several modules:

```
include/color/
├── core/           # Basic color types and definitions
├── conversion/     # Color space conversion templates
├── operations/     # Color manipulation operations
├── palettes/       # Predefined color palettes
├── traits/         # Type traits and concepts
└── utils/          # Utility functions and literals
```

### Core Types
- `basic_rgb<T, R, G, B>` - RGB color template
- `basic_hsv<T, H, S, V>` - HSV color template  
- `basic_hsl<T, H, S, L>` - HSL color template
- `basic_cmyk<T, C, M, Y, K>` - CMYK color template

### Type Aliases
- `rgb8<R, G, B>` - 8-bit RGB colors
- `rgbf<R, G, B>` - Floating-point RGB colors
- `hsv_int<H, S, V>` - Integer HSV colors
- `hsl_int<H, S, L>` - Integer HSL colors
- `cmyk_int<C, M, Y, K>` - Integer CMYK colors

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- Built with modern C++20 features
- Inspired by various color theory and computer graphics principles
- Designed for performance and type safety
