
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
Gamut clipping and mapping

**Gamut** operations provide functions to check if colors are within displayable gamuts and to map out-of-gamut colors back into valid ranges.

In colorcpp
------------

* Header: ``include/colorcpp/algorithms/gamut.hpp``
* Module structure:

  * ``gamut_main.hpp`` — core sRGB gamut functions
  * ``distance.hpp`` — gamut distance calculation
  * ``mapping_result.hpp`` — detailed mapping result information
  * ``multi_colorspace.hpp`` — multi-colorspace support (sRGB, Display P3, Adobe RGB)
  * ``preserve.hpp`` — perceptual preservation strategies

Core functions
--------------

* ``is_in_srgb_gamut`` — check if color is in sRGB gamut
* ``gamut_clip`` — map color to sRGB gamut using various methods

  * ``clip_method::clip`` — per-channel clamp in sRGB (fast).
  * ``clip_method::chroma_reduce`` — reduce chroma in OKLCh-like space until in gamut (bisection).
  * ``clip_method::css_color4`` — chroma reduction guided by a **ΔE\_OK**-style JND step.

Multi-colorspace support
-------------------------

* ``gamut_type`` enum: ``srgb``, ``display_p3``, ``adobe_rgb``
* ``is_in_gamut`` — check gamut for specified color space
* ``is_in_displayp3_gamut`` — check Display P3 gamut
* ``is_in_adobergb_gamut`` — check Adobe RGB gamut
* ``gamut_clip_to_gamut`` — clip to specified gamut

Gamut distance
--------------

* ``gamut_distance`` — calculate distance to gamut boundary (ΔE_OK)
* ``is_near_gamut_boundary`` — check if color is near boundary

Preservation strategies
-----------------------

* ``preserve_mode`` enum: ``lightness``, ``hue``, ``saturation``, ``perceptual``
* ``gamut_clip_preserve`` — map with specified preservation mode
* ``gamut_clip_preserve_lightness`` — preserve lightness
* ``gamut_clip_preserve_hue`` — preserve hue
* ``gamut_clip_preserve_saturation`` — preserve saturation
* ``gamut_clip_perceptual`` — minimize perceptual difference

Mapping result information
--------------------------

* ``gamut_mapping_result`` structure with:

  * ``mapped_color`` — the mapped color
  * ``delta_e`` — perceptual color difference
  * ``original_chroma`` — original chroma
  * ``mapped_chroma`` — chroma after mapping
  * ``was_in_gamut`` — whether original was in gamut
  * ``is_valid()`` — check if mapping succeeded
  * ``chroma_reduction_percent()`` — percentage of chroma reduction

* ``gamut_clip_with_info`` — get detailed mapping result

Notes
-----

* **Clip** may change hue strongly at high chroma; **chroma_reduce** preserves lightness better in many cases.
* Display P3 and Adobe RGB have wider gamuts than sRGB, allowing more saturated colors.
* Preservation strategies offer different trade-offs between maintaining color properties.
* Internal OkLab ↔ linear RGB coefficients match :doc:`conversion` (Ottosson matrices).

References
----------

* `CSS Color Module Level 4 — gamut mapping <https://www.w3.org/TR/css-color-4/#gamut-mapping>`__
* `OkLab <https://bottosson.github.io/posts/oklab/>`__
* `ΔE\_OK (OKLab distance) <https://bottosson.github.io/posts/gamutclipping/>`__ (background for CSS-style clipping discourse)
* `Display P3 <https://www.color.org/chardata/rgb/DisplayP3.xalter>`__
* `Adobe RGB (1998) <https://www.color.org/chardata/rgb/AdobeRGB1998.xalter>`__
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
