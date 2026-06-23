# Wide-Color-Gamut Completion Design

**Date:** 2026-06-24
**Branch:** `feat/wide-color-gamut` (merged to `main`)
**Status:** Design approved, pending implementation plan

## Context

The `feat/wide-color-gamut` branch added three wide-gamut color spaces (Display P3, Adobe RGB, ProPhoto RGB) to the colorcpp library. However, the implementation is incomplete:

1. Rec.2020 (BT.2020) — the most widely used HDR/WCG standard — has no core color type; it only exists as CSS parsing glue code.
2. Chromatic adaptation (white point conversion) is only available implicitly — hardcoded as combined matrix coefficients inside ProPhoto RGB conversions. No standalone, reusable module.
3. ProPhoto RGB is missing from the gamut mapping `gamut_type` enum.
4. Display P3 conversion tests are commented out ("Temporarily disabled").
5. No dedicated examples for Adobe RGB, ProPhoto RGB, Rec.2020, or chromatic adaptation.

This spec defines the minimum set of additions needed to make the branch feature-complete.

## Scope: Complete the Wide-Color-Gamut Picture

**In scope:**
- Rec.2020 (BT.2020) core color type with full conversion graph registration
- Standalone chromatic adaptation module (Bradford, Von Kries) with standard white point constants
- ProPhoto RGB gamut mapping support
- Test un-blocking and coverage for all new modules
- Examples for all wide-gamut color spaces and chromatic adaptation

**Out of scope (recorded as Future Work):**
- HDR transfer functions (PQ/ST.2084, HLG)
- Tone mapping operators
- ACES color spaces (AP0/AP1)
- Rec.2100, DCI-P3 (theatrical), Rec.709 core types
- CAT02 / CAT16 chromatic adaptation
- ICC profile support
- Wide-gamut-native blend / interpolation (currently routes through sRGB)

## Module 1: Rec.2020 Color Type

### Files

| File | Purpose |
|---|---|
| `include/colorcpp/core/rec2020.hpp` | Core type definitions |
| `include/colorcpp/operations/conversion/functions/rec2020.hpp` | Gamma encode/decode + conversion functions |
| `include/colorcpp/operations/conversion/functions/index.hpp` | Graph edge registration (edit) |
| `include/colorcpp/io/css/color_function.hpp` | Wire up `color(rec2020 ...)` to return `rec2020_rgbaf_t` directly (edit) |
| `include/colorcpp/io/css/relative_color.hpp` | Wire up relative color parsing (edit) |

### Types

Follow the existing Display P3 / Adobe RGB layered pattern:

```
rec2020_rgbf_t        — gamma-encoded, 3-channel, float [0,1]
rec2020_rgbaf_t       — gamma-encoded, 4-channel, float [0,1] (alpha)
linear_rec2020_rgbf_t  — scene-linear, 3-channel, float [0,1]
linear_rec2020_rgbaf_t — scene-linear, 4-channel, float [0,1] (alpha)
```

### Transfer Function (BT.709-style piecewise)

Rec.2020 uses the BT.709/BT.2020 "camera" transfer function with Rec.2020 parameters:

- α = 1.09929682680944
- β = 0.018053968510807
- Linear segment threshold = β × 4.5 ≈ 0.081242858

```
Linear → Gamma:  v ≤ β ? v × 4.5 : α × v^0.45 − (α − 1)
Gamma → Linear:  v ≤ β × 4.5 ? v / 4.5 : ((v + α − 1) / α)^(1/0.45)
```

Implementation in `operations/conversion/functions/rec2020.hpp`, following the style of `linear_rgb.hpp` (sRGB gamma) and `adobe_rgb.hpp` (pure power).

### XYZ Conversion Matrix

Rec.2020 primaries to CIE XYZ (D65 white point):

```
XYZ = M_rec2020_to_xyz × Linear_Rec2020

Forward (linear rec2020 → XYZ D65):
  0.6369580483012914  0.14461690358620838  0.16888097516417213
  0.2627002120112671   0.6779980715188708   0.05930171646986196
  0.000000000000000    0.028072693049087428 1.0609850577107909

Inverse (XYZ D65 → linear rec2020):
   1.7166511879712679 −0.35567078377639240 −0.25336628137365992
  −0.6666843518324889  1.6164812366349388   0.015768545813911114
   0.01763985744531078 −0.04277061325780865 0.942103121235474
```

Standard BT.2020 matrices from ITU-R BT.2020-2. D65 white point matches the XYZ hub — no chromatic adaptation needed.

### Conversion Graph Registration

- **Canonical (cost=1):** `rec2020 ↔ linear_rec2020`, `linear_rec2020 ↔ xyz`
- **Shortcut (cost=5):** `rec2020 ↔ srgb` (4-hop direct, mirrors Display P3 and Adobe RGB patterns)

Standard path: Rec2020 → Linear Rec2020 → XYZ → Linear sRGB → sRGB (cost=4)
Shortcut path: Rec2020 → sRGB (cost=5, but fewer conversions → better precision)

### CSS I/O Adaptation

The generic `color()` CSS function in `color_function.hpp` returns `rgbaf_t` for all predefined spaces — keep this consistent for Rec.2020 (no return-type change). However, two internal improvements apply:

1. **`color_function.hpp`:** The existing `rec2020_to_rgbaf()` helper currently applies gamma decode + XYZ matrix inline. Refactor it to use the new `color_cast<rgbaf_t>(rec2020_rgbaf_t{...})` path, verifying round-trip fidelity is maintained.
2. **`relative_color.hpp`:** The existing `gamma_decode_rec2020()` / `gamma_encode_rec2020()` helpers remain — they are already correct and are reused by the conversion functions in `operations/conversion/functions/rec2020.hpp`.

A dedicated `parse_css_color<rec2020_rgbaf_t>()` overload is added so users can parse `color(rec2020 ...)` directly into the native type.

## Module 2: Chromatic Adaptation

### Files

| File | Purpose |
|---|---|
| `include/colorcpp/algorithms/chromatic_adaptation.hpp` | Public API: Bradford, Von Kries, white point constants |
| `include/colorcpp/operations/conversion/functions/prophoto_rgb.hpp` | Refactor: split combined matrix into pure primaries + explicit Bradford call (edit) |

### Public API

```cpp
namespace colorcpp::algorithms::chromatic_adaptation {

// Predefined white points (CIE 1931 xy, converted to XYZ tristimulus with Y=1)
inline constexpr auto WHITEPOINT_D65 = xyz_t{0.95047, 1.00000, 1.08883};
inline constexpr auto WHITEPOINT_D50 = xyz_t{0.96422, 1.00000, 0.82521};
inline constexpr auto WHITEPOINT_D55 = xyz_t{0.95682, 1.00000, 0.92149};
inline constexpr auto WHITEPOINT_A   = xyz_t{1.09850, 1.00000, 0.35585};
inline constexpr auto WHITEPOINT_C   = xyz_t{0.98074, 1.00000, 1.18232};

// Bradford chromatic adaptation (recommended for most use cases)
template <typename ColorT>
ColorT bradford_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white);

// Von Kries chromatic adaptation (simpler, diagonal scaling in LMS)
template <typename ColorT>
ColorT von_kries_adapt(const ColorT& color, const core::xyz_t& src_white, const core::xyz_t& dst_white);

} // namespace colorcpp::algorithms::chromatic_adaptation
```

### Bradford Matrix (Constant)

The Bradford LMS cone response matrix and its inverse are standard constants:

```
M_BFD (XYZ → LMS):
   0.8951   0.2664  −0.1614
  −0.7502   1.7135   0.0367
   0.0389  −0.0685   1.0296

M_BFD_inv (LMS → XYZ):
   0.9869929  −0.1470543   0.1599627
   0.4323053   0.5183603   0.0492912
  −0.0085287   0.0400428   0.9684867
```

### ProPhoto RGB Refactoring

Current state: `linear_prophoto ↔ xyz` matrices in `prophoto_rgb.hpp` are combined (primaries + Bradford D50→D65 baked in).

New state:
1. Replace combined matrices with pure ProPhoto primaries → XYZ(D50) matrices
2. In the conversion function, apply `bradford_adapt(xyz_d50, WHITEPOINT_D50, WHITEPOINT_D65)` or the reverse
3. Conversion graph edges unchanged; only internal implementation changes

This makes the ProPhoto conversion transparent about what it's doing and reusable as a pattern for other non-D65 color spaces.

### What Is NOT Implemented

- CAT02 (CIE 2002 chromatic adaptation transform)
- CAT16 (CIE 2016 chromatic adaptation transform)

## Module 3: ProPhoto RGB Gamut Mapping

### Files

| File | Purpose |
|---|---|
| `include/colorcpp/algorithms/gamut/multi_colorspace.hpp` | Add `prophoto_rgb` to enum + implementation (edit) |

### Changes

1. **Enum addition:** Add `prophoto_rgb` to `gamut_type` enum
2. **Helper function:** Add `linrgb_in_prophoto_gamut()` following the same pattern as `linrgb_in_displayp3_gamut()` and `linrgb_in_adobergb_gamut()`
3. **Switch case:** Add `case gamut_type::prophoto_rgb:` to `linrgb_in_gamut()` switch
4. **Convenience function:** Add `is_in_prophoto_gamut()` following the existing `is_in_displayp3_gamut()` / `is_in_adobergb_gamut()` pattern

The gamma function used for gamut checking reuses the ProPhoto gamma (1.8 with linear segment) already defined in `operations/conversion/functions/prophoto_rgb.hpp`. Extract it to `gamut/details.hpp` alongside the existing sRGB/Display P3/Adobe RGB gamma helpers if needed.

## Tests

### test_display_p3.cpp — Uncomment Conversion Tests

Uncomment and enable the 7 blocked tests (lines 243-304):
- `BlackConversion`, `WhiteConversion`, `AlphaPreserved`
- `FloatRoundTrip`, `AlphaRoundTrip`
- `LinearConversion`, `LinearRoundTrip`

Fix any failures (the conversion code exists and is registered; the tests were likely commented out prematurely).

### test_rec2020.cpp — New File

Following the pattern of `test_adobe_rgb.cpp` and `test_prophoto_rgb.cpp`:

- **Construction:** default, parameterized, boundaries, out-of-range
- **Conversion:** sRGB ↔ Rec2020 round-trip, alpha preservation, gamma direction (gamma values < linear for dark colors), linear round-trip
- **Cross-space:** Display P3 ↔ Rec2020, ProPhoto ↔ Rec2020, OKLab ↔ Rec2020, XYZ ↔ Rec2020
- **CSS parsing:** `color(rec2020 r g b)` round-trip, alpha, none values
- **Type traits:** copy/move/equality

### test_chromatic_adaptation.cpp — New File

- **Bradford:** D65→D50→D65 round-trip ≈ identity, D65→D50 known reference values (Bruce Lindbloom dataset), same white point = identity, symmetric non-negative output
- **Von Kries:** same structure, coarser tolerance
- **White point constants:** verify XYZ values against CIE standard
- **Integration:** validate ProPhoto RGB round-trip still works with refactored conversion

### test_gamut.cpp — Edit

Add ProPhoto RGB cases to existing test suites:
- `MultiColorspaceGamutTest`: `is_in_gamut` for `gamut_type::prophoto_rgb`
- `is_in_prophoto_gamut` convenience function
- `gamut_clip_to_gamut` with `gamut_type::prophoto_rgb`

## Examples

### adobe_rgb_example.cpp — New

- Construct Adobe RGB colors
- Convert sRGB → Adobe RGB → sRGB (show gamut expansion/contraction)
- Demonstrate gamma encoding: show raw linear values
- Cross-space: Adobe RGB → Display P3 → Rec.2020 → OKLab
- CSS: `color(a98-rgb ...)` parsing and formatting

### prophoto_rgb_example.cpp — New

- Construct ProPhoto RGB colors
- Convert sRGB → ProPhoto → sRGB (show out-of-gamut clipping)
- Demonstrate D50 white point / chromatic adaptation
- Gamut check: which sRGB colors are in ProPhoto gamut
- CSS: `color(prophoto-rgb ...)` parsing and formatting

### rec2020_example.cpp — New

- Construct Rec.2020 colors
- Gamut comparison: sRGB ⊂ Display P3 ⊂ Adobe RGB ⊂ Rec.2020 ⊂ ProPhoto
- Show which sRGB colors are still inside Rec.2020
- CSS: `color(rec2020 ...)` parsing

### chromatic_adaptation_example.cpp — New

- Demonstrate D65 → D50 adaptation (daylight to warm)
- Demonstrate D65 → A (tungsten) adaptation
- Show color shifts with ANSI swatches
- Round-trip verification: D65 → D50 → D65

## Future Work

Recorded here for traceability; NOT implemented in this spec.

1. **HDR Pipeline:** PQ (ST.2084) and HLG transfer functions, Tone Mapping operators, Rec.2100 color space
2. **Additional Color Spaces:** ACES (AP0/AP1), DCI-P3 (theatrical, 2.6 gamma), Rec.709
3. **CAT02 / CAT16:** Modern chromatic adaptation transforms
4. **Wide-gamut-native operations:** Blend and interpolation directly in Display P3 / Adobe RGB / ProPhoto RGB / Rec.2020 spaces without sRGB intermediate
5. **ICC Profile support:** Parse and apply ICC profiles

## File Change Summary

| # | File | Action |
|---|---|---|
| 1 | `include/colorcpp/core/rec2020.hpp` | New |
| 2 | `include/colorcpp/core/core.hpp` | Edit — include rec2020.hpp |
| 3 | `include/colorcpp/operations/conversion/functions/rec2020.hpp` | New |
| 4 | `include/colorcpp/operations/conversion/functions/index.hpp` | Edit — register Rec.2020 edges |
| 5 | `include/colorcpp/operations/conversion/functions/prophoto_rgb.hpp` | Edit — split combined matrices |
| 6 | `include/colorcpp/operations/conversion.hpp` | Edit — include rec2020 core type |
| 7 | `include/colorcpp/algorithms/chromatic_adaptation.hpp` | New |
| 8 | `include/colorcpp/algorithms/algorithms.hpp` | Edit — include chromatic_adaptation |
| 9 | `include/colorcpp/algorithms/gamut/multi_colorspace.hpp` | Edit — add prophoto_rgb |
| 10 | `include/colorcpp/io/css/color_function.hpp` | Edit — refactor rec2020 helper to use core type |
| 11 | `include/colorcpp/io/css/relative_color.hpp` | Edit — add parse_css_color<rec2020_rgbaf_t> overload |
| 12 | `tests/core/test_display_p3.cpp` | Edit — uncomment 7 tests |
| 13 | `tests/core/test_rec2020.cpp` | New |
| 14 | `tests/algorithms/test_chromatic_adaptation.cpp` | New |
| 15 | `tests/algorithms/test_gamut.cpp` | Edit — ProPhoto cases |
| 16 | `examples/adobe_rgb_example.cpp` | New |
| 17 | `examples/prophoto_rgb_example.cpp` | New |
| 18 | `examples/rec2020_example.cpp` | New |
| 19 | `examples/chromatic_adaptation_example.cpp` | New |

**Total: 19 files** (8 new + 11 edits)
