Accessibility: WCAG 2 contrast and APCA
========================================

colorcpp provides **two parallel APIs**: WCAG 2.x **relative luminance** and **contrast ratio**, and **APCA** lightness contrast (Lc). They are **not interchangeable**: do not apply WCAG 2 ratio thresholds to APCA Lc or vice versa.

In colorcpp
------------

* Header: ``include/colorcpp/algorithms/accessibility.hpp``
* WCAG 2: ``relative_luminance``, ``contrast_ratio``, ``is_wcag_compliant``, helpers such as ``contrast_text_color``, ``find_contrast_color``
* APCA: ``apca_luminance``, ``apca_contrast``, ``apca_meets_min_abs_lc``

✅ **WCAG 2 API**:

  * ``relative_luminance(c)``
  * ``contrast_ratio(a, b)``
  * ``is_wcag_compliant(fg, bg, level, size)``
  * ``contrast_text_color(background)``

✅ **APCA API**:

  * ``apca_luminance(c)``
  * ``apca_contrast(text, background)``


Notes
-----

* **WCAG relative luminance** uses **Rec. 709** coefficients on **linear sRGB** obtained via ``color_cast`` (IEC 61966-2-1 linearization).
* **APCA display Y** in this library uses a **single 2.4 gamma** on normalized 8-bit channels and **W3 G-4g-style** weights for RGB→Y, matching the common **SAPC / apca-w3** presentation—not the same numeric pipeline as ``relative_luminance``.
* **Lc** is **signed** and **order-dependent** (text vs background). Published APCA thresholds and lookup tables evolve with W3C Silver / community drafts; this is **not legal advice** for compliance.

References
----------

* `Web Content Accessibility Guidelines (WCAG) 2.2 — Contrast (Minimum) <https://www.w3.org/TR/WCAG22/#contrast-minimum>`__
* `Understanding Success Criterion 1.4.3: Contrast (Minimum) <https://www.w3.org/WAI/WCAG22/Understanding/contrast-minimum.html>`__
* `W3C Silver / Visual Contrast of Text Subgroup (resources on APCA) <https://www.w3.org/WAI/GL/task-forces/silver/wiki/Visual_Contrast_of_Text_Subgroup>`__
* `Myndex APCA (reference implementation and discussion) <https://github.com/Myndex/apca-w3>`__
