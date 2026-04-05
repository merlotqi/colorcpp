Alpha blending and blend modes
==============================

Compositing combines a **source** and **destination** color with alpha. colorcpp performs blending in **linearized sRGB** after ``color_cast``, using separable RGB formulas aligned with common **CSS Compositing** blend modes, plus **Porter–Duff**-style alpha composition.

In colorcpp
------------

* Header: ``include/colorcpp/operations/blend.hpp``

Available blend modes:

  * **Separable blend modes** (per-channel operation):
    * ``normal``, ``multiply``, ``screen``, ``overlay``
    * ``darken``, ``lighten``, ``color_dodge``, ``color_burn``
    * ``hard_light``, ``soft_light``, ``difference``, ``exclusion``

  * **Non-separable blend modes**:
    * ``hue`` - Preserve destination lightness & saturation, use source hue
    * ``saturation`` - Preserve destination hue & lightness, use source saturation
    * ``color`` - Preserve destination lightness, use source hue & saturation
    * ``luminosity`` - Preserve destination hue & saturation, use source lightness

  * **Compositing operations**:
    * Full Porter-Duff compositing operators


Notes
-----

* All blending operations are performed in linearized sRGB space
* Non-separable modes use Oklch space for perceptually accurate channel separation
* Blend modes follow W3C Compositing and Blending Level 1 specification exactly

References
----------

* `Compositing and Blending Level 1 <https://www.w3.org/TR/compositing-1/>`__
* Porter, T., & Duff, T. (1984). *Compositing digital images.* SIGGRAPH '84.
* `CSS Blending Modes <https://www.w3.org/TR/compositing-1/#blending>`__

References
----------

* Porter, T., & Duff, T. (1984). *Compositing digital images.* SIGGRAPH ’84. `DOI 10.1145/964965.801153 <https://doi.org/10.1145/964965.801153>`__
* `Compositing and Blending Level 1 <https://www.w3.org/TR/compositing-1/>`__
* `CSS Color Module Level 4 — “OK” for color spaces used elsewhere <https://www.w3.org/TR/css-color-4/>`__ (related color pipeline context)
