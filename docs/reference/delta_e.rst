Color difference (ΔE)
======================

ΔE metrics compare two colors after conversion to **CIELAB** (D65). They answer “how far apart” two colors are in a perceptually motivated space, not whether they match under a specific print or viewing condition.

In colorcpp
------------

* Header: ``include/colorcpp/operations/delta_e.hpp``
* Functions: ``delta_e_76``, ``delta_e_94``, ``delta_e_2000`` (and related overloads as documented in the header).

Notes
-----

* **ΔE76** is Euclidean distance in LAB: fast but non-uniform across hue and chroma.
* **ΔE94** is asymmetric: the reference sample is the first argument.
* **ΔE2000** follows **ISO 11664-6 / CIE 142-2001**; the implementation uses **double** internally to reduce accumulation error near the achromatic axis.
* Rule-of-thumb bands for ΔE2000 are summarized in the header Doxygen (JND-scale comments).

References
----------

* `CIE 1976 LAB Euclidean metric (historical ΔE) <https://cie.co.at/>`__ — see CIE 015 and related CIE publications
* `ISO 11664-6:2022 — CIEDE2000 formula <https://www.iso.org/standard/78907.html>`__
* Sharma, G., Wu, W., & Dalal, E. N. (2005). *The CIEDE2000 color-difference formula: Implementation notes, supplementary test data, and mathematical observations.* `DOI 10.1002/col.20070 <https://doi.org/10.1002/col.20070>`__
