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

  * **Modern Perceptual Metric**:
    * ``delta_e_ok()`` - Oklab Euclidean distance
    * Fast, uniform, and well-behaved across the entire color space
    * Recommended for most new applications
    * Remains scalar by default; the current SIMD path is experimental and requires
      ``COLORCPP_ENABLE_SIMD=ON`` plus ``COLORCPP_ENABLE_EXPERIMENTAL_DELTA_E_OK_SIMD=ON``

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

* ΔE94 is asymmetric: reference color is the first parameter
* ΔE2000 uses double precision internally for accuracy near neutral axis
* ΔE_OK values are approximately 2.5x smaller scale than ΔE2000
* ``delta_e_ok()`` has an optional experimental SIMD-backed path for Oklab inputs when ``COLORCPP_ENABLE_SIMD=ON`` on supported targets
* All metrics work automatically across all input color spaces


References
----------

* `CIE 15:2018 Colorimetry <https://cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`__
* `ISO 11664-6:2022 CIEDE2000 Formula <https://www.iso.org/standard/78907.html>`__
* Sharma, G. et al. (2005) The CIEDE2000 color-difference formula
* `Oklab Perceptual Color Space <https://bottosson.github.io/posts/oklab/>`__
