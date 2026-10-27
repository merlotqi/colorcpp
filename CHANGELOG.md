# Changelog

All notable changes to `colorcpp` are documented here.

## [Unreleased]

### Fixed

- **Alpha preservation across graph routes (breaking fix)**: `color_cast` now
  restores the source alpha on the result whenever both endpoints have alpha.
  Minimal-cost routes cross 3-channel transit nodes (XYZ / OkLab / CIELAB) that
  cannot carry alpha, so conversions such as `rgbaf → rec2020_rgbaf` previously
  arrived with `a = 1.0`. Conversion functions copy alpha verbatim; the restore
  at the public `color_cast` boundary is exact for every route. New opt-in
  `model_traits::has_alpha` marks the 14 alpha models (CMYK's K channel is not
  alpha).

- **CSS `lab()` / `lch()` white point (breaking)**: CSS lab()/lch() are
  D50-based (CSS Color 4 §9.1), but the parser returned raw D50 channels in
  the D65-based `core::cielab_t` / `core::cielch_t`. Absolute parses now
  Bradford-adapt D50→D65 (neutrals unchanged, chromatic values shift by up to
  ~4.5° hue); typed test expectations updated accordingly.
- **Relative channel lists**: a channel beginning with a negative number (e.g.
  `lab(from white 50% 25% -50%)`) was greedily consumed as binary subtraction,
  failing the parse. In channel lists a binary `+`/`-` now requires whitespace
  after the sign (the CSS calc() convention), leaving ` -50%` to the next
  channel; `calc()` and parenthesized groups keep lenient tight arithmetic.
- **Oklab/Oklch** (`oklab.hpp`, gamut clipping): the M2 b-axis row and the
  inverse LMS matrix coefficients did not match the Ottosson/CSS standard
  (b was scaled by ~0.54x). Oklch chroma/hue, `lerp_oklab`, `delta_e_ok`, and
  gamut chroma/hue geometry were affected; corrected to the reference
  constants (0.0259040371 / 0.7827717662 / -0.8086757660 and the matching
  inverse rows).
- **Rec.2020 transfer function** (`rec2020.hpp`, CSS `color(rec2020 …)` and
  relative colors): replaced the BT.2020 broadcast OETF (alpha/beta piecewise)
  with the pure gamma 2.4 (BT.1886) function defined by CSS Color 4
  section 10.8 and used by the reference implementation (color.js);
  encoded values differed from CSS by ~0.05-0.08.
- **Gamut mapping** (`gamut_clip`, default `css_color4`): linear sRGB values
  from `oklab_to_linrgb` were stored into `rgbaf_t` without sRGB encoding,
  darkening mapped colours (lightness shifted by ~0.12 for strongly
  out-of-gamut inputs); the result is now constructed via `linear_rgbaf_t`.
- **Gamut mapping** (`chroma_reduce`): preserve the input alpha instead of
  resetting it to 1.0.
- **CMake**: `-Werror`/`/W4`/`/WX` are no longer exported on the `colorcpp`
  INTERFACE target — consumers no longer inherit the library's warning flags.
  First-party strict warnings moved to an internal `colorcpp_warnings` target;
  `/utf-8` remains on the INTERFACE for MSVC (non-ASCII header comments).

### Added

- **Reference-vector tests**: data-driven harness (`tests/reference/`) that
  converts every ordered pair of color spaces in CSS Color 4 worked examples
  (`css_color4_vectors.json`) via `color_cast`, covering sRGB, sRGB-linear,
  Display P3 (linear), Adobe RGB, ProPhoto RGB, Rec.2020, XYZ D65, Oklab and
  Oklch, plus the §14.2 chroma-reduction gamut-mapping example.
- **`blend()` compositing semantics**: new `blend_space` parameter —
  `blend_space::srgb` (default, W3C/CSS parity: formulas on gamma-encoded
  sRGB) or `blend_space::linear` (linear-light compositing: linearize,
  composite, re-encode). Non-separable modes keep the W3C sRGB-HSL definition
  regardless of `blend_space`. The API contract is now documented: straight
  (un-premultiplied) alpha at the boundary, premultiplied Porter-Duff steps
  internally (W3C Compositing and Blending Level 1). The previous "not a
  linear-sRGB compositing contract" disclaimers in `blend.hpp`, the README and
  the docs are removed.
- **CSS relative color syntax for `hsl()` / `hwb()`**: `hsl(from …)` and
  `hwb(from …)` with channel keywords (`h`/`s`/`l`, `h`/`w`/`b`, `alpha`),
  percentage scales (s/l/w/b: 100), and hue angle units `deg`/`grad`/`rad`/
  `turn` inside relative channel expressions (CSS Color 4 §5).
- **CSS relative color syntax for `lab()` / `lch()`**: `lab(from …)` and
  `lch(from …)` evaluate in the D50 domain (source channels adapted D65→D50,
  result adapted D50→D65) with percentage scales l: 100, a/b: ±125, c: 150,
  h: 360; typed `cielab_t` / `cielch_t` results available.
- **CSS relative color syntax for `oklab()` / `oklch()`**: `oklab(from …)` and
  `oklch(from …)` with percentage scales l: 1.0, a/b: ±0.4, c: 0.4, h: 360;
  typed `oklab_t` / `oklch_t` results available.
- **CSS `contrast-color()`** (CSS Color 5 §8): resolves to white or black,
  whichever contrasts more as text on the argument used as a solid background
  (WCAG 2.x relative luminance; equal contrast resolves to white per the
  spec). The argument is a full `<color>` — `light-dark()`, `color-mix()`,
  relative colors, and variables resolve with the ambient context. Background
  alpha is ignored; the result is opaque. Accepted by `parse_css_color_rgbaf`,
  `parse_css_color_rgba8`, and typed `parse_css_color<Color>()`.

### Changed

- **Conversion graph: shortcut edges removed (breaking)**: all weighted
  shortcut registrations (`route_cost::shortcut_2_hop` / `shortcut_4_hop`) and
  their lower-precision matrix implementations are deleted; every pair routes
  through the canonical Linear → XYZ → Linear chain — the single numeric source
  of truth. Only `display_p3af ↔ rgbaf` actually changes route (its cost-3
  shortcut was inconsistent with its non-alpha twin and the only shortcut edge
  ever picked); results now follow the 16-digit canonical matrices.
- **Hub routing system removed (breaking)**: `color_traits`,
  `color_space_registry.hpp`, `conversion/traits.hpp`, and the hub route-cost
  helpers (`best_route_cost`, `route_cost_via_from_hub`/`route_cost_via_to_hub`,
  `get_hub_t`) are gone. Routing is exclusively the compile-time registered-edge
  graph (`graph.hpp` + `registry.hpp`).

- **`color-mix()` rejects non-absolute colors**: `light-dark()` and
  `contrast-color()` are no longer accepted as `color-mix()` items (CSS Color
  5 forbids non-absolute colors inside `color-mix()`); nested `color-mix()`
  and relative colors remain valid items. Previously `light-dark()` items
  resolved through the ambient context and parsed.
- **README**: documented HWB, Adobe RGB, ProPhoto RGB, Rec.2020, and chromatic
  adaptation; completed the examples and benchmarks lists.

## [1.0.3] - 2026-07-06

Patch release focused on parser robustness, platform fixes, and validation.

### Fixed

- **CSS parsing**: prevent undefined behaviour in hex expansion (`hex_expand`) and
  refactor hue parsing.
- **CSS parsing**: restore the cursor position on failed parenthesized content
  parsing.
- **Float parsing**: use `strtof`/`strtod` on Apple platforms, where
  `std::from_chars` for floating-point is unavailable; simplify the shared
  `parse_float` implementation around `std::from_chars`.
- **`.cube` parser**: return an error on invalid size, domain, or data lines
  instead of accepting malformed LUT files.

### Added

- **Literals**: hex validation and `static_assert` for hex digit counts;
  improved decimal literal parsing and validation.
- **Serialization**: validation for null channel names and NaN values.

### Changed

- **CSS named colors**: case-insensitive lookup without heap allocation (perf).

## [1.0.2] - 2026-06-27

Wide-gamut release adding three RGB working spaces and white-point adaptation.

### Added

- **Core colour spaces**: `core::adobe_rgb_t`, `core::prophoto_rgb_t`, and
  `core::rec2020_t` (with linear and alpha variants), including examples
  (`adobe_rgb_example`, `prophoto_rgb_example`, `rec2020_example`).
- **`algorithms::chromatic_adaptation`** — Bradford and Von Kries white-point
  adaptation; ProPhoto RGB conversions use explicit Bradford adaptation.
- **Conversion graph**: registered edges for Adobe RGB, ProPhoto RGB, and
  Rec.2020.
- **Gamut**: ProPhoto RGB support in multi-colourspace gamut mapping.
- **CSS parsing**: Adobe RGB and ProPhoto RGB direct support; Rec.2020 wired to
  the core type.
- **Tests**: coverage for Adobe RGB / ProPhoto RGB models and conversions;
  enabled Display P3 conversion tests.

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
