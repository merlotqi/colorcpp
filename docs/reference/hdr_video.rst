HDR Video & Professional Colour Spaces
=======================================

colorcpp adds full support for broadcast HDR, digital cinema, and Academy colour
encoding workflows on top of its existing SDR colour spaces.

.. toctree::
   :maxdepth: 1

Overview
--------

The following colour spaces and signal encodings are new in the ``feature/hdr-video``
branch:

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Family
     - Types / Details
   * - BT.2100 PQ / HLG
     - ``rec2100_pq``, ``rec2100_pqa``, ``rec2100_hlg``, ``rec2100_hlga`` — BT.2020 primaries, D65 white, ST 2084 PQ and ARIB B-67 HLG transfer functions
   * - Rec.709 / BT.1886
     - ``rec709``, ``rec709a`` — sRGB primaries, D65 white, pure gamma 2.4 (BT.1886 display EOTF)
   * - DCI-P3 (theatrical)
     - ``dci_p3``, ``dci_p3a`` — P3 primaries, D62 white (Bradford-adapted to D65 hub), gamma 2.6
   * - scRGB (extended range)
     - ``scrgb``, ``scrgba`` — linear sRGB primaries, D65 white, extended range (negatives / >1 pass through)
   * - ACES
     - ``aces2065``/``aces2065a`` (AP0 linear), ``acescg``/``acescga`` (AP1 linear), ``acescct``/``acesccta`` (S-2016-003 log grading); Bradford-adapted ACES D60 → D65 hub
   * - ICtCp
     - ``ictcp_t`` — BT.2100 perceptual encoding: PQ intensity + opponent chroma (Ct/Cp)
   * - YCbCr
     - ``ycbcr601_t`` (BT.601, SD), ``ycbcr709_t`` (BT.709, HD), ``ycbcr2020_t`` (BT.2020, UHD); ``ycbcr::quantize()`` / ``dequantize()`` with explicit range (full/limited) and bit-depth (8/10-bit)

BT.2100 PQ / HLG
----------------

Header: ``include/colorcpp/core/rec2100.hpp``

BT.2100 defines two signal encodings of the same BT.2020 primaries:

* **PQ (Perceptual Quantizer, SMPTE ST 2084)** — absolute luminance encoding. Under
  colorcpp's scale (relative Y = 1 maps to the 100 cd/m² reference white) diffuse white
  encodes to PQ(100) = 0.5080784. The PQ encode/decode is shared with the ICtCp path so
  both encodings can never drift apart.
* **HLG (Hybrid Log-Gamma, ARIB B-67)** — scene-referred OETF mapping scene linear
  light [0, 1] to signal [0, 1]. The display-dependent OOTF (BT.2100 §7.2) is deliberately
  out of scope; apply it downstream if display-referred HLG is required.

Channels are signal code values in [0, 1] (alpha rides along). The linear counterpart is
``linear_rec2020_rgbf_t`` — same primaries, no transfer function.

Conversion functions (in ``operations/conversion/functions/rec2100.hpp``):

* ``linear_rec2020_to_rec2100_pq`` / ``rec2100_pq_to_linear_rec2020``
* ``linear_rec2020_to_rec2100_hlg`` / ``rec2100_hlg_to_linear_rec2020``

ACES
----

Header: ``include/colorcpp/core/aces.hpp``

* **ACES2065-1 (AP0, linear)** — archival/exchange reference space. Extremely wide gamut
  (blue primary has negative *y*), so extended range.
* **ACEScg (AP1, linear)** — rendering space. Extended range (out-of-gamut colours
  representable).
* **ACEScct (AP1, S-2016-003 quasi-log)** — grading signal encoding (toe + log2 branch);
  clamps at the type boundary like other display encodings.

All three are Bradford-adapted from the ACES D60 white (0.32168, 0.33767) into the
library's D65 XYZ hub, so signal (1, 1, 1) maps exactly onto the reference white.

Conversion functions (in ``operations/conversion/functions/aces.hpp``):

* ``aces2065_to_xyz`` / ``xyz_to_aces2065``
* ``acescg_to_xyz`` / ``xyz_to_acescg``
* ``acescg_to_acescct`` / ``acescct_to_acescg``

ICtCp
-----

Header: ``include/colorcpp/core/ictcp.hpp``

ICtCp is the perceptual colour encoding used by BT.2100 PQ / Dolby. Channels:

* **I** — PQ-coded intensity [0, 1] (absolute luminance)
* **Ct** — tritan opponent chroma axis (nominal [-1, 1])
* **Cp** — protan opponent chroma axis (nominal [-1, 1])

Formerly an internal detail of ``algorithms/delta_e/itp.hpp``; promoted to a core model
and added to the conversion graph.

YCbCr
-----

Header: ``include/colorcpp/core/ycbcr.hpp``

Three luma matrices: BT.601 (Kr = 0.299, SD), BT.709 (Kr = 0.2126, HD), BT.2020
(Kr = 0.2627, UHD).

Float channels are normalized: Y ∈ [0, 1], Cb/Cr ∈ [-0.5, 0.5] (neutral = 0). Range and
bit depth are *quantization parameters*, not part of the type:

* ``ycbcr::quantize(c, range, bit_depth)`` — normalized → integer codes
* ``ycbcr::dequantize<Color>(codes, range, bit_depth)`` — integer codes → normalized

Range: ``full`` (0..max) or ``limited`` (studio swing). Bit depth: ``depth8`` or
``depth10``.

Quick Start
-----------

.. code-block:: cpp

   #include <colorcpp/colorcpp.hpp>

   using namespace colorcpp::core;
   using namespace colorcpp::operations::conversion;

   // BT.2100 PQ from linear Rec.2020
   auto pq = color_cast<rec2100_pq_rgbf_t>(linear_rec2020);

   // ACES2065-1 from XYZ (archive exchange)
   auto aces = color_cast<aces2065_rgbf_t>(xyz);

   // ACEScct from ACEScg (grading signal)
   auto cct = color_cast<acescct_rgbf_t>(acescg);

   // ICtCp from BT.2100 PQ (perceptual)
   auto ictcp = color_cast<ictcp_t>(pq_color);

   // YCbCr with explicit quantization
   auto ycbcr = color_cast<ycbcr709_t>(rgb_color);
   auto codes = ycbcr::quantize(ycbcr, ycbcr::range::limited, ycbcr::bit_depth::depth10);

References
----------

* `Recommendation ITU-R BT.2100 — Image parameter values for high dynamic range television <https://www.itu.int/rec/R-REC-BT.2100>`__ (PQ/HLG)
* `Recommendation ITU-R BT.709 — Image parameter values for high-definition television <https://www.itu.int/rec/R-REC-BT.709>`__ (Rec.709/BT.1886)
* `SMPTE ST 2065-1 — ACES2065-1 (AP0) <https://www.smpte.org/standboards/standards/ST-2065-1>`__ (ACES archive space)
* `Academy S-2016-003 — ACEScct <https://www.oscars.org/science-technology/cast/aces>`__ (ACEScct grading)
* `Recommendation ITU-R BT.601 — Studio encoding parameters of digital television <https://www.itu.int/rec/R-REC-BT.601>`__ (YCbCr SD)
* `Lu et al. (2016). A Perceptual EOTF for High Dynamic Range Content <https://doi.org/10.1371/journal.pone.0154262>`__ (ICtCp)