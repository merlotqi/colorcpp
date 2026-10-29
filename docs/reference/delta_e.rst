Color difference (ΔE)
======================

Perceptual color difference metrics for measuring how visually distinct two colors appear.

In colorcpp
------------

* Header: ``include/colorcpp/algorithms/delta_e.hpp``
* Namespace: ``colorcpp::algorithms::delta_e``

Available metrics:

  * **CIE Standard Metrics**:
    * ``delta_e_76()`` - CIE 1976 Euclidean distance in LAB space
    * ``delta_e_94()`` - CIE 94 weighted metric (graphic arts)
    * ``delta_e_2000()`` - CIEDE2000 industry standard metric

  * **Industry Standard Metrics**:
    * ``delta_e_cmc()`` - CMC l:c (Colour Measurement Committee, 1984)
        - Developed for textile industry
        - Configurable weights: l (lightness, default 2.0) and c (chroma, default 1.0)
        - Common presets: l=2, c=1 (acceptability), l=1, c=1 (perceptibility)
    * ``delta_e_din99()`` - DIN99 (DIN 6176)
        - Euclidean distance in the DIN99-transformed LAB space
        - Supports ``k_L`` lightness scaling and inverse ``k_E`` overall scaling
        - Checked against equation-derived reference pairs from the published DIN99 transform

  * **HDR / Modern Metrics**:
    * ``delta_e_itp()`` - ΔE_ITP in IC_T_C P (ITU-R BT.2124)
        - 720·√(ΔI² + (ΔCt/2)² + ΔCp²); a value of 1 ≈ JND under critical viewing
        - Operates on ``ictcp_t`` values; use ``xyz_to_ictcp()`` / ``ictcp_to_xyz()``
          (XYZ D65 → Linear BT.2020 → LMS' → PQ → IC_T_C P) for other colour types
        - Designed for HDR (PQ, up to 10000 cd/m²) and wide gamut content
    * ``delta_e_hyab()`` - Hybrid absolute-Bernard metric in CIELAB (Abasi et al. 2020)
        - |ΔL*| + √(Δa*² + Δb*²): absolute lightness term plus planar chroma distance
        - Avoids the hue-collapse artefact of Euclidean ΔE76 for large lightness differences
    * ``delta_e_hych()`` - Hybrid chroma-hue metric in CIELAB (Abasi et al. 2020)
        - |ΔL'/(kL·SL)| + √((ΔC'/(kC·SC))² + (ΔH'/(kH·SH))²) on CIE 2000 intermediates
        - HyAB's hue-aware partner for large differences (~10 CIELAB units and up);
          the textile parametrization is the ``kL = 2`` preset
    * ``delta_e_ok()`` - Oklab Euclidean distance
    * Fast, uniform, and well-behaved across the entire color space
    * Recommended for most new applications
    * Remains scalar by default; the current SIMD path is experimental and requires
      ``COLORCPP_ENABLE_SIMD=ON`` plus ``COLORCPP_ENABLE_EXPERIMENTAL_DELTA_E_OK_SIMD=ON``
    * ``delta_e_jz()`` - Jzazbz Euclidean distance (Safdar et al. 2017)
        - Natural companion metric for HDR / wide-gamut Jzazbz pipelines
        - No community-standard JND constant: never reuse ΔE2000 or ΔE_OK thresholds

  * **Utility functions**:
    * ``is_visually_same()`` - Compare using ΔE2000 (default threshold 1.0)
    * ``is_visually_same_cmc()`` - Compare using CMC metric
    * ``is_visually_same_din99()`` - Compare using DIN99 metric
    * ``is_visually_same_ok()`` - Compare using Oklab (default threshold ~0.02)

  * **Utility constants**:
    * ``oklab_jnd_typical`` - Typical just noticeable difference threshold (~0.02)


Typical threshold values for ΔE2000:

| Value | Perception |
|-------|------------|
| < 1   | Imperceptible to human eye |
| 1-2   | Only visible through close inspection |
| 2-5   | Clearly noticeable difference |
| 5-10  | Significant visual difference |
| > 50  | Completely different colors |


Notes
-----

* ΔE94 and CMC l:c are asymmetric: the reference color is the first parameter
* Parametric weights (``kL``/``kC``/``kH``, CMC ``l``/``c``, DIN99 ``k_L``/``k_E``)
  are positive scaling factors; non-positive values fall back to 1.0 instead of
  dividing by zero
* ΔE2000 uses double precision internally for accuracy near neutral axis
* ΔE_OK values are far smaller in scale than ΔE2000 — the JND anchors are 0.02 vs 1.0
  (~50x), and the gap widens with the size of the difference (typically 50–100x and up).
  Never reuse ΔE2000 thresholds for ΔE_OK; use ``oklab_jnd_typical`` instead
* ``delta_e_ok()`` has an optional experimental SIMD-backed path for Oklab inputs when ``COLORCPP_ENABLE_SIMD=ON`` on supported targets
* All metrics work automatically across all input color spaces


References
----------

* `CIE 15:2018 Colorimetry <https://cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`__
* `ISO 11664-6:2022 CIEDE2000 Formula <https://www.iso.org/standard/78907.html>`__
* Sharma, G. et al. (2005) The CIEDE2000 color-difference formula
* `ITU-R BT.2124 — Objective difference metric for HDR/WCG content <https://www.itu.int/rec/R-REC-BT.2124>`__
* `Oklab Perceptual Color Space <https://bottosson.github.io/posts/oklab/>`__
* Abasi, S., Akyüz, M. S., & Lenz, R. (2020). Accuracy of Color Difference Formulae in Complex Scenes. Journal of Imaging.
* Safdar, M., Cui, G., Kim, Y. J., & Luo, M. R. (2017). Perceptually uniform color space for image signals including high dynamic range and wide gamut. Optics Express.
