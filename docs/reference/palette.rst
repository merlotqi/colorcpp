Palettes and color harmony
===========================

Palettes combine **interpolation** with **hue geometry**: ramps between two colors, complementary / triadic / tetradic layouts, etc.

In colorcpp
------------

* Header: ``include/colorcpp/operations/palette.hpp``
* Types: ``palette_set`` (ordered list with wrap indexing)
* Functions: ``linear_scale`` (RGB ``lerp`` ramp), harmony helpers (rotate hue in HSL space, etc.—see header for exact names)

Notes
-----

* Harmony angles (e.g. 180° complementary) are applied in **HSL hue** space after conversion; saturation/lightness come from the source color unless an API states otherwise.
* ``linear_scale`` uses RGB interpolation; for perceptual ramps prefer building stops with :doc:`interpolate` in OkLab/OkLCH and then packing into a ``palette_set``.

References
----------

* Traditional color theory references are numerous; for **digital** use, prefer perceptual spaces (:doc:`color_spaces`) and verify results with :doc:`delta_e` or visual judgment.
