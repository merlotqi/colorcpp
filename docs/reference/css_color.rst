CSS Color string parsing
=========================

A focused parser turns **CSS Color Level 4**-style **strings** into ``rgba8_t``: **hex**, ``rgb()`` / ``rgba()``, ``hsl()`` / ``hsla()``.

In colorcpp
------------

* Header: ``include/colorcpp/core/css_color.hpp``
* Typical API: ``parse_css_color(std::string_view)`` → ``std::optional<rgba8_t>`` (see header for exact signatures).

Notes
-----

* **Not implemented** in the current subset: named colors, ``color()``, ``lab()`` / ``lch()`` / ``oklab()`` / ``oklch()``, relative syntax, ``hwb()``, ``device-cmyk()``. See README section *CSS color parsing* for the authoritative list.
* Parsing behavior should match the README examples; for full browser behavior always verify against a CSS implementation.

References
----------

* `CSS Color Module Level 4 <https://www.w3.org/TR/css-color-4/>`__
* `CSS Color Module Level 5 <https://www.w3.org/TR/css-color-5/>`__ (future-relative syntax; not required for the current subset)
