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

Color space properties
----------------------

.. list-table::
   :widths: 22 14 14 14 14 22
   :header-rows: 1

   * - Color Space
     - Gamma
     - White Point
     - Channels
     - Range
     - Typical Use Case
   * - sRGB
     - 2.2 piecewise
     - D65
     - R G B [A]
     - [0, 255] / [0, 1]
     - Displays, web, standard images
   * - Linear RGB
     - Linear 1.0
     - D65
     - R G B
     - [0, 1]
     - Rendering, blending, math operations
   * - Display P3
     - 2.2
     - D65
     - R G B
     - [0, 1]
     - Apple displays, wide gamut content
   * - Oklab
     - Perceptual
     - D65
     - L a b
     - L: [0,1] a,b: [-0.5, 0.5]
     - Perceptual operations, best uniformity
   * - CIELAB
     - Perceptual
     - D65
     - L* a* b*
     - L: [0,100] a*,b*: [-128, 127]
     - Industry standard colorimetry
   * - XYZ
     - Tristimulus
     - D65
     - X Y Z
     - [0, ~1]
     - Conversion hub space


Notes
-----

* All color spaces in colorcpp use standard D65 2° standard observer white point
* sRGB uses IEC 61966-2-1 piecewise gamma curve not simple 2.2 power function
* Oklab is recommended for all perceptual operations (blending, interpolation, distance)
* Integer formats are automatically normalized to [0,1] during conversion
* Cylindrical variants (OkLCH, LCH, HSL, HSV) are defined alongside their cartesian counterparts

References (specs and overviews)
---------------------------------

* `IEC 61966-2-1:1999 — Multimedia systems — Colour measurement and management — Part 2-1: Default RGB colour space sRGB <https://webstore.iec.ch/en/publication/6169>`__
* `Recommendation ITU-R BT.709 — Image parameter values for high-definition television <https://www.itu.int/rec/R-REC-BT.709>`__
* `CIE 015:2018 — Colorimetry, 4th Edition <https://www.cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`__ (XYZ, LAB context)
* `OkLab — Björn Ottosson (2020) <https://bottosson.github.io/posts/oklab/>`__
