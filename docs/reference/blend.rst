Alpha blending and blend modes
==============================

Compositing combines a **source** and a **destination** color with alpha. colorcpp evaluates the
W3C Porter-Duff general formula with a selectable working space (encoded sRGB or linear sRGB).

Compositing contract
--------------------

* Channels are **straight (un-premultiplied) alpha** at the API boundary.
* The Porter-Duff steps are computed in **premultiplied form** —
  ``Cs·αs·(1-αd) + Cd·αd·(1-αs) + B(Cb,Cs)·αs·αd`` — and un-premultiplied again
  by dividing with the output alpha ``αo = αs + αd·(1-αs)``; this is the W3C
  Compositing and Blending Level 1 general formula.
* Results are clamped to ``[0, 1]``.
* ``blend_space::srgb`` (default) applies the formulas to gamma-encoded sRGB
  channels — W3C/CSS parity.
* ``blend_space::linear`` linearizes both inputs first, applies the same
  formulas to linear-sRGB channels, and re-encodes the result to sRGB gamma
  ("linear-light" compositing, as in Photoshop's linear-light blending).
* Non-separable modes (``hue``, ``saturation``, ``color_blend``, ``luminosity``)
  are defined by W3C on sRGB HSL only and always use the sRGB path; the
  ``blend_space`` argument does not affect them.

In colorcpp
------------

* Header: ``include/colorcpp/operations/blend.hpp``
* API: ``blend(dst, src, mode = normal, opacity = 1, space = blend_space::srgb)``
* Optional SIMD fast path: ``include/colorcpp/operations/blend/simd.hpp`` when ``COLORCPP_ENABLE_SIMD=ON``
  (selected separable modes; operates on the working-space channels, so it applies to both spaces)

Available blend modes:

* **Separable blend modes** (per-channel operation):
  ``normal``, ``multiply``, ``screen``, ``overlay``, ``darken``, ``lighten``,
  ``addition``, ``subtraction``, ``difference``, ``exclusion``,
  ``hard_light``, ``soft_light``, ``color_dodge``, ``color_burn``, ``divide``
* **Non-separable blend modes**:
  ``hue``, ``saturation``, ``color_blend``, ``luminosity``

Notes
-----

* Non-separable modes use the W3C hue/saturation/luminosity decomposition helpers, not Oklch
* ``COLORCPP_ENABLE_SIMD=ON`` currently accelerates selected separable modes only; unsupported modes fall back to the scalar path
* Blend mode behavior follows the W3C compositing model used elsewhere in CSS tooling

References
----------

* Porter, T., & Duff, T. (1984). *Compositing digital images.* SIGGRAPH ’84. `DOI 10.1145/964965.801153 <https://doi.org/10.1145/964965.801153>`__
* `Compositing and Blending Level 1 <https://www.w3.org/TR/compositing-1/>`__
