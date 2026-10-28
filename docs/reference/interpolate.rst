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

Semantic grouping
-----------------

Working space families
^^^^^^^^^^^^^^^^^^^^^^

* **RGB-style interpolation** — ``lerp()``
* **Cylindrical hue-aware interpolation** — ``lerp_hsl()``, ``lerp_hsv()``, ``lerp_oklch()``, ``lerp_lch()``
* **Perceptual cartesian interpolation** — ``lerp_oklab()``, ``lerp_lab()``
* **Curve/path helpers** — cubic, Catmull-Rom, monotonic spline, and path helpers layered on the same space choices

Alpha handling
^^^^^^^^^^^^^^

Alpha behavior is helper-dependent: ``lerp()`` / ``lerp_hsl()`` / ``lerp_hsv()`` interpolate alpha in the same
working-space pass, the perceptual and cylindrical helpers take alpha from the endpoints in unit space and attach it
to the result type's own alpha channel, cubic and path helpers spline alpha as part of their own contracts, and
``lerp_alpha_mode()`` exposes explicit alpha strategies.

gamut strategy
^^^^^^^^^^^^^^

General interpolation helpers do not guarantee automatic display-gamut preservation. ``lerp_chroma_preserving()`` is
the notable exception: its default ``chroma_mode::preserve_safe`` clamps chroma against the measured sRGB gamut
boundary (binary search over unclamped linear RGB).

When the deduced result type is a wide-space color (``oklab_t``, ``oklch_t``, ``cielab_t``, ``cielch_t``), the
working-space midpoint is kept exactly in that space — an out-of-sRGB midpoint is **not** clamped. RGB result types
still clamp through the normal conversion path.


Notes
-----

* Most advanced interpolators operate internally in **OkLab** and then convert back to the requested color type.
* ``lerp_catmull_rom()`` passes exactly through ``p1`` and ``p2`` for each segment in every ``cr_mode``; the
  centripetal and chordal modes build joint knots from the OkLab-space distances between control points
  (non-uniform Catmull-Rom). ``lerp_path()``'s default ``catmull_mode`` is ``cr_mode::centripetal``.
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
