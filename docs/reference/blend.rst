Alpha blending and blend modes
==============================

Compositing combines a **source** and **destination** color with alpha. colorcpp performs blending in **linearized sRGB** after ``color_cast``, using separable RGB formulas aligned with common **CSS Compositing** blend modes, plus **Porter–Duff**-style alpha composition.

In colorcpp
------------

* Header: ``include/colorcpp/operations/blend.hpp``
* Optional SIMD fast path: ``include/colorcpp/operations/blend/simd.hpp`` when ``COLORCPP_ENABLE_SIMD=ON``

Available blend modes:

* **Separable blend modes** (per-channel operation):
  ``normal``, ``multiply``, ``screen``, ``overlay``, ``darken``, ``lighten``,
  ``addition``, ``subtraction``, ``difference``, ``exclusion``,
  ``hard_light``, ``soft_light``, ``color_dodge``, ``color_burn``, ``divide``
* **Non-separable blend modes**:
  ``hue``, ``saturation``, ``color_blend``, ``luminosity``

Notes
-----

* All blending operations are performed in linearized sRGB space
* Non-separable modes use the W3C hue/saturation/luminosity decomposition helpers, not Oklch
* ``COLORCPP_ENABLE_SIMD=ON`` currently accelerates selected separable modes only; unsupported modes fall back to the scalar path
* Blend mode behavior follows the W3C compositing model used elsewhere in CSS tooling

References
----------

* Porter, T., & Duff, T. (1984). *Compositing digital images.* SIGGRAPH ’84. `DOI 10.1145/964965.801153 <https://doi.org/10.1145/964965.801153>`__
* `Compositing and Blending Level 1 <https://www.w3.org/TR/compositing-1/>`__
