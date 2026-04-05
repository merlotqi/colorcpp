CSS Color Parsing & Formatting
==============================

Full CSS Color Level 4 compliant parser and formatter with support for all modern color space formats.

In colorcpp
------------

* Header: ``include/colorcpp/io/css.hpp``
* Namespace: ``colorcpp::io::css``

**Parsing API**:

  * ``parse_css_color<ColorType>()`` - Generic parser for any color space
  * ``parse_css_color_rgba8()`` - Parse to 8-bit sRGB
  * ``parse_css_color_rgbaf()`` - Parse to floating point sRGB

**Formatting API**:

  * ``format_css_color()`` - Format color to CSS string
  * ``format_css_color_hex()`` - Format as hexadecimal notation
  * ``format_css_color_function()`` - Format as css function notation


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
    * ``device-cmyk()`` - CMYK device colors

  * **Named colors**:
    * Full SVG / CSS Level 4 named color table (148 colors)
    * Case insensitive lookup


Parser features:
  * Zero-copy string_view interface
  * Full percentage / number / angle unit parsing
  * Whitespace tolerant
  * Graceful error handling
  * No exceptions, returns optional types


Notes
-----

* Parser is fully CSS Color Level 4 specification compliant
* All percentage values are properly normalized
* Angle units support: ``deg``, ``rad``, ``grad``, ``turn``
* Parser accepts both legacy comma syntax and modern space-separated syntax
* Formatter produces canonical CSS output format


References
----------

* `CSS Color Module Level 4 Specification <https://www.w3.org/TR/css-color-4/>`__
* `CSS Color Module Level 5 Specification <https://www.w3.org/TR/css-color-5/>`__
* `W3C CSS Color Parsing Algorithm <https://www.w3.org/TR/css-color-4/#parsing>`__
