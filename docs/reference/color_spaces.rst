Color models (core types)
===========================

colorcpp represents colors as typed ``basic_color`` specializations with tagged channels (RGB, HSL, CIELAB, OkLab, etc.). Each model has a header under ``include/colorcpp/core/``.

In colorcpp
------------

Aggregated includes: ``include/colorcpp/core/core.hpp``.

.. list-table::
   :widths: 28 72
   :header-rows: 1

   * - Model
     - Header (``include/colorcpp/core/``)
   * - sRGB (8-bit and float)
     - ``rgb.hpp``
   * - Linear RGB (linear light, often sRGB primaries)
     - ``linear_rgb.hpp``
   * - Display P3 (DCI-P3 primaries, D65 white point)
     - ``display_p3.hpp``
   * - HSV / HSL
     - ``hsv.hpp``, ``hsl.hpp``
   * - HWB
     - ``hwb.hpp``
   * - CMYK
     - ``cmyk.hpp``
   * - CIE XYZ (D65)
     - ``xyz.hpp``
   * - CIELAB / CIELCH
     - ``cielab.hpp`` (cylindrical form defined alongside LAB)
   * - OkLab / OkLCH
     - ``oklab.hpp``
   * - Named constants
     - ``constants.hpp``

Notes
-----

* **CIELAB** and conversions that depend on it use the library’s **D65** white point unless a header comment states otherwise.
* **sRGB** stored as 8-bit or float is still **display-referred / encoded** sRGB until converted with ``color_cast`` to ``linear_rgb`` (see :doc:`conversion`).

References (specs and overviews)
---------------------------------

* `IEC 61966-2-1:1999 — Multimedia systems — Colour measurement and management — Part 2-1: Default RGB colour space sRGB <https://webstore.iec.ch/en/publication/6169>`__
* `Recommendation ITU-R BT.709 — Image parameter values for high-definition television <https://www.itu.int/rec/R-REC-BT.709>`__
* `CIE 015:2018 — Colorimetry, 4th Edition <https://www.cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`__ (XYZ, LAB context)
* `OkLab — Björn Ottosson (2020) <https://bottosson.github.io/posts/oklab/>`__
