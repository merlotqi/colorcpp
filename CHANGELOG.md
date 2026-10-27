# Changelog

All notable changes to `colorcpp` are documented here.

## [Unreleased]

### Fixed

- **Stream parsing defaults (breaking fix)**: a missing trailing channel
  defaulted to the channel's max for every model — `parse<cielab_t>("lab(50,
  10)")` produced `b* = 128`, `rgb8` input `"10 20"` produced `b = 255`. The
  default fill now applies only to alpha models (`model_has_alpha_v`), where an
  omitted trailing alpha becomes opaque; a missing non-alpha channel sets
  `failbit`.
- **Stream parsing range (breaking fix)**: `operator>>` validates each channel
  against the model's `[min, max]` at read time and sets `failbit` on
  out-of-range text instead of throwing `std::out_of_range` from the color
  constructor (compatible with `-fno-exceptions`). Integral channels no longer
  wrap: `rgb8` input `"300 0 0"` used to parse as `r = 44`, now it fails.
- **Stream operators found by ADL (breaking fix)**: the 13 `operator<<` /
  `operator>>` pairs moved from the global namespace into `colorcpp::core`.
  Global overloads are invisible to ADL and were hidden as soon as a user
  namespace declared any `operator<<` of its own; streaming now works from any
  namespace.
- **Structured bindings (breaking fix)**: `auto [r, g, b, a] = rgbaf_t{…}` — and
  every other derived family — now decomposes its channels. Previously only
  `basic_color<Model>` had the `std::tuple_size` / `std::tuple_element`
  specializations and bindings fell back to member decomposition of `data`.
  The new `COLORCPP_ENABLE_STRUCTURED_BINDINGS(Family)` macro (invoked for the
  13 family classes) derives the count and element types from `model_traits`.
  The four `std::get` overloads on color types are removed — adding function
  overloads to `namespace std` is undefined behavior. Use structured bindings
  or `colorcpp::core::get`.
- **Hex output**: `std::hex` channel bytes now round to nearest (`0.5 → 128`,
  matching `from_unit`) instead of truncating, and hex output is enabled only
  for RGB-family models (Oklab / Lab / XYZ / … print their text form even with
  `std::hex` set).
- **Constants layering**: `core/constants.hpp` no longer includes
  `io/literals.hpp` (a reverse `core → io` dependency). The 143 named colors
  are built by a core-local `detail::rgb8(0xRRGGBB)` helper with identical
  values and `rgba8_t` type.

- **Alpha preservation across graph routes (breaking fix)**: `color_cast` now
  restores the source alpha on the result whenever both endpoints have alpha.
  Minimal-cost routes cross 3-channel transit nodes (XYZ / OkLab / CIELAB) that
  cannot carry alpha, so conversions such as `rgbaf → rec2020_rgbaf` previously
  arrived with `a = 1.0`. Conversion functions copy alpha verbatim; the restore
  at the public `color_cast` boundary is exact for every route. Alpha models
  are identified by the shared `traits::alpha_tag` in `channels_type`
  (CMYK's K channel is not alpha).

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
- **Hex literal digit counts (breaking fix)**: `_rgb` / `_rgba` / `_argb` are
  template-only — the `operator""_*(unsigned long long)` overloads that
  accepted any digit count are removed, so a wrong count is a compile error
  instead of a misdecoded color. `0xFF0000_rgba` (6 digits where 8 are
  required) used to store `{0, 255, 0, 0}` — fully transparent green;
  `0xFFF_rgb` stored `{0, 15, 255}`; `0x1000000_rgb` threw `std::out_of_range`
  only at runtime; a decimal spelling such as `16711680_rgb` was read as hex
  and accepted. The digit-count and 24/32-bit checks now live solely in the
  template `static_assert`s; the parsing helpers were reordered so
  `is_hex_digit` is visible at the template definition point (two-phase
  lookup — previously masked on MSVC without `/permissive-`).
  Negative-compilation tests in `tests/compile_fail/` pin the behavior
  (`ctest -R CompileFail`); `RuntimeOverflowThrows` is retired with the ULL
  overloads.

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

- **Core traits and structured bindings**: `traits::model_channel_count_v`
  (channel count derived from `channels_type`), `traits::rgb_model_tag` /
  `traits::is_rgb_model_v` (RGB-family marker enabling `#RRGGBB` hex output),
  and `COLORCPP_ENABLE_STRUCTURED_BINDINGS(Family)` for opting a color family
  class into tuple-like structured bindings.
- **Shared `traits::alpha_tag`**: the single tag every straight-alpha channel
  uses (family-local `a_tag` names alias it), so generic code locates alpha by
  tag — `traits::channel_index_v<Model, alpha_tag>` and
  `core::get<alpha_tag>(c)` work on any alpha model.
- **Value factories `clamped()` / `make_checked()`**: opt-in boundary policy for
  value-type colors. `clamped<rgb8_t>(300, -5, 200.7f)` clamps into each
  channel's nominal range (rounding to nearest for integral channels);
  `make_checked<rgb8_t>(300, 0, 0)` validates before any narrowing conversion
  and throws `std::out_of_range`. `make_checked` is the exception-based opt-in
  for callers that can use exceptions; `clamped` is always defined.
- **Family authoring macros**: `COLORCPP_CHANNEL_ACCESSOR(Tag, Name)` declares
  an enable_if-guarded accessor pair forwarding to `basic_color::get<Tag>`, and
  `COLORCPP_STREAM_OPERATORS(Family)` declares the stream operators forwarding
  to the `basic_color` base. All 13 model headers use them (~640 lines of
  duplicated boilerplate removed); no behavior change.

### Changed

- **Colors are plain value types (breaking)**: construction and `set<Tag>` no
  longer validate against nominal channel ranges and never throw; values are
  stored verbatim (integral channels narrow with a `static_cast` — `rgb8_t(300,
  0, 0)` stores `r = 44`). Boundary policy lives at the edges: the stream parser
  pre-validates and sets `failbit` (all-or-nothing, never throws), and
  `clamped()` / `make_checked()` are the opt-in factories. Nominal ranges
  (`Channel::min` / `Channel::max`) now drive normalization, stream I/O policy
  and the factories — not construction or `set`.
- **OkLab / OkLCH channel ranges relaxed (breaking)**: `oklab::a` and `okb`
  widen from ±0.5 to ±1, `oklch::c` from [0, 0.4] to [0, 1]. The nominal ranges
  fed conversion-side clamping, cutting wide-gamut intermediates and any chroma
  above the relative-syntax 100% references (CSS defines no upper chroma bound).
  **Linear RGB range relaxation is deliberately deferred**: linear channels are
  unit-normalized through `to_unit` / `from_unit`, so widening [0, 1] would
  shift every conversion through them (black would map to linear −1); it is
  gated on the conversion module's unclamped-intermediate-representation
  redesign.
- **`model_traits::channel_size` removed (breaking)**: the channel count is
  derived from `channels_type` via `traits::model_channel_count_v` —
  `channels_type` is now the single source of truth. The 35 handwritten copies
  had no cross-check against the tuple and could silently disagree (out-of-
  bounds `get_index`, wrong parse width). Custom models delete the member and
  keep `channels_type` / `prefix` as before.
- **`colorcpp::get` moved to `colorcpp::core::get` (breaking)**: the free
  `get<Tag>` / `get<I>` helpers now live in `colorcpp::core` so ADL finds them
  alongside the stream operators (required for structured bindings). Unqualified
  `get<…>(c)` call sites are unaffected; explicit `colorcpp::get<…>` resolves
  through `core.hpp`'s re-export but not when including `color_base.hpp` or a
  model header directly — use `colorcpp::core::get` there. The free `get`
  overloads are now forwarding-reference-based (lvalue / const / rvalue).
- **`traits::default_range` and `traits::channel_traits` removed**: neither was
  referenced anywhere.
- **`model_traits::has_alpha` opt-in members removed (breaking)**: alpha is
  derived from `traits::alpha_tag` presence in `channels_type`
  (`traits::model_has_alpha_v`), so every alpha model is detected uniformly
  and a model can no longer disagree with its own channel tuple. The 14 opt-in
  members are deleted; OkLab's green-red `a` axis and CMYK's K use their own
  tags and stay non-alpha. `get_src_alpha` and `color_cast`'s alpha restore
  now look alpha up by tag index instead of assuming "channel 3 of 4+".
- **`basic_channel` default denominator (breaking)**: the default `Den` changed
  from `Max` to `1`, so `basic_channel<Tag, T, Min, Max>` now declares the
  range `[Min, Max]` instead of `[Min/Max, 1]`. Existing call sites were
  unaffected (`[0, 1]` channels keep their range; non-unit ranges already
  spelled `Den` explicitly). Integral `T` requires `Den = 1` with `Min >= 0`
  (a `static_assert` — integral division by a denominator would truncate).
- **`palette_set` API tightened (breaking)**: `operator[]` on an empty set
  throws `std::out_of_range` instead of fabricating a `Color{}`; the implicit
  `operator std::vector` is now `explicit` (it silently copied);
  `size_t` spelled `std::size_t`. Wrap indexing on non-empty sets is unchanged
  and remains the documented behavior of `operator[]`.
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
- **Decimal-packed literal suffixes removed (breaking)**: `_hsl`, `_hsla`,
  `_hsv`, `_hsva`, `_hwb`, `_hwba`, `_cmyk`, `_oklab`, `_oklch` and their five
  headers (`io/literals/{hsl,hsv,hwb,cmyk,oklab}.hpp`, plus the internal
  `parse_dec_template` helper) are deleted. The packed decimal spelling
  collided with C++ octal lexing — a token starting with `0` (the common
  zero-padded spellings `050'…`, `000'…`) failed to compile whenever it
  contained an 8 or a 9, before the operator ever ran — and the `'` groups
  were display-only (`parse_dec_template` stripped them before counting
  digits). Use the constructors instead: `hsl_float_t{120.0f, 0.5f, 0.75f}`,
  `cmyk8_t{50, 30, 0, 20}`, `oklab_t{0.5f, 0.0f, 0.0f}` — with
  `clamped<T>(…)` / `make_checked<T>(…)` for boundary control (see
  `docs/reference/literals.rst`). `_rgb` / `_rgba` / `_argb` / `_hex` are
  unaffected; `io/literals.hpp` now pulls in the RGB header only.

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
