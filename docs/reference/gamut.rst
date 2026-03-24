Gamut clipping and mapping
===========================

**sRGB gamut** means all linear sRGB channels lie in a valid display range (typically the closed interval from 0 to 1). Out-of-gamut colors can arise from **OkLab / OkLCH** or other wide-gamut intermediates.

In colorcpp
------------

* Header: ``include/colorcpp/operations/gamut.hpp``
* ``is_in_gamut``, ``gamut_clip`` with ``clip_method``:

  * ``clip`` — per-channel clamp in sRGB (fast).
  * ``chroma_reduce`` — reduce chroma in OKLCh-like space until in gamut (bisection).
  * ``css_color4`` — chroma reduction guided by a **ΔE\_OK**-style JND step (see source comments).

Notes
-----

* **Clip** may change hue strongly at high chroma; **chroma_reduce** preserves lightness better in many cases.
* Internal OkLab ↔ linear RGB coefficients match :doc:`conversion` (Ottosson matrices).

References
----------

* `CSS Color Module Level 4 — gamut mapping <https://www.w3.org/TR/css-color-4/#gamut-mapping>`__
* `OkLab <https://bottosson.github.io/posts/oklab/>`__
* `ΔE\_OK (OKLab distance) <https://bottosson.github.io/posts/gamutclipping/>`__ (background for CSS-style clipping discourse)
