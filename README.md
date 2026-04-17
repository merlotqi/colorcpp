# colorcpp

[![GitHub CI](https://github.com/merlotqi/colorcpp/actions/workflows/ci.yml/badge.svg)](https://github.com/merlotqi/colorcpp/actions/workflows/ci.yml)

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

### API Layout

Canonical public namespaces:

- `colorcpp::core` for color types, constants, and stream I/O
- `colorcpp::operations::{conversion, blend, interpolate, palette, flow, random, compare}`
- `colorcpp::algorithms::{accessibility, color_temperature, delta_e, gamut, gradient, harmony, vision}`
- `colorcpp::io::{css, literals, serialization, binary_io, ansi}`

When you include [`colorcpp/colorcpp.hpp`](include/colorcpp/colorcpp.hpp), `core`, `operations`, and `algorithms`
are also re-exported into `colorcpp` for convenience, so `colorcpp::conversion`-style code remains valid. The docs and
examples below use the canonical nested namespaces; `colorcpp::io` stays under `colorcpp::io`.

### Basic Usage

```cpp
#include <colorcpp/colorcpp.hpp>

using namespace colorcpp::core;
using namespace colorcpp::operations::blend;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::operations::flow;

// Create colors using type aliases
constexpr auto red = rgba8_t{255, 0, 0, 255};
constexpr auto blue = rgbaf_t{0.0f, 0.0f, 1.0f, 1.0f};

// Convert between color spaces
auto hsv_red = color_cast<hsv_float_t>(red);
auto lab_red = color_cast<cielab_t>(red);

// Color operations
auto blended = blend(red, blue, blend_mode::multiply);
auto lighter = make(red).lighten(20).to_rgba8();
```

## 📚 Color Literals

```cpp
using namespace colorcpp::io::literals;

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

**Supported (CSS Color Module Level 4 plus selected Level 5 helpers):**

- **Hex:** `#rgb`, `#rgba`, `#rrggbb`, `#rrggbbaa`
- **`rgb()` / `rgba()`:** legacy commas, modern spaces, slash alpha, percentages
- **`hsl()` / `hsla()`:** hue with `deg`/`grad`/`rad`/`turn`, percentage saturation/lightness
- **`hwb()`**
- **`oklab(L a b)`:** L as number/percentage, a/b as numbers/percentages
- **`oklch(L C H)`:** L as number/percentage, C as number/percentage, H as hue angle
- **`lab(L a b)`:** CIE L\*a\*b\* with D65 white point
- **`lch(L C H)`:** CIE L\*C\*h\* cylindrical form
- **`hwb(H W B)`:** Hue-Whiteness-Blackness
- **`color(...)`:** `srgb`, `srgb-linear`, `display-p3`, `display-p3-linear`, `a98-rgb`, `prophoto-rgb`, `rec2020`, `xyz`, `xyz-d50`, `xyz-d65`
- **`color-mix()`:** `srgb`, `srgb-linear`, `display-p3`, `display-p3-linear`, `lab`, `lch`, `oklab`, `oklch`, `xyz`
- **`device-cmyk(...)`:** CMYK device colors, including slash alpha
- **Named colors and keywords:** all 140+ CSS named colors plus `transparent` — case-insensitive

**Context-aware support:** `currentColor`, CSS system colors, and `light-dark()` are available through the overloads that accept `parse_css_color_context`.

**Still pending:** relative color syntax (`rgb(from …)`, `color(from …)`), advanced `color-mix()` features such as hue interpolation keywords, and the remaining context-sensitive CSS color features that depend on authoring-time style state.

## 🔄 Color Conversion

```cpp
using namespace colorcpp::core;
using namespace colorcpp::operations::conversion;

// Convert to any color space
auto lab = color_cast<cielab_t>(rgb_color);
auto oklch = color_cast<oklch_t>(rgb_color);
auto xyz = color_cast<xyz_t>(rgb_color);

// sRGB ↔ Linear RGB (gamma handling)
auto linear = color_cast<linear_rgbf_t>(srgb_color);
auto gamma = color_cast<rgba8_t>(linear_color);

// CIELAB ↔ CIELCH (cartesian ↔ polar)
auto lch = color_cast<cielch_t>(lab_color);
auto cartesian = color_cast<cielab_t>(lch_color);

// OkLab ↔ OkLCH
auto ok = color_cast<oklab_t>(lch_color);
auto polar = color_cast<oklch_t>(oklab_color);

// Cross-space conversions (automatically routed)
auto lab_to_ok = color_cast<oklab_t>(lab_color);  // via XYZ (no gamut clip)
```

## 🎨 Color Operations

### Blending

```cpp
using namespace colorcpp::operations::blend;

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
using namespace colorcpp::operations::palette;

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
using namespace colorcpp::operations::interpolate;

// Linear interpolation
auto mid = lerp(red, blue, 0.5f);

// HSL interpolation (visually smoother)
auto smooth = lerp_hsl(red, blue, 0.5f);

// Perceptual interpolation via OkLab
auto perceptual = lerp_oklab(red, blue, 0.5f);

// Shape-preserving cubic interpolation with neighbor anchors
auto smooth_curve = lerp_monotonic_spline(prev, red, blue, next, 0.5f);

// Multi-stop path interpolation
auto path = lerp_path(std::vector{red, 0x00FF00_rgb, blue}, 0.35f);
```

### Accessibility

```cpp
using namespace colorcpp::algorithms::accessibility;

// WCAG 2.x contrast ratio (do not compare numerically to APCA Lc)
auto ratio = contrast_ratio(foreground, background);
bool passes_aa = is_wcag_compliant(foreground, background, wcag_level::aa, text_size::normal);
bool passes_aaa = is_wcag_compliant(foreground, background, wcag_level::aaa, text_size::normal);

// APCA Lc (SAPC / Silver draft style; text vs background order matters; thresholds are not WCAG 2)
float lc = apca_contrast(foreground, background);
bool strong_enough = apca_meets_min_abs_lc(foreground, background, 60.0f);

// Pick black or white text for a background
auto recommended_text = contrast_text_color(background);
```

### Vision Simulation

```cpp
using namespace colorcpp::algorithms::vision;

// Simulate color blindness
auto protanopia = simulate_protanopia(color);
auto deuteranopia = simulate_deuteranopia(color);
auto tritanopia = simulate_tritanopia(color);

// Check if colors remain distinguishable under common CVD models
bool distinguishable = is_accessible_for_deuteranopia(c1, c2);
bool robust = is_accessible_for_all_cvd(c1, c2);
```

### Gamut Clipping

```cpp
using namespace colorcpp::algorithms::gamut;

// Check if color is within sRGB gamut
bool in_gamut = is_in_srgb_gamut(color);

// Clip to sRGB gamut
auto clipped = gamut_clip(color);

// Inspect mapping metadata
float distance = gamut_distance(color);
auto info = gamut_clip_with_info(color);
```

### Delta E (Color Difference)

```cpp
using namespace colorcpp::algorithms::delta_e;

// Different Delta E metrics
auto de00 = delta_e_2000(color1, color2);  // CIEDE2000 (best for human perception)
auto de94 = delta_e_94(color1, color2);    // CIE94
auto de76 = delta_e_76(color1, color2);    // CIE76 (simple Euclidean)

// Threshold comparisons
bool imperceptible = delta_e_2000(c1, c2) < 1.0f;
bool acceptable = delta_e_2000(c1, c2) < 3.0f;
```

### Random Colors

```cpp
using namespace colorcpp::core;
using namespace colorcpp::operations;
using namespace colorcpp::operations::random;

// Random colors in different spaces
auto random_rgb = random_color<rgbf_t>();
auto random_hsl = random_color<hsl_float_t>();
auto random_lab = random_color<cielab_t>();

// With seed for reproducibility
auto seeded = random_color<rgbf_t>(seed);
rgb8_generator generator(seed);
auto sample = generator.next();
```

## 📖 I/O Operations

```cpp
using namespace colorcpp::core;
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

using namespace colorcpp::core;
using namespace colorcpp::io::serialization;

// Specialize json_adapter for your JSON library (e.g. nlohmann::json)
// Then use to_json / from_json for any color type:

auto j = to_json<nlohmann::json>(coral_color);
auto recovered = from_json<nlohmann::json, rgba8_t>(j);

// Named format with custom channel names
std::string names[] = {"red", "green", "blue", "alpha"};
auto j_named = to_json<nlohmann::json>(coral, names, opts);
```

### Binary IO (LUT Files)

```cpp
#include <colorcpp/io/binary_io.hpp>

using namespace colorcpp::io::binary_io;

// Read a DaVinci Resolve .cube LUT file
auto lut = cube::read_3d("grade.cube");

// Apply LUT to a color
auto graded = apply(*lut, 0.5f, 0.3f, 0.8f);

// Write a LUT file
cube::write("output.cube", *lut, "My Grade");
```

### ANSI Terminal Output

```cpp
#include <colorcpp/io/ansi.hpp>

using namespace colorcpp::io::ansi;
using namespace colorcpp::io::css::named_literal;

// Print colored swatch with info
auto c = "coral"_color;
print_color(std::cout, c, "coral");
// Output: ██████ coral #ff7f50ff  RGB(255,127,80)

// Print palette
print_palette(std::cout, colors.data(), colors.size());

// Print gradient
print_gradient(std::cout, "red"_color, "blue"_color);

// Print WCAG contrast check
print_contrast(std::cout, "white"_color, "navy"_color);
// Output:  Aa  #fff on #000080  contrast: 14.4:1  AAA
```

### Color Temperature

```cpp
#include <colorcpp/algorithms/color_temperature.hpp>

using namespace colorcpp::algorithms::color_temperature;

// Kelvin → sRGB (1000–40000K)
auto warm = kelvin_to_rgba8(2700.0f);     // Warm white
auto daylight = kelvin_to_rgba8(6500.0f); // D65 white
auto cool = kelvin_to_rgba8(10000.0f);    // Overcast sky

// Float version
auto linear = kelvin_to_linear_rgb(5500.0f);
auto srgb = kelvin_to_rgb(5500.0f);
```

## 📁 Project Structure

```
.
├── include/colorcpp/         # Public header-only library
│   ├── colorcpp.hpp          # Umbrella header
│   ├── core/                 # Color types, constants, stream I/O
│   ├── operations/           # Conversion, blend, compare, flow, interpolate, palette, random
│   ├── algorithms/           # Accessibility, delta_e, gamut, gradient, harmony, vision, color_temperature
│   ├── io/                   # CSS parsing, literals, serialization, binary IO, ANSI helpers
│   └── detail/               # Internal shared helpers (for example SIMD support)
├── examples/                 # Small runnable examples for major features
├── tests/                    # GoogleTest suites
│   ├── core/                 # Core color space types and primitives
│   ├── operations/           # Conversion, blend, compare, flow, interpolate, palette, random
│   ├── algorithms/           # Delta E, gamut, gradient, harmony, accessibility, vision, etc.
│   ├── io/                   # CSS, literals, serialization, ANSI, binary IO
│   └── test_api_contract.cpp # Umbrella header / namespace contract checks
├── benchmarks/               # Optional Google Benchmark microbenchmarks
├── docs/                     # Narrative Sphinx docs and reference notes
│   └── reference/            # Topic-oriented reference pages
├── cmake/                    # Package config and pkg-config templates
├── doxygen/                  # Doxygen template/config input
├── .github/workflows/        # CI and release workflows
├── CMakeLists.txt            # Main build configuration
├── Dockerfile                # Containerized build / verification image
└── requirements-docs.txt     # Python dependencies for Sphinx docs
```

The public API is centered around `include/colorcpp/`, while `examples/`, `tests/`, and `docs/`
show how the library is expected to be consumed, validated, and documented.

## 🔧 Requirements

- **C++17** or later
- Any C++17-compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **Optional**: C++20 for enhanced template features

## 📝 Examples

See the `examples/` directory for complete working examples:

- `io_example.cpp` - Parsing and formatting colors
- `cast_example.cpp` - Color space conversions
- `interpolate_example.cpp` - Color interpolation
- `palette_example.cpp` - Palette generation
- `random_example.cpp` - Random color generation
- `flow_example.cpp` - Fluent theme / export API
- `accessibility_example.cpp` - WCAG 2 and APCA helpers
- `gamut_example.cpp` - Gamut mapping and clipping
- `delta_e_example.cpp` - Color difference metrics
- `vision_example.cpp` - Vision deficiency simulation
- `serialization_example.cpp` - JSON and MessagePack adapters

## ⏱️ Benchmarks

Optional microbenchmarks use [Google Benchmark](https://github.com/google/benchmark), fetched with CMake `FetchContent` when enabled:

```bash
cmake -B build -DCOLORCPP_BUILD_BENCHMARKS=ON -DCOLORCPP_ENABLE_SIMD=ON
cmake --build build
./build/benchmarks/benchmark_conversion
# or: cmake --build build --target run_all_benchmarks
```

Sources under `benchmarks/` are split by area and each builds into its own executable, such as
`benchmark_conversion`, `benchmark_blend`, `benchmark_interpolate`, `benchmark_delta_e`, `benchmark_io`, and
`benchmark_gamut_palette`.

`COLORCPP_ENABLE_SIMD=ON` is currently an opt-in fast path for selected operations. Confirmed accelerated paths cover
selected separable `blend()` modes. `delta_e_ok()` also has an experimental SIMD-backed implementation; unsupported
targets and all other modes keep the scalar implementation.

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

**GitHub Actions** (`.github/workflows/ci.yml`): on push/PR to `main`, `master`, or `channel`, runs a **GCC** and **Clang** matrix (Ninja + CMake) with tests and examples enabled, plus an **install + consumer** smoke test and a **Docker** build of the `verify` stage.

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
