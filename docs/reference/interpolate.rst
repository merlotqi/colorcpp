Interpolation and easing
=========================

Interpolation (**lerp**) moves between colors in a chosen space. colorcpp supports **RGB**, **HSL**, **HSV**, **OkLab**, **OkLCH**, **CIELAB**, and **CIELCH**, plus multi-stop gradients and **easing** curves that remap the interpolation parameter *t* from the unit interval [0, 1].

In colorcpp
------------

* Header: ``include/colorcpp/operations/interpolate.hpp``
* Typical entry points: ``lerp``, ``lerp_hsl``, ``lerp_oklab``, ``lerp_oklch``, ``gradient``, ``ease`` / ``ease_*`` variants.

Notes
-----

* **Hue** paths use **shortest arc** in degree space (see ``lerp_angle_deg`` in the header).
* **RGB lerp** is light-linear only after conversion; the API converts via ``color_cast`` as documented per overload.
* **OkLab / CIELAB** lerps are often **more perceptually even** than RGB for many palettes but are not a substitute for spectral mixing.

References
----------

* `CSS Images Module — linear color interpolation (conceptual background) <https://www.w3.org/TR/css-images-4/>`__
* `OkLab <https://bottosson.github.io/posts/oklab/>`__
* `CIE color spaces (LAB) <https://www.cie.co.at/>`__
