Alpha blending and blend modes
==============================

Compositing combines a **source** and **destination** color with alpha. colorcpp performs blending in **linearized sRGB** after ``color_cast``, using separable RGB formulas aligned with common **CSS Compositing** blend modes, plus **Porter–Duff**-style alpha composition.

In colorcpp
------------

* Header: ``include/colorcpp/operations/blend.hpp``
* Main API: ``blend`` / overloads taking ``blend_mode`` (normal, multiply, screen, overlay, HSL-channel modes, etc.)

Notes
-----

* Modes such as **overlay**, **soft_light**, and **hard_light** follow **W3C**-style definitions where noted in source comments.
* **Hue / saturation / color / luminosity** modes operate by converting to a cylindrical space (HSL-style separation) for those channels.

References
----------

* Porter, T., & Duff, T. (1984). *Compositing digital images.* SIGGRAPH ’84. `DOI 10.1145/964965.801153 <https://doi.org/10.1145/964965.801153>`__
* `Compositing and Blending Level 1 <https://www.w3.org/TR/compositing-1/>`__
* `CSS Color Module Level 4 — “OK” for color spaces used elsewhere <https://www.w3.org/TR/css-color-4/>`__ (related color pipeline context)
