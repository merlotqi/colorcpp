Color Temperature
==================

Overview
--------

Two families of colour-temperature utilities:

1. **Kelvin → RGB approximation** (Tanner Helland): fast sRGB values for warm/cool
   lighting UI; no colourimetric accuracy claims.
2. **Colour-temperature metrology**: Planckian locus generation and correlated
   colour temperature (CCT) / Duv estimation per Ohno (2013), McCamy (1992) and
   Robertson (1968). All chromaticity math uses the **CIE 1960 UCS (u, v)**
   diagram — not the CIE 1976 u′v′ diagram of CIELUV (whose v′ is 1.5× larger).

In colorcpp
-----------

**Headers:** ``include/colorcpp/algorithms/color_temperature.hpp`` (umbrella; each
piece also lives individually under ``color_temperature/`` — ``kelvin_rgb.hpp``,
``common.hpp``, ``planckian.hpp``, ``ohno.hpp``, ``mccamy.hpp``, ``robertson.hpp``).

Kelvin → RGB approximation
~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``color_temperature::kelvin_to_linear_rgb(kelvin)`` — Kelvin → linear RGB [0, 1]
- ``color_temperature::kelvin_to_rgb(kelvin)`` — Kelvin → gamma-corrected sRGB [0, 1]
- ``color_temperature::kelvin_to_rgba8(kelvin, alpha)`` — Kelvin → 8-bit sRGB

Valid range: 1000K – 40000K.

Planckian locus and CCT / Duv
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``color_temperature::planck_uv(cct)`` — Planckian-locus CIE 1960 uv at a
  temperature (spectral integration of Planck's law against the embedded CIE
  1931 2° CMFs, 5 nm, 360–830 nm; agrees with 1 nm integration to ~3e-7)
- ``color_temperature::planckian_table(start, end, spacing)`` — geometrically
  spaced locus table (Ohno 2014 adaptive spacing); defaults 1000–25000 K, 1.005
- ``color_temperature::uv_to_cct_ohno(u, v)`` — Ohno (2013) CCT + signed Duv;
  overloads for explicit tables, ``xyy_t`` and ``xyz_t``
- ``color_temperature::cct_to_uv_ohno(cct, duv)`` — inverse: locus uv at CCT + Duv
- ``color_temperature::xy_to_cct_mccamy(xyy)`` — McCamy (1992) closed-form cubic
  (fast, less accurate far from the locus; no Duv, no inverse)
- ``color_temperature::uv_to_cct_robertson1968(u, v)`` — Robertson (1968)
  isothermal-line method (30-row table, Lindbloom's 325 mired correction)
- ``color_temperature::cct_to_uv_robertson1968(cct, duv)`` — inverse
- ``color_temperature::cct_to_mired(cct)`` / ``mired_to_cct(mired)`` — micro
  reciprocal degrees
- ``color_temperature::xy_to_uv1960(x, y)`` / ``uv1960_to_xy(u, v)`` /
  ``xyz_to_uv1960(xyz)`` / ``xyy_to_uv1960(xyy)`` — chromaticity helpers

The CCT/Duv result type is ``cct_duv { float cct; float duv; }``. Duv is a
**signed** distance in the CIE 1960 uv plane (positive above the locus).

Example
--------

.. code-block:: cpp

   #include <colorcpp/algorithms/color_temperature.hpp>

   using namespace colorcpp::algorithms::color_temperature;

   // Kelvin -> sRGB approximation (legacy, UI use)
   auto warm = kelvin_to_rgba8(2700.0f);

   // Metrology: CCT + Duv of a measured chromaticity (Ohno 2013)
   const cct_duv r = uv_to_cct_ohno(0.1978f, 0.3122f);
   // r.cct ≈ 6507.5 K, r.duv ≈ 0.00322

   // Inverse: where does 6500 K sit, 0.002 above the locus?
   auto uv = cct_to_uv_ohno(6500.0f, 0.002f);

Notes
-----

- The Helland approximation is not spectrally accurate; use the metrology
  functions for anything colourimetric
- ``uv_to_cct_ohno`` caches a default Planckian table (1000–25000 K, spacing
  1.005) on first use; pass an explicit ``planckian_table`` to control the
  precision/range trade-off (smaller spacing ⇒ more precise, slower build)
- Ohno (2013) selects between triangular and parabolic interpolation with a
  |Duv| ≥ 0.002 rule; cross-method agreement between Ohno and Robertson is
  typically within ~10 K on real samples
- McCamy can differ by tens of kelvin far from the Planckian locus

References
----------

- `Tanner Helland's Algorithm <https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html>`_
- `Ohno, N. (2013). Practical Locus and CCT Estimation. Proc. CIE 2013 <https://www.cie.co.at/publications/practical-locus-and-cct-estimation>`_
- `Ohno, N. (2014). Practical Use and Calculation of CCT and Duv. LEUKOS 10(1) <https://doi.org/10.1080/15502724.2014.839020>`_
- `McCamy, C. S. (1992). Correlation colour temperature by way of the chromaticity diagram. Color Res. Appl. 17(5), 381–387`
- `Robertson, A. R. (1968). Calculation of correlated colour temperature from the chromaticity diagram. JOSA 58(11), 1528–1532`
- `CIE 015:2018 — Colorimetry, 4th Edition <https://www.cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`_
