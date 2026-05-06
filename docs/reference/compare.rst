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
* **perceptual_policy** - Perceptual equality using a ΔE2000 distance threshold

Usage patterns:

.. code-block:: cpp

    // Default exact comparison
    bool same = compare::equal(color_a, color_b);

    // Epsilon tolerance
    bool close = compare::equal<compare::epsilon_policy>(color_a, color_b, 0.01f);

    // Perceptual similarity (CIELAB conversion + ΔE2000 threshold)
    bool visually_same = compare::equal<compare::perceptual_policy>(color_a, color_b, 1.0f);


Notes
-----

All comparison policies work across supported color spaces. Cross-space comparison is not allowed; inputs must describe the same color space.

``compare::perceptual_policy`` currently uses ``algorithms::delta_e::delta_e_2000()``, which converts both inputs through CIELAB and then applies the requested ΔE2000 threshold.

``perceptual_equal()`` and ``compare::equal<compare::perceptual_policy>(...)`` support mixed color types as long as both inputs can be converted by ``operations::conversion::color_cast``.

For most user-facing applications use perceptual_policy. For rendering and graphics use epsilon_policy. For cache keys use exact_policy.


References
----------

* ISO 17321-1:2012 Color difference metrics
* "Delta E: The Color Difference" - Bruce Lindbloom
* https://en.wikipedia.org/wiki/Color_difference
