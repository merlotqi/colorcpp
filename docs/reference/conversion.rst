Color space conversion
=======================

Conversions route colors between registered models using explicit **hub** spaces (linear sRGB, XYZ, OkLab for perceptual paths). Unsupported pairs fail at **compile time** with a ``static_assert``.

In colorcpp
------------

* Header: ``include/colorcpp/operations/conversion.hpp``
* Entry point: ``colorcpp::operations::conversion::color_cast<To>(from)``
* **Extension:** register a direct conversion with ``COLORCPP_REGISTER_CONVERSION`` / ``COLORCPP_REGISTER_CONVERSION_BIDIR`` (see ``include/colorcpp/operations/conversion/registry.hpp``), typically next to other registrations in ``include/colorcpp/operations/conversion/functions/index.hpp``. For automatic multi-hop routing, specialize ``color_traits<YourModel>::hub_type`` in ``include/colorcpp/operations/conversion/color_space_registry.hpp`` (or a header you include after ``traits.hpp``).

Notes
-----

* **sRGB ↔ linear sRGB** follows the **IEC 61966-2-1** piecewise transfer (same family as used in CSS and WCAG luminance in this library).
* Channel mapping to **unit interval** uses per-channel min/max; clamping in ``from_unit`` guards floating-point drift and does not by itself implement full **gamut mapping** (see :doc:`gamut`).
* Matrices and OkLab coefficients in code match **Björn Ottosson (2020)** for the OkLab path.

References
----------

* `W3C CSS Color Module — sRGB and transfer functions <https://www.w3.org/TR/css-color-4/#srgb>`__
* `IEC 61966-2-1 (sRGB definition) <https://webstore.iec.ch/en/publication/6169>`__
* `OkLab derivation and matrices <https://bottosson.github.io/posts/oklab/>`__
* `ICC Specification — color management architecture <https://www.color.org/index.xalter>`__ (background on PCS and transforms)
