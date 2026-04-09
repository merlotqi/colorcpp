Gamut clipping and mapping
==========================

**Gamut** operations provide functions to check if colors are within displayable gamuts and to map out-of-gamut colors back into valid ranges.

In colorcpp
------------

* Header: ``include/colorcpp/algorithms/gamut.hpp``
* Module structure:

  * ``distance.hpp`` — gamut boundary distance calculation
  * ``mapping_result.hpp`` — detailed mapping result metadata
  * ``multi_colorspace.hpp`` — multi-colorspace gamut support
  * ``preserve.hpp`` — perceptual preservation strategies
  * ``srgb.hpp`` — standard sRGB gamut implementations

Core functions
--------------

* ``is_in_srgb_gamut()`` — fast sRGB gamut check
* ``is_in_gamut(color, gamut_type)`` — check against sRGB / Display P3 / Adobe RGB
* ``gamut_clip()`` — map color into sRGB using various methods:

  * ``clip_method::clip`` — per-channel clamp (fastest)
  * ``clip_method::chroma_reduce`` — chroma reduction bisection algorithm
  * ``clip_method::css_color4`` — CSS Color Level 4 standard gamut mapping

* ``gamut_distance()`` — perceptual distance from the sRGB gamut boundary
* ``gamut_clip_with_info()`` — mapped color plus metadata
* ``gamut_clip_to_gamut(color, gamut_type)`` — simple clipping for non-sRGB target gamuts

Multi-colorspace support
-------------------------

Supported gamut types:
* ``gamut_type::srgb`` — Standard RGB
* ``gamut_type::display_p3`` — Apple Display P3 wide gamut
* ``gamut_type::adobe_rgb`` — Adobe RGB (1998)

Preservation strategies
-----------------------

Advanced mapping with perceptual priority:

* ``preserve_mode::lightness`` — Prioritize lightness accuracy
* ``preserve_mode::hue`` — Prioritize hue preservation
* ``preserve_mode::saturation`` — Prioritize saturation levels
* ``preserve_mode::perceptual`` — Minimize overall ΔE difference

Related helpers:

* ``gamut_clip_preserve_lightness()``
* ``gamut_clip_preserve_hue()``
* ``gamut_clip_preserve_saturation()``
* ``gamut_clip_perceptual()``

Mapping result information
--------------------------

* ``gamut_mapping_result`` structure returns complete mapping metadata:
  * Final mapped color
  * Perceptual distance delta
  * Chroma reduction percentage
  * Boundary distance information
  * Operation success status

Notes
-----

* Simple clipping may alter hue significantly at high chroma
* Chroma reduction algorithms preserve lightness much better
* All gamut operations work natively in Oklab perceptual space
* Display P3 gamut is approximately 25% larger than sRGB

References
----------

* `CSS Color Module Level 4 — Gamut Mapping Algorithm <https://www.w3.org/TR/css-color-4/#gamut-mapping>`__
* `Display P3 Color Space <https://www.color.org/chardata/rgb/DisplayP3.xalter>`__
* `Adobe RGB (1998) Specification <https://www.color.org/chardata/rgb/AdobeRGB1998.xalter>`__
* `Oklab Gamut Clipping <https://bottosson.github.io/posts/gamutclipping/>`__
