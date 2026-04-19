Interpolation and easing
========================

Interpolation (**lerp**) moves between colors in a chosen space. colorcpp supports **RGB**, **HSL**, **HSV**, **OkLab**, **OkLCH**, **CIELAB**, and **CIELCH**, plus cubic curve interpolation, multi-stop gradients, and easing curves that remap the interpolation parameter *t* from the unit interval ``[0, 1]``.

In colorcpp
------------

* Header: ``include/colorcpp/operations/interpolate.hpp``

Available interpolation algorithms:

  * **Basic interpolation**:
    * ``lerp()`` - Linear interpolation
    * ``lerp_hsl()`` / ``lerp_hsv()`` - Cylindrical hue interpolation with shortest-arc hue handling
    * ``lerp_oklab()`` / ``lerp_oklch()`` - Perceptual interpolation
    * ``lerp_lab()`` / ``lerp_lch()`` - CIELAB / CIELCH interpolation

  * **Advanced cubic interpolation**:
    * ``lerp_cubic_hermite()`` - Cubic Hermite interpolation in OkLab space
    * ``lerp_catmull_rom()`` - Catmull-Rom interpolation in OkLab space
    * ``lerp_monotonic_spline()`` / ``lerp_spline()`` - Shape-preserving cubic interpolation without segment overshoot

  * **Path and gradient helpers**:
    * ``lerp_path()`` - Piecewise interpolation across multiple control points
    * ``multi_lerp()`` / ``multi_lerp_hsl()`` / ``multi_lerp_oklab()`` / ``multi_lerp_oklch()`` - Multi-stop interpolation
    * ``ease()`` / ``ease_hsl()`` / ``ease_oklab()`` / ``ease_oklch()`` - Interpolation with easing curves

  * **Interpolation modifiers**:
    * ``lerp_hue_locked()`` - Hold hue fixed while interpolating lightness/chroma
    * ``lerp_chroma_preserving()`` - Preserve chroma across the transition
    * ``lerp_alpha_mode()`` - Interpolate color and alpha independently


Notes
-----

* Most advanced interpolators operate internally in **OkLab** and then convert back to the requested color type.
* ``lerp_catmull_rom()`` passes exactly through ``p1`` and ``p2`` for each segment.
* ``lerp_monotonic_spline()`` is designed for smooth transitions that avoid per-segment overshoot.
* ``lerp_path()`` repeats the endpoints as anchors for the first and last segment, which keeps boundary behavior stable.
* All interpolators work across supported color spaces through automatic conversion.

References
----------

* `CSS Color Module Level 4 — Color Interpolation <https://www.w3.org/TR/css-color-4/#interpolation>`__
* `Catmull-Rom Splines <https://en.wikipedia.org/wiki/Cubic_Hermite_spline#Catmull%E2%80%93Rom_spline>`__
* `Steffen, M. 1990 — A Simple Method for Monotonic Interpolation in One Dimension <https://ui.adsabs.harvard.edu/abs/1990A%26A...239..443S/abstract>`__
* `Oklab Perceptual Uniformity <https://bottosson.github.io/posts/oklab/>`__
* `CSS Images Module — linear color interpolation (conceptual background) <https://www.w3.org/TR/css-images-4/>`__
* `CIE color spaces (LAB) <https://www.cie.co.at/>`__
