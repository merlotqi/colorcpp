# colorcpp

[![CI](https://github.com/merlotqi/colorcpp/actions/workflows/ci.yml/badge.svg)](https://github.com/merlotqi/colorcpp/actions/workflows/ci.yml)

A modern, header-only C++ library for color manipulation and conversion between different color spaces. Supports compile-time operations, perceptually uniform color spaces, and comprehensive color operations.

## ✨ Features

- **Header-only**: No build required, just include and use
- **Multiple color spaces**: RGB, HSV, HSL, HWB, CMYK, Linear RGB, CIELAB, CIELCH, OkLab, OkLCH, CIE XYZ, Display P3
- **Perceptually uniform**: Built-in support for CIELAB and OkLab for perceptual operations
- **Color operations**: Blending, interpolation, palette generation, accessibility checking, color temperature
- **I/O support**: CSS Color 4 parsing (hex, rgb, hsl, oklab, oklch, display-p3, lab, lch, hwb, named colors)
- **Serialization**: JSON and MessagePack adapters for network/config integration
- **Binary IO**: Read/write DaVinci Resolve .cube LUT files (1D and 3D)
- **ANSI terminal**: Colored swatches, palettes, gradients, WCAG contrast previews for debugging
- **Type-safe literals**: User-defined literals for RGB, HSL, HSV, CMYK, OkLab, named colors
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
| **Display P3** | DCI-P3 primaries with D65 white point (wide gamut) |

## Documentation

- **Algorithm reference (reStructuredText):** [docs/reference/index.rst](docs/reference/index.rst) — background, conventions, and links to specs/papers for conversions, ΔE, blending, gamut, accessibility (WCAG 2 + APCA), vision simulation, CSS parsing, and related topics. Start from [docs/index.rst](docs/index.rst) for how this relates to other doc outputs.
- **API reference (Doxygen):** Configure CMake with `-DCOLORCPP_BUILD_DOCS=ON`, build target `doc`, and open the generated HTML (output directory is set in [doxygen/Doxygen.in](doxygen/Doxygen.in)).
- **Sphinx HTML (optional):** Install Sphinx (`pip install -r requirements-docs.txt`), then run `sphinx-build -b html docs docs/_build/html`, or configure with `-DCOLORCPP_BUILD_SPHINX=ON` and build the `sphinx` CMake target.

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

Include [`colorcpp/io/css.hpp`](include/colorcpp/io/css.hpp) or the umbrella [`colorcpp/colorcpp.hpp`](include/colorcpp/colorcpp.hpp). Parsing returns `std::nullopt` on failure (no exceptions).

```cpp
#include <colorcpp/colorcpp.hpp>

auto c = colorcpp::io::css::parse_css_color_rgba8("rgb(255 99 71 / 50%)");
// or any registered color type:
auto hsl = colorcpp::io::css::parse_css_color<colorcpp::core::hsla_float_t>("hsl(120, 100%, 50%)");

colorcpp::io::css::parse_css_color_context css_context;
css_context.dark_theme = true;
css_context.current_color = colorcpp::core::rgbaf_t{1.0f, 0.0f, 0.0f, 0.5f};
css_context.canvas_text = colorcpp::core::rgbaf_t{1.0f, 1.0f, 1.0f, 1.0f};
auto resolved = colorcpp::io::css::parse_css_color_rgba8("light-dark(currentColor, CanvasText)", css_context);
```

**Supported (CSS Color Module Level 4):**

- **Hex:** `#rgb`, `#rgba`, `#rrggbb`, `#rrggbbaa`
- **`rgb()` / `rgba()`:** legacy commas, modern spaces, slash alpha, percentages
- **`hsl()` / `hsla()`:** hue with `deg`/`grad`/`rad`/`turn`, percentage saturation/lightness
- **`oklab(L a b)`:** L as number/percentage, a/b as numbers/percentages
- **`oklch(L C H)`:** L as number/percentage, C as number/percentage, H as hue angle
- **`lab(L a b)`:** CIE L\*a\*b\* with D65 white point
- **`lch(L C H)`:** CIE L\*C\*h\* cylindrical form
- **`hwb(H W B)`:** Hue-Whiteness-Blackness
- **`color(...)`:** `srgb`, `srgb-linear`, `display-p3`, `display-p3-linear`, `a98-rgb`, `prophoto-rgb`, `rec2020`, `xyz`, `xyz-d50`, `xyz-d65`
- **`device-cmyk(...)`:** CMYK device colors, including slash alpha
- **Named colors and keywords:** all 140+ CSS named colors plus `transparent` — case-insensitive

**Context-aware support:** `currentColor`, CSS system colors, and `light-dark()` are available through the overloads that accept `parse_css_color_context`.

**Still pending:** relative color syntax (`rgb(from …)`, `color(from …)`), broader `color-mix()` coverage, and the remaining context-sensitive CSS color features that depend on authoring-time style state.

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
using namespace colorcpp::operations::accessibility;

// WCAG 2.x contrast ratio (do not compare numerically to APCA Lc)
auto ratio = contrast_ratio(foreground, background);
bool passes_aa = passes_aa_standard(ratio);  // 4.5:1 for normal text
bool passes_aaa = passes_aaa_standard(ratio); // 7:1 for normal text

// APCA Lc (SAPC / Silver draft style; text vs background order matters; thresholds are not WCAG 2)
float lc = apca_contrast(foreground, background);
bool strong_enough = apca_meets_min_abs_lc(foreground, background, 60.0f);

// Find accessible colors (WCAG ratio space)
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

### Named Color Literals

```cpp
using namespace colorcpp::io::css::named_literal;

// CSS named color → rgba8_t
auto coral = "coral"_color;       // → rgba8_t{255, 127, 80, 255}
auto red = "red"_color;           // → rgba8_t{255, 0, 0, 255}
auto steelblue = "steelblue"_color; // → rgba8_t{70, 130, 180, 255}

// Case-insensitive
auto gold = "GOLD"_color;         // → rgba8_t{255, 215, 0, 255}

// Lookup by name
auto c = get_named_color("coral");  // → std::optional<rgba8_t>
bool valid = is_named_color("red"); // → true
```

### Serialization (JSON / MessagePack)

```cpp
#include <colorcpp/io/serialization.hpp>

// Specialize json_adapter for your JSON library (e.g. nlohmann::json)
// Then use to_json / from_json for any color type:

auto j = serialization::to_json<nlohmann::json>(coral_color);
auto recovered = serialization::from_json<nlohmann::json, rgba8_t>(j);

// Named format with custom channel names
std::string names[] = {"red", "green", "blue", "alpha"};
auto j_named = serialization::to_json<nlohmann::json>(coral, names, opts);
```

### Binary IO (LUT Files)

```cpp
#include <colorcpp/io/binary_io.hpp>

// Read a DaVinci Resolve .cube LUT file
auto lut = binary_io::cube::read_3d("grade.cube");

// Apply LUT to a color
auto graded = binary_io::apply(*lut, 0.5f, 0.3f, 0.8f);

// Write a LUT file
binary_io::cube::write("output.cube", *lut, "My Grade");
```

### ANSI Terminal Output

```cpp
#include <colorcpp/io/ansi.hpp>

// Print colored swatch with info
auto c = "coral"_color;
ansi::print_color(std::cout, c, "coral");
// Output: ██████ coral #ff7f50ff  RGB(255,127,80)

// Print palette
ansi::print_palette(std::cout, colors, count);

// Print gradient
ansi::print_gradient(std::cout, "red"_color, "blue"_color);

// Print WCAG contrast check
ansi::print_contrast(std::cout, "white"_color, "navy"_color);
// Output:  Aa  #fff on #000080  contrast: 14.4:1  AAA
```

### Color Temperature

```cpp
#include <colorcpp/algorithms/color_temperature.hpp>

// Kelvin → sRGB (1000–40000K)
auto warm = color_temperature::kelvin_to_rgba8(2700.0f);   // Warm white
auto daylight = color_temperature::kelvin_to_rgba8(6500.0f); // D65 white
auto cool = color_temperature::kelvin_to_rgba8(10000.0f);  // Overcast sky

// Float version
auto linear = color_temperature::kelvin_to_linear_rgb(5500.0f);
auto srgb = color_temperature::kelvin_to_rgb(5500.0f);
```

## 📁 Project Structure

```
include/colorcpp/
├── colorcpp.hpp              # Main header (includes everything)
├── core/
│   ├── core.hpp              # Core includes
│   ├── color_base.hpp        # Base color type definitions
│   ├── concepts.hpp          # Type traits and concepts
│   ├── constants.hpp         # Named color constants
│   ├── io.hpp                # Stream I/O operators
│   ├── rgb.hpp               # RGB / RGBA (8-bit & float)
│   ├── linear_rgb.hpp        # Linear RGB (no gamma)
│   ├── hsl.hpp               # HSL / HSLA
│   ├── hsv.hpp               # HSV / HSVA
│   ├── hwb.hpp               # HWB / HWBA
│   ├── cmyk.hpp              # CMYK
│   ├── cielab.hpp            # CIELAB / CIELCH
│   ├── oklab.hpp             # OkLab / OkLCH
│   ├── xyz.hpp               # CIE XYZ
│   └── display_p3.hpp        # Display P3
├── algorithms/
│   ├── algorithms.hpp        # Algorithm includes
│   ├── accessibility.hpp     # WCAG 2 + APCA Lc
│   ├── color_temperature.hpp # Kelvin → RGB conversion
│   ├── delta_e.hpp           # ΔE76 / ΔE94 / ΔE2000
│   ├── gamut.hpp             # Gamut mapping
│   ├── gradient.hpp          # Gradient generation
│   ├── harmony.hpp           # Color harmony
│   └── vision.hpp            # Color blindness simulation
├── operations/
│   ├── operations.hpp        # Operation includes
│   ├── conversion.hpp        # Color space conversion
│   ├── blend.hpp             # Blending modes
│   ├── compare.hpp           # Color comparison
│   ├── interpolate.hpp       # Color interpolation
│   ├── palette.hpp           # Palette generation
│   ├── harmony.hpp           # Harmony operations
│   └── random.hpp            # Random color generation
├── io/
│   ├── io.hpp                # I/O aggregate
│   ├── css.hpp               # CSS Color 4 parsing
│   ├── css/                  # CSS parsing internals
│   ├── literals.hpp          # User-defined literals
│   ├── literals/             # Literal operators
│   ├── serialization.hpp     # JSON / MessagePack adapters
│   ├── serialization/        # Serialization internals
│   ├── binary_io.hpp         # LUT file formats
│   ├── binary_io/            # Binary IO internals
│   └── ansi.hpp              # ANSI terminal output
└── algorithms/
    └── (see above)
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
