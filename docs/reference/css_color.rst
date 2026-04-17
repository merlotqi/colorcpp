CSS Color Parsing & Formatting
==============================

CSS Color 4 parsing with selected Color 5 helpers, plus CSS string formatting for sRGB outputs.

In colorcpp
------------

* Header: ``include/colorcpp/io/css.hpp``
* Namespace: ``colorcpp::io::css``

**Parsing API**:

  * ``parse_css_color<ColorType>()`` - Generic parser for any color space
  * ``parse_css_color_rgba8()`` - Parse to 8-bit sRGB
  * ``parse_css_color_rgbaf()`` - Parse to floating point sRGB
  * ``parse_css_color_context`` - Ambient values for ``currentColor``, system colors, and theme-dependent parsing

**Formatting API**:

  * ``to_css_color_string()`` - Format ``rgba8_t`` / ``rgbaf_t`` as canonical CSS ``rgb()`` strings


✅ **Fully implemented formats**:

  * **Hex notation**:
    * Short forms: ``#rgb``, ``#rgba``
    * Long forms: ``#rrggbb``, ``#rrggbbaa``
    * Case insensitive, with or without leading hash

  * **Color functions**:
    * ``rgb()`` / ``rgba()`` - legacy and modern comma-less syntax
    * ``hsl()`` / ``hsla()`` - hue/saturation/lightness
    * ``hwb()`` - hue/whiteness/blackness
    * ``oklab()`` / ``oklch()`` - perceptual uniform space
    * ``lab()`` / ``lch()`` - CIE LAB space
    * ``color(display-p3)`` - Display P3 wide gamut
    * ``color-mix()`` - interpolation in ``srgb``, ``srgb-linear``, ``display-p3``, ``display-p3-linear``, ``lab``, ``lch``, ``oklab``, ``oklch``, and ``xyz``; ``lch`` / ``oklch`` also accept ``shorter|longer|increasing|decreasing hue``
    * ``device-cmyk()`` - CMYK device colors with optional alpha
    * ``light-dark()`` - Theme-aware color selection through context-aware parsing

  * **Named colors**:
    * Full SVG / CSS Level 4 named color table (148 colors)
    * Case insensitive lookup

  * **Missing components**:
    * ``none`` is accepted in supported functional syntaxes and mapped to the documented per-space defaults


Parser features:
  * Zero-copy string_view interface
  * Full percentage / number / angle unit parsing
  * Whitespace tolerant
  * Graceful error handling
  * No exceptions, returns optional types
  * Context-aware parsing for ``currentColor`` and CSS system colors
  * ``parse_css_color<ColorType>()`` keeps ``color(...)`` inputs in their predefined space until the final conversion step, which avoids forcing wide-gamut inputs through sRGB first


Notes
-----

* Context-sensitive colors require the overloads that take ``parse_css_color_context``
* All percentage values are properly normalized
* Angle units support: ``deg``, ``rad``, ``grad``, ``turn``
* Parser accepts both legacy comma syntax and modern space-separated syntax
* ``color-mix()`` follows CSS premultiplied-alpha mixing semantics, including reduced-alpha results when explicit percentages sum below ``100%``
* General relative color syntax such as ``rgb(from ...)`` is not implemented
* Formatter produces canonical ``rgb()`` output for sRGB colors


References
----------

* `CSS Color Module Level 4 Specification <https://www.w3.org/TR/css-color-4/>`__
* `CSS Color Module Level 5 Specification <https://www.w3.org/TR/css-color-5/>`__
* `W3C CSS Color Parsing Algorithm <https://www.w3.org/TR/css-color-4/#parsing>`__
