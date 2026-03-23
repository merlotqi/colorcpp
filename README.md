# ColorCpp

[![CI](https://github.com/merlotqi/colorcpp/actions/workflows/ci.yml/badge.svg)](https://github.com/merlotqi/colorcpp/actions/workflows/ci.yml)

A modern, header-only C++ library for color manipulation and conversion between different color spaces. Supports compile-time operations, perceptually uniform color spaces, and comprehensive color operations.

## ✨ Features

- **Header-only**: No build required, just include and use
- **Multiple color spaces**: RGB, HSV, HSL, CMYK, Linear RGB, CIELAB, CIELCH, OkLab, OkLCH, CIE XYZ
- **Perceptually uniform**: Built-in support for CIELAB and OkLab for perceptual operations
- **Color operations**: Blending, interpolation, palette generation, accessibility checking
- **I/O support**: Parse colors from strings, format colors for output; CSS Color 4-style `rgb()` / `hsl()` / hex (see below)
- **Type-safe literals**: User-defined literals for convenient color creation
- **Template-based**: Zero-cost abstractions with compile-time validation
- **C++17+**: Works with any C++17 compatible compiler

## 📦 Supported Color Spaces

| Color Space | Description |
|-------------|-------------|
| **RGB** | Standard red-green-blue (8-bit and float) |
| **HSV** | Hue-Saturation-Value (cylindrical) |
| **HSL** | Hue-Saturation-Lightness (cylindrical) |
| **CMYK** | Cyan-Magenta-Yellow-Key (printing) |
| **Linear RGB** | Gamma-corrected RGB (no sRGB transfer) |
| **CIELAB** | CIE L\*a\*b\* (perceptual uniformity, D65) |
| **CIELCH** | Cylindrical form of CIELAB |
| **OkLab** | Modern perceptually uniform space (Björn Ottosson) |
| **OkLCH** | Cylindrical form of OkLab |
| **CIE XYZ** | Device-independent reference space (D65) |

## 🚀 Quick Start

### Installation

**CMake (recommended)**
```cmake
find_package(colorcpp REQUIRED)
target_link_libraries(your_target PRIVATE colorcpp::colorcpp)
```

**Header-only**: Copy `include/colorcpp/` to your project

### Basic Usage

```cpp
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

// Create colors using type aliases
constexpr auto red = core::rgba8_t{255, 0, 0, 255};
constexpr auto blue = core::rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f};

// Convert between color spaces
auto hsv_red = conversion::color_cast<core::hsv_float_t>(red);
auto lab_red = conversion::color_cast<core::cielab_t>(red);

// Color operations
auto blended = blend::blend(red, blue, blend::blend_mode::multiply);
auto lighter = operations::lighten(red, 20);  // Lighten by 20%
```

## 📚 Color Literals

```cpp
using namespace colorcpp::literals;

// RGB/Hex literals
auto coral = 0xFF6347_rgb;           // → rgba8_t{255, 99, 71, 255}
auto with_alpha = 0xFF634780_rgba;   // → rgba8_t{255, 99, 71, 128}
auto argb = 0x80FF6347_argb;         // → rgba8_t{255, 99, 71, 128} (AARRGGBB)
auto from_hex = "#FF6347"_hex;        // → rgba8_t

// HSL literals
auto mint = 160'070'080_hsl;         // → hsl_float_t{160, 70, 80}
auto with_alpha = 160'070'080'085_hsla; // → hsla_float_t{160, 70, 80, 85}

// HSV literals
auto sky = 210'080'090_hsv;          // → hsv_float_t{210, 80, 90}
auto with_alpha = 210'080'090'075_hsva; // → hsva_float_t{210, 80, 90, 75}

// CMYK literals
auto teal = 50'030'000'020_cmyk;     // → cmyk8_t{50, 30, 0, 20}
```

## CSS color parsing

Include [`colorcpp/core/css_color.hpp`](include/colorcpp/core/css_color.hpp) or the umbrella [`colorcpp/colorcpp.hpp`](include/colorcpp/colorcpp.hpp). Parsing returns `std::nullopt` on failure (no exceptions).

```cpp
#include <colorcpp/colorcpp.hpp>

auto c = colorcpp::core::parse_css_color_rgba8("rgb(255 99 71 / 50%)");
// or any registered color type:
auto hsl = colorcpp::core::parse_css_color<colorcpp::core::hsla_float_t>("hsl(120 100% 50%)");
```

**Supported (subset of CSS Color Module Level 4):**

- Hex: `#rgb`, `#rgba`, `#rrggbb`, `#rrggbbaa` (optional surrounding ASCII whitespace).
- `rgb()` / `rgba()`: legacy commas (`rgb(255, 99, 71)`), modern spaces (`rgb(255 99 71)`), slash alpha (`rgb(255 99 71 / 50%)`), mixing numbers (0–255) and percentages per channel, alpha as number or percentage.
- `hsl()` / `hsla()`: comma or space syntax; hue with optional `deg`, `grad`, `rad`, `turn` (bare number = degrees); saturation/lightness as `%` or legacy 0–100 number scaled to [0, 1]; optional alpha (comma or `/`).

**Not supported yet:** named colors (`red`, `transparent`, …), `color()`, `lab()` / `lch()` / `oklab()` / `oklch()`, `hwb()`, `device-cmyk()`, relative color syntax (`rgb(from …)`). These may be added in later releases.

## 🔄 Color Conversion

```cpp
using namespace colorcpp::conversion;

// Convert to any color space
auto lab = color_cast<core::cielab_t>(rgb_color);
auto oklch = color_cast<core::oklch_t>(rgb_color);
auto xyz = color_cast<core::xyz_t>(rgb_color);

// sRGB ↔ Linear RGB (gamma handling)
auto linear = color_cast<core::linear_rgbf_t>(srgb_color);
auto gamma = color_cast<core::rgba8_t>(linear_color);

// CIELAB ↔ CIELCH (cartesian ↔ polar)
auto lch = color_cast<core::cielch_t>(lab_color);
auto cartesian = color_cast<core::cielab_t>(lch_color);

// OkLab ↔ OkLCH
auto ok = color_cast<core::oklab_t>(lch_color);
auto polar = color_cast<core::oklch_t>(oklab_color);

// Cross-space conversions (automatically routed)
auto lab_to_ok = color_cast<core::oklab_t>(lab_color);  // via XYZ (no gamut clip)
```

## 🎨 Color Operations

### Blending

```cpp
using namespace colorcpp::blend;

// Basic blend modes
auto normal = blend(red, blue);
auto multiply = blend(red, blue, blend_mode::multiply);
auto screen = blend(red, blue, blend_mode::screen);
auto overlay = blend(red, blue, blend_mode::overlay);

// All modes: normal, multiply, screen, overlay, darken, lighten,
// addition, subtraction, difference, exclusion, hard_light,
// soft_light, color_dodge, color_burn, divide

// With opacity control
auto semi_transparent = blend(red, blue, blend_mode::multiply, 0.5f);

// Non-separable blends (hue, saturation, color, luminosity)
auto hue_blend = blend(red, blue, blend_mode::hue);
```

### Palette Generation

```cpp
using namespace colorcpp::palette;

// Generate color scales
auto linear_palette = linear_scale(red, blue, 5);
auto visual_palette = visual_scale(red, blue, 5);           // HSL interpolation
auto perceptual_palette = perceptual_scale(red, blue, 5); // OkLab interpolation

// Color harmony generators
auto complementary = generate::complementary(base_color);
auto analogous = generate::analogous(base_color);
auto triadic = generate::triadic(base_color);
auto split_complementary = generate::split_complementary(base_color);
auto tetradic = generate::tetradic(base_color);
auto square = generate::square(base_color);
auto monochromatic = generate::monochromatic(base_color);

// Access palette colors
for (const auto& color : perceptual_palette) {
    // ...
}
auto first = perceptual_palette[0];
```

### Interpolation

```cpp
using namespace colorcpp::interpolate;

// Linear interpolation
auto mid = lerp(red, blue, 0.5f);

// HSL interpolation (visually smoother)
auto smooth = lerp_hsl(red, blue, 0.5f);

// Perceptual interpolation via OkLab
auto perceptual = lerp_oklab(red, blue, 0.5f);
```

### Accessibility

```cpp
using namespace colorcpp::accessibility;

// WCAG contrast ratio
auto ratio = contrast_ratio(foreground, background);
bool passes_aa = passes_aa_standard(ratio);  // 4.5:1 for normal text
bool passes_aaa = passes_aaa_standard(ratio); // 7:1 for normal text

// Find accessible colors
auto accessible = find_contrast_color(foreground, background, 4.5f);
```

### Vision Simulation

```cpp
using namespace colorcpp::vision;

// Simulate color blindness
auto protanopia = simulate_protanopia(color);
auto deuteranopia = simulate_deuteranopia(color);
auto tritanopia = simulate_tritanopia(color);

// Check if colors are distinguishable under different conditions
bool distinguishable = is_distinguishable(c1, c2, deficiency::deuteranopia);
```

### Gamut Clipping

```cpp
using namespace colorcpp::gamut;

// Check if color is within sRGB gamut
bool in_gamut = is_in_gamut(color);

// Clip to sRGB gamut
auto clipped = clip_to_gamut(color);

// Check clip amount
auto [clipped_color, distance] = clip_with_distance(color);
```

### Delta E (Color Difference)

```cpp
using namespace colorcpp::delta_e;

// Different Delta E metrics
auto de00 = delta_e_2000(color1, color2);  // CIEDE2000 (best for human perception)
auto de94 = delta_e_1994(color1, color2);  // CIE94
auto de76 = delta_e_1976(color1, color2);  // CIE76 (simple Euclidean)

// Threshold comparisons
bool imperceptible = delta_e_2000(c1, c2) < 1.0f;
bool acceptable = delta_e_2000(c1, c2) < 3.0f;
```

### Random Colors

```cpp
using namespace colorcpp::random;

// Random colors in different spaces
auto random_rgb = random_rgb_color();        // Random RGB
auto random_hsl = random_hsl_color();        // Random HSL
auto random_lab = random_lab_color();        // Random CIELAB (perceptually uniform)

// With seed for reproducibility
auto seeded = random_rgb_color(seed);
```

## 📖 I/O Operations

```cpp
using namespace colorcpp::core::io;

// Output formatting
std::cout << rgba_color << "\n";          // "255 99 71 255"
std::cout << std::hex << rgba_color;     // "#ff6347ff"

// Parse from strings
auto rgb = parse<rgb8_t>("255 99 71");
auto hsl = parse<hsl_float_t>("hsl(16, 100, 63.9)");
auto cmyk = parse<cmyk8_t>("cmyk(0, 61, 72, 0)");

// Round-trip: output → re-parse
std::ostringstream oss;
oss << color;
auto recovered = parse<decltype(color)>(oss.str());
```

## 📁 Project Structure

```
include/colorcpp/
├── colorcpp.hpp           # Main header (includes everything)
├── core/
│   ├── core.hpp           # Core includes
│   ├── color_base.hpp     # Base color type definitions
│   ├── constants.hpp      # Named color constants
│   ├── rgb.hpp           # RGB color space
│   ├── hsv.hpp           # HSV color space
│   ├── hsl.hpp           # HSL color space
│   ├── cmyk.hpp          # CMYK color space
│   ├── linear_rgb.hpp     # Linear RGB (no gamma)
│   ├── cielab.hpp        # CIELAB / CIELCH
│   ├── oklab.hpp         # OkLab / OkLCH
│   ├── xyz.hpp           # CIE XYZ
│   ├── io.hpp            # I/O (parsing/formatting)
│   └── css_color.hpp     # CSS Color 4 subset (hex, rgb, hsl)
├── operations/
│   ├── operations.hpp     # Operations includes
│   ├── conversion.hpp     # Color space conversion
│   ├── blend.hpp          # Blending modes
│   ├── palette.hpp        # Palette generation
│   ├── interpolate.hpp    # Color interpolation
│   ├── delta_e.hpp        # Color difference metrics
│   ├── gamut.hpp          # Gamut checking/clipping
│   ├── accessibility.hpp  # WCAG accessibility
│   ├── vision.hpp         # Color vision simulation
│   └── random.hpp         # Random color generation
├── literals/
│   └── literals.hpp       # User-defined literals
└── traits/
    └── concepts.hpp       # Type traits and concepts
```

## 🔧 Requirements

- **C++17** or later
- Any C++17-compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **Optional**: C++20 for enhanced template features

## 📝 Examples

See the `examples/` directory for complete working examples:

- `io_example.cpp` - Parsing and formatting colors
- `cast_example.cpp` - Color space conversions
- `blend_example.cpp` - Blending operations
- `interpolate_example.cpp` - Color interpolation
- `palette_example.cpp` - Palette generation
- `random_example.cpp` - Random color generation

## ⏱️ Benchmarks

Optional microbenchmarks use [Google Benchmark](https://github.com/google/benchmark), fetched with CMake `FetchContent` when enabled:

```bash
cmake -B build -DCOLORCPP_BUILD_BENCHMARKS=ON
cmake --build build
./build/benchmarks/colorcpp_benchmark   # or: cmake --build build --target run_benchmarks
```

Sources under `benchmarks/` are split by area: `benchmark_conversion.cpp`, `benchmark_blend.cpp`, `benchmark_interpolate.cpp`, `benchmark_delta_e.cpp`, `benchmark_io.cpp`, `benchmark_gamut_palette.cpp` — all linked into the single `colorcpp_benchmark` binary.

## API reference (Doxygen)

HTML API documentation is generated from headers with [Doxygen](https://www.doxygen.nl/). Configure with `COLORCPP_BUILD_DOCS=ON`, build the `doc` target, then open `build/doc_doxygen/html/index.html` in a browser:

```bash
cmake -B build -DCOLORCPP_BUILD_DOCS=ON
cmake --build build --target doc
```

Requires `doxygen` on your `PATH`. The Doxygen config template lives in `doxygen/Doxygen.in` (`EXTRACT_ALL` is off so only documented symbols are listed; see `WARN_IF_UNDOCUMENTED` in that file).

## 🐳 Docker & CI

**Docker** — configure, build examples, and run unit tests inside Ubuntu 24.04:

```bash
docker build -t colorcpp:verify .
# or
docker compose build verify
```

**Interactive shell** with the repo bind-mounted at `/src`:

```bash
docker compose run --rm dev
```

**GitHub Actions** (`.github/workflows/ci.yml`): on push/PR to `main`, `master`, or `channel`, runs a **GCC** and **Clang** matrix (Ninja + CMake) with tests and examples enabled, plus a **Docker** build of the `verify` stage.

**Publishing** (`.github/workflows/docker-publish.yml`): pushing a tag `v*` builds the same `verify` image and pushes it to **GHCR** as `ghcr.io/<owner>/<repo>` (useful as a reproducible toolchain snapshot, not a runtime dependency for consumers).

## 📄 License

MIT License - see LICENSE file for details.

## 👥 Author

**Merlot.Qi** - [merlotqi](https://github.com/merlotqi)

## 🙏 Acknowledgments

- CIE color space definitions per ISO 11664-4
- OkLab by [Björn Ottosson](https://bottosson.github.io/posts/oklab/)
- sRGB gamma per IEC 61966-2-1
- W3C blend mode specifications
