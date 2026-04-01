Palettes and color harmony
===========================

Palettes combine **interpolation** with **hue geometry**: ramps between two colors, complementary / triadic / tetradic layouts, etc.

In colorcpp
------------

* Header: ``include/colorcpp/operations/palette.hpp``
* Types: ``palette_set`` (ordered list with wrap indexing)
* Functions: ``linear_scale`` (RGB ``lerp`` ramp), harmony helpers (rotate hue in HSL space, etc.—see header for exact names)

Implemented Features
--------------------

**Scales:**
* ``linear_scale(start, end, count)`` - RGB linear interpolation
* ``visual_scale(start, end, count)`` - HSL interpolation (more vivid)
* ``perceptual_scale(start, end, count)`` - Oklab interpolation

**Advanced Scales:** (see :doc:`advanced_scales`)
* ``easing_scale(start, end, count, easing)`` - Gradient with easing functions
* ``spline_scale(control_points, count)`` - Catmull-Rom spline through control points
* ``multi_scale(colors, count)`` - Smooth multi-color gradient

**Harmony:**
* ``generate::complementary(base)`` - 180° rotation
* ``generate::analogous(base, angle)`` - ±angle rotation (default: 30°)
* ``generate::triadic(base)`` - 120° rotations
* ``generate::split_complementary(base)`` - 150° and 210° rotations
* ``generate::tetradic(base)`` - 60°, 180°, 240° rotations
* ``generate::square(base)`` - 90° rotations
* ``generate::monochromatic(base, count, min, max)`` - Varying lightness

**Rainbow:**
* ``rainbow(count)`` - Rainbow palette with evenly distributed hues
* ``rainbow_custom(count, saturation, lightness)`` - Custom saturation/lightness

**Temperature:**
* ``warm_palette(base, count)`` - Warm colors (reds, oranges, yellows)
* ``cool_palette(base, count)`` - Cool colors (blues, greens, purples)
* ``neutral_palette(base, count)`` - Neutral grays with subtle tint

**Material Design:**
* ``material_primary(base, count)`` - Primary color tonal variants
* ``material_secondary(base, count)`` - Secondary color (hue +60°)
* ``material_tertiary(base, count)`` - Tertiary color (hue +120°)
* ``material_neutral(base, count)`` - Neutral colors (low saturation)
* ``material_error(count)`` - Error colors (red-based)
* ``material_design(base, count)`` - Complete Material Design 3 system

Notes
-----

* Harmony angles (e.g. 180° complementary) are applied in **HSL hue** space after conversion; saturation/lightness come from the source color unless an API states otherwise.
* ``linear_scale`` uses RGB interpolation; for perceptual ramps prefer building stops with :doc:`interpolate` in OkLab/OkLCH and then packing into a ``palette_set``.
* Edge cases are handled: NaN/Inf values return safe defaults, angle=0 returns single color, count=0 returns empty palette.

TODO (Future Implementations)
-----------------------------

**Extended Harmony:**
* ``accented_analogous()`` - Analogous with accent color
* ``double_complementary()`` - Double complementary scheme
* ``pentadic()`` - Five-color harmony (72° steps)
* ``hexadic()`` - Six-color harmony (60° steps)

**Design Palettes:**
* ``flat_ui()`` - Flat UI design colors
* ``pastel()`` - Pastel color palette
* ``neon()`` - Neon/vibrant colors
* ``earth_tones()`` - Earth tone palette

**Accessibility:**
* ``accessible_palette(base, min_contrast)`` - WCAG compliant palette
* ``colorblind_safe(base)`` - Colorblind-friendly palette

**Color Theory:**
* ``itten_primary()`` - Itten primary colors
* ``itten_secondary()`` - Itten secondary colors
* ``munsell_hue(base, count)`` - Munsell color system

**Advanced:**
* ``seasonal(base, season)`` - Seasonal palettes (spring/summer/autumn/winter)
* ``extract_from_image(image, count)`` - Extract dominant colors from image

References
----------

* Traditional color theory references are numerous; for **digital** use, prefer perceptual spaces (:doc:`color_spaces`) and verify results with :doc:`delta_e` or visual judgment.
* Material Design 3: https://m3.material.io/styles/color/overview
* WCAG 2.1: https://www.w3.org/TR/WCAG21/
