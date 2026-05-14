Alpha blending and blend modes
==============================

Compositing combines a **source** and **destination** color with alpha. The current colorcpp implementation converts
inputs through ``rgbaf_t`` and applies the blend formulas there. Treat this as the current encoded ``rgbaf_t`` working
space contract, not as a verified linear-sRGB compositing claim.

Current implementation note
---------------------------

* The public implementation currently works in encoded ``rgbaf_t`` space.
* The documentation therefore describes the module as an encoded working-space implementation, even though a future
  follow-up may introduce a true linear-sRGB policy.

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

* All blending operations currently use the encoded ``rgbaf_t`` working space described above
* Non-separable modes use the W3C hue/saturation/luminosity decomposition helpers, not Oklch
* ``COLORCPP_ENABLE_SIMD=ON`` currently accelerates selected separable modes only; unsupported modes fall back to the scalar path
* Blend mode behavior follows the W3C compositing model used elsewhere in CSS tooling

References
----------

* Porter, T., & Duff, T. (1984). *Compositing digital images.* SIGGRAPH ’84. `DOI 10.1145/964965.801153 <https://doi.org/10.1145/964965.801153>`__
* `Compositing and Blending Level 1 <https://www.w3.org/TR/compositing-1/>`__
