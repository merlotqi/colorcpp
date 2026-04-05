Color Comparison
================

Policy-based color equality checking with multiple comparison strategies for different use cases.

In colorcpp
-----------

Header path: ``<colorcpp/operations/compare.hpp>``

Main namespace: ``colorcpp::operations::compare``

Available comparison policies:

* **exact_policy** - Strict bitwise exact channel comparison
* **epsilon_policy** - Fixed tolerance comparison for floating point colors
* **relative_policy** - Relative tolerance comparison scaled to value magnitude
* **perceptual_policy** - Perceptual equality using ΔE 2000 distance threshold

Usage patterns:

.. code-block:: cpp

    // Default exact comparison
    bool same = compare::equal(color_a, color_b);

    // Epsilon tolerance
    bool close = compare::equal<compare::epsilon_policy>(color_a, color_b, 0.01f);

    // Perceptual similarity (just noticeable difference threshold)
    bool visually_same = compare::equal<compare::perceptual_policy>(color_a, color_b, 1.0f);


Notes
-----

All comparison policies work across all color spaces. Cross-space comparison is not allowed, colors must be in the same color space.

Perceptual comparison automatically converts colors to Oklab space internally for uniform distance calculation.

For most user-facing applications use perceptual_policy. For rendering and graphics use epsilon_policy. For cache keys use exact_policy.


References
----------

* ISO 17321-1:2012 Color difference metrics
* "Delta E: The Color Difference" - Bruce Lindbloom
* https://en.wikipedia.org/wiki/Color_difference
