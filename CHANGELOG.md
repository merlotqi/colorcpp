# Changelog

All notable changes to `colorcpp` are documented here.

## [1.0.1] - 2026-05-23

v1.0.1 is the first stable release of colorcpp. It consolidates 28+ feature PRs,
reorganising the library into a clean module hierarchy and delivering a
production-ready header-only C++17 colour library.

### Added

#### Core colour spaces
- `core::hwb_t` — Hue-Whiteness-Blackness cylindrical model.
- `core::display_p3_t` — Display P3 wide-gamut colour space with D65 white point.
- `core::cielab_t` / `core::cielch_t` — CIE L\*a\*b\* and cylindrical form (D65).
- `core::oklab_t` / `core::oklch_t` — Perceptually uniform OkLab / OkLCH (Björn Ottosson).
- `core::linear_rgbf_t` — Linear-light sRGB (no transfer function).

#### Operations
- **`operations::conversion`** — Compile-time shortest-path graph routing for
  `color_cast<T>()`. Every registered colour space can convert to every other
  registered space through ranked edge-cost routes. Extension points are
  documented and guarded.
- **`operations::blend`** — 16 blend modes: `normal`, `multiply`, `screen`,
  `overlay`, `darken`, `lighten`, `addition`, `subtraction`, `difference`,
  `exclusion`, `hard_light`, `soft_light`, `color_dodge`, `color_burn`, `divide`,
  plus non-separable `hue`, `saturation`, `color`, `luminosity`. Opt-in SIMD
  fast paths for selected separable modes when `COLORCPP_ENABLE_SIMD=ON`.
- **`operations::interpolate`** — Rich interpolation family: `lerp`, `lerp_hsl`,
  `lerp_oklab`, `lerp_oklch`, `lerp_monotonic_spline` (Fritsch–Carlson),
  `lerp_path`, `lerp_catmull_rom` (uniform / centripetal / chordal). Multi-stop
  piecewise interpolation with positioned colour stops.
- **`operations::random`** — Random colour generation: uniform per colour space,
  luminance-bounded, WCAG/APCA contrast-bounded, golden-angle distributed,
  HSL-harmony constrained, Poisson-disk sampling. Generator families are
  reusable and seedable.
- **`operations::palette`** — Structural palette builders: `linear_scale`,
  `visual_scale` (HSL), `perceptual_scale` (OkLab). Harmony-backed `schemes::`
  (complementary, analogous, triadic, split-complementary, tetradic, square,
  monochromatic). Warm/cool/neutral `families::`. Heuristic `material_*` and
  `theme` design helpers.
- **`operations::compare`** — `exact`, `perceptual`, `relative`, and
  `epsilon`-based colour comparison with explicit cross-type contracts.

#### Algorithms (new top-level module)
- **`algorithms::accessibility`** — WCAG 2.x contrast ratio and compliance
  checks (`wcag_level::aa` / `aaa`, `text_size::normal` / `large`). APCA
  (SAPC / Silver draft) perceptual contrast with direction-aware
  foreground-vs-background semantics.
- **`algorithms::delta_e`** — CIE76 (`ΔE*₇₆`), CIE94, CIEDE2000, CMC(l:c),
  DIN99, and OkLab-based ΔE. Asymmetry contracts enforced for CMC / DIN99.
  Published test vectors for CIEDE2000 and DIN99. Experimental SIMD fast path
  for `delta_e_ok()` via `COLORCPP_ENABLE_EXPERIMENTAL_DELTA_E_OK_SIMD`.
- **`algorithms::gamut`** — Gamut containment checks, single-space and
  multi-colourspace clipping, distance-to-gamut, and mapping-result metadata
  inspection.
- **`algorithms::gradient`** — `linear`, `radial`, `angular`, `box`, and
  `diamond` gradient types. `stepped` quantisation, easing functions, built-in
  presets, position-type-safe stop containers.
- **`algorithms::harmony`** — Rule engine for complementary / analogous /
  triadic / split-complementary / tetradic / square / monochromatic harmony.
  Correction with status, suggestion, assessment, and weighted scoring.
- **`algorithms::vision`** — Protanopia, deuteranopia, tritanopia CVD
  simulation (Machado model). Experimental SIMD fast path via
  `COLORCPP_ENABLE_EXPERIMENTAL_VISION_MACHADO_SIMD`. Accessibility helpers
  (`is_accessible_for_deuteranopia`, `is_accessible_for_all_cvd`).
- **`algorithms::color_temperature`** — Kelvin-to-sRGB conversion (1 000–40 000 K).
- **`algorithms::palette`** — `advanced_scales`, `rainbow`, `families` generators.

#### I/O
- **`io::css`** — Full CSS Color Module Level 4 parser plus selected Level 5
  features:
  * Hex notation (`#rgb`, `#rgba`, `#rrggbb`, `#rrggbbaa`).
  * `rgb()` / `rgba()` — legacy comma and modern space-separated, slash alpha,
    percentages.
  * `hsl()` / `hsla()` / `hwb()` — hue angles in `deg` / `grad` / `rad` / `turn`.
  * `oklab()` / `oklch()`, `lab()` / `lch()`.
  * `color()` function — `srgb`, `srgb-linear`, `display-p3`, `display-p3-linear`,
    `a98-rgb`, `prophoto-rgb`, `rec2020`, `xyz`, `xyz-d50`, `xyz-d65`.
  * `color-mix()` — multi-colour item-list mixes, progress-form mixes, hue
    interpolation for cylindrical spaces.
  * Relative colour syntax `rgb(from …)` / `color(from …)` with arithmetic
    expressions (`+ - * /`), `calc()` wrapper, and nested `var(--token)` resolution.
  * Context-aware parsing: `currentColor`, CSS system colours, `light-dark()`,
    `var()` injection via `variable_resolver` (colour) and
    `numeric_variable_resolver` (scalar).
  * AST delayed evaluation with `parse_css_color_ast()` / `evaluate()`.
  * All 140+ CSS named colours plus `transparent`.
- **`io::literals`** — Compile-time user-defined literals: `_rgb`, `_rgba`,
  `_argb`, `_hex`, `_hsl`, `_hsv`, `_hwb`, `_cmyk`, `_oklab`, `_oklch`,
  `_color` (CSS named colour lookup). Literal contracts are hardened with
  validated range checks.
- **`io::serialization`** — JSON and MessagePack adapter traits.
  `to_json` / `from_json` with `serialization_format::named` and custom
  channel name overrides. MessagePack `pack_color` / `unpack_color` helpers.
- **`io::binary_io`** — DaVinci Resolve `.cube` LUT file read/write (1D and
  3D). LUT application with hardened passthrough for invalid tables.
- **`io::ansi`** — Terminal colour preview helpers: `print_color()`,
  `print_color_verbose()`, `print_palette()` (pointer + contiguous-container
  overloads), `print_gradient()`, `print_contrast()` (WCAG). Stream formatting
  state is fully restored after output. Boundary behaviour is explicit for
  all step counts.

### Changed
- **Module reorganisation** — `operations/` split from flat collections into
  fine-grained sub-modules (`conversion`, `blend`, `compare`, `interpolate`,
  `palette`, `random`). `algorithms/` promoted to a top-level namespace
  (`colorcpp::algorithms`) independent of `operations`. The umbrella header
  `colorcpp.hpp` re-exports convenience aliases for backward compatibility.
- **CSS parser** refactored into dedicated sub-headers (`parse_public.hpp`,
  `parse_detail.hpp`, `parse_templates.hpp`, `color_function.hpp`,
  `relative_color.hpp`, `device_cmyk.hpp`, etc.).
- **`palette_set`** now uses `std::vector::at()` for bounds-checked access.
- **`color_cast`** routes through a compile-time weighted graph instead of a
  hard-coded hub, producing optimal conversion paths and allowing downstream
  registration of custom spaces.
- **Stream I/O** for colour types (`core::io`) uses consistent `parse<T>()` /
  `operator<<` round-trip semantics.

### Fixed
- **Binary I/O hardening** — `apply_color()` preserves input passthrough for
  invalid LUTs instead of silently producing garbage. Regression-tested.
- **Literals correctness** — `_rgb` / `_rgba` / `_argb` enforce value-range
  contracts at compile time. Regression-tested.
- **DIN99 delta E** — Parametric factors now match the published DIN99
  transform; added conformance test vectors.
- **MSVC C++17 compatibility** — `constexpr` math utilities for MSVC in
  harmony / gradient modules.
- **macOS libc++ compatibility** — Float parsing fallback for libc++ < 19.
- **ANSI helpers** — Stream formatting state (fill, precision, flags) is now
  fully restored after `print_color()`, `print_color_verbose()`, and
  `print_contrast()`.

### Build & CI
- **Cross-platform CI matrix**: GCC + Clang (Linux), MSVC + Ninja (Windows),
  MSVC + Visual Studio 17 2022 (Windows), Clang + Ninja (macOS).
- **SIMD CI job** — `COLORCPP_ENABLE_SIMD=ON` tested on every push.
- **Install + consumer smoke test** — Verifies `find_package(colorcpp)` and
  `target_link_libraries(… colorcpp::colorcpp)` end-to-end.
- **Docs CI** — Doxygen API reference + Sphinx narrative docs built and
  verified in CI.
- **Docker** — `Dockerfile` with `verify` target for reproducible builds.
  **CD** publishes verified images to GHCR on `v*` tags.
- **Google Benchmark** — Optional microbenchmarks via
  `-DCOLORCPP_BUILD_BENCHMARKS=ON`. Split by area: `benchmark_conversion`,
  `benchmark_blend`, `benchmark_interpolate`, `benchmark_delta_e`,
  `benchmark_io`, `benchmark_gamut_palette`.
- **Debian packaging** — `debian/` directory for `dpkg-buildpackage` /
  `debuild` producing `libcolorcpp-dev`.

### Removed
- **Flow module** (`operations/flow`) — unmaintained; removed from public API.
- **Legacy algorithm headers** — `operations/vision.hpp` and
  `operations/accessibility.hpp` replaced by `algorithms/vision/` and
  `algorithms/accessibility/`.
- **`ChangeLog`** (capital-C, capital-L) — consolidated into `CHANGELOG.md`.

### API Notes
- The canonical namespaces are `colorcpp::core`, `colorcpp::operations::*`,
  `colorcpp::algorithms::*`, and `colorcpp::io::*`. The umbrella
  `colorcpp.hpp` also re-exports `core`, `operations`, and `algorithms` into
  `colorcpp` for convenience. New code should prefer the canonical nested
  namespaces.
- `color_cast<T>()` is the single entry point for all colour-space
  conversions. Extension is through the documented registration API.
- `blend()` currently evaluates through `rgbaf_t` — the shipped implementation
  is correct for opaque sRGB content but should not be read as a proof of a
  linear-sRGB compositing contract.
- `random_contrast_color()` is best-effort within an internal attempt budget;
  it is not an unconditional guarantee.
- CSS `parse_css_color*()` overloads return `std::nullopt` on failure (no
  exceptions thrown).

---

## [0.3.0] - 2026-05-14

### Added
- Header-only Debian packaging support for `libcolorcpp-dev`.
- GitHub Release-ready release notes and a structured changelog entry for the current release.
- ANSI palette container overload for contiguous containers in C++17-friendly form.

### Changed
- ANSI helper output now restores caller stream formatting state after `print_color()`, `print_color_verbose()`, and `print_contrast()`.
- `print_gradient()` now has explicit boundary behavior for `steps <= 1`.
- ANSI docs and examples now describe the converted `rgba8_t` terminal preview model more explicitly.

### Packaging
- Added Debian packaging files under `debian/` for `dpkg-buildpackage` / `debuild` workflows.
- Added `.gitignore` entries for Debian build artifacts.

### API Notes
- `ansi::print_palette()` now accepts a contiguous container overload in addition to pointer + count.
- `ansi::print_gradient()` now defines behavior for zero and one step.
- Stream-formatting helpers no longer leak fixed/precision/fill state to caller streams.

### Release Process
- Tag the release as `v0.3.0`.
- Attach the source archive generated by GitHub Releases.
- Copy the contents of `docs/release-notes/v0.3.0.md` into the GitHub Release description.
- Follow the full checklist in `docs/release-process/v0.3.0.md`.
