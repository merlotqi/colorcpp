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
   * - CIE Luv
     - ``luv.hpp``
   * - CIE xyY
     - ``xyy.hpp``
   * - Jzazbz
     - ``jzazbz.hpp``
   * - OkLab / OkLCH
     - ``oklab.hpp``
   * - Rec.709 / BT.1886
      - ``rec709.hpp``
   * - Rec.2100 PQ / HLG
      - ``rec2100.hpp``
   * - DCI-P3 (theatrical)
      - ``dci_p3.hpp``
   * - scRGB (extended range)
      - ``scrgb.hpp``
   * - ACES2065-1 / ACEScg / ACEScct
      - ``aces.hpp``
   * - ICtCp
      - ``ictcp.hpp``
   * - YCbCr (BT.601 / 709 / 2020)
      - ``ycbcr.hpp``
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
   * - CIE Luv
     - Perceptual
     - D65
     - L* u* v*
     - L: [0,100] u*,v*: [-128, 128]
     - Additive mixing, displays, soft-proofing
   * - CIE xyY
     - Chromaticity
     - D65
     - x y Y
     - x,y: [0,1] Y: [0,2]
     - Chromaticity diagrams, CCT work, colorimetry
   * - Jzazbz
     - Perceptual (PQ-based)
     - D65
     - Jz az bz
     - Jz: [0,1] az,bz: [-1,1]
     - HDR, large dynamic range uniformity
   * - XYZ
     - Tristimulus
     - D65
     - X Y Z
     - [0, ~1]
     - Conversion hub space

   * - Rec.709 / BT.1886
      - Gamma 2.4 (BT.1886)
      - D65
      - R G B [A]
      - [0, 1]
      - Broadcast HDTV
   * - Rec.2100 PQ / HLG
      - ST 2084 PQ / ARIB B-67 HLG
      - D65
      - R G B [A]
      - [0, 1]
      - HDR video, broadcast UHD
   * - DCI-P3 (theatrical)
      - Gamma 2.6
      - D62 (Bradford-adapted to D65 hub)
      - R G B [A]
      - [0, 1]
      - Digital cinema projection
   * - scRGB
      - Linear 1.0 (extended)
      - D65
      - R G B [A]
      - (-inf, +inf) extended
      - Compositing headroom, rendering
   * - ACES2065-1 / ACEScg / ACEScct
      - Linear / S-2016-003 log
      - ACES D60 (Bradford-adapted)
      - R G B [A]
      - Extended (AP0/AP1 negatives allowed)
      - Academy color pipeline, VFX, grading
   * - ICtCp
      - PQ intensity + opponent chroma
      - D65
      - I Ct Cp
      - I: [0,1] Ct,Cp: [-1,1]
      - HDR perceptual coding, BT.2124
   * - YCbCr (BT.601 / 709 / 2020)
      - Matrix + quantization
      - D65
      - Y Cb Cr
      - Y: [0,1] Cb/Cr: [-0.5, 0.5]
      - SD/HD/UHD video, limited/full range 8/10-bit

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
* `CIE 015:2018 — Colorimetry, 4th Edition <https://www.cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`__ (XYZ, LAB, Luv, xyY context)
* `OkLab — Björn Ottosson (2020) <https://bottosson.github.io/posts/oklab/>`__
* `Safdar et al. (2017). Perceptually uniform color space for image signals of higher dynamic range and wider gamut <https://doi.org/10.1364/OE.25.016325>`__ (Jzazbz)
* `Recommendation ITU-R BT.2100 — Image parameter values for high dynamic range television <https://www.itu.int/rec/R-REC-BT.2100>`__ (PQ/HLG)
* `Recommendation ITU-R BT.709 — Image parameter values for high-definition television <https://www.itu.int/rec/R-REC-BT.709>`__ (Rec.709/BT.1886)
* `SMPTE ST 2065-1 — ACES2065-1 (AP0) <https://www.smpte.org/standboards/standards/ST-2065-1>`__ (ACES archive space)
* `Academy S-2016-003 — ACEScct <https://www.oscars.org/science-technology/cast/aces>`__ (ACEScct grading)
* `Recommendation ITU-R BT.601 — Studio encoding parameters of digital television <https://www.itu.int/rec/R-REC-BT.601>`__ (YCbCr SD)
* `Lu et al. (2016). A Perceptual EOTF for High Dynamic Range Content <https://doi.org/10.1371/journal.pone.0154262>`__ (ICtCp)
