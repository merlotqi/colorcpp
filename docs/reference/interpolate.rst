Interpolation and easing
=========================

Interpolation (**lerp**) moves between colors in a chosen space. colorcpp supports **RGB**, **HSL**, **HSV**, **OkLab**, **OkLCH**, **CIELAB**, and **CIELCH**, plus multi-stop gradients and **easing** curves that remap the interpolation parameter *t* from the unit interval [0, 1].

In colorcpp
------------

* Header: ``include/colorcpp/operations/interpolate.hpp``

Available interpolation algorithms:

  * **Basic interpolation**:
    * ``lerp()`` - Linear interpolation
    * ``lerp_hue()`` - Hue interpolation with shortest/longest path selection
    * ``lerp_preserve_chroma()`` - Chroma preserving interpolation

  * **Advanced cubic interpolation**:
    * ``cubic_interpolate()`` - Cubic Hermite spline
    * ``catmull_rom_interpolate()`` - Catmull-Rom spline (C2 continuous)

  * **Interpolation modifiers**:
    * Hue interpolation mode: ``shorter`` / ``longer`` / ``increasing`` / ``decreasing``
    * Gamut clamping options
    * Chroma preservation strategies


Notes
-----

* Hue paths support 4 different interpolation directions
* Catmull-Rom interpolation passes exactly through all control points
* Oklab space provides the most perceptually uniform interpolation results
* All interpolators work across all color spaces automatically

References
----------

* `CSS Color Module Level 4 — Color Interpolation <https://www.w3.org/TR/css-color-4/#interpolation>`__
* `Catmull-Rom Splines <https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline>`__
* `Oklab Perceptual Uniformity <https://bottosson.github.io/posts/oklab/>`__

References
----------

* `CSS Images Module — linear color interpolation (conceptual background) <https://www.w3.org/TR/css-images-4/>`__
* `OkLab <https://bottosson.github.io/posts/oklab/>`__
* `CIE color spaces (LAB) <https://www.cie.co.at/>`__
