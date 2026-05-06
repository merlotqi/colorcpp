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

``exact_policy``, ``epsilon_policy``, and ``relative_policy`` are same-type channel comparisons.

``compare::perceptual_policy`` currently uses ``algorithms::delta_e::delta_e_2000()``, which converts both inputs through ``cielab_t`` and then applies the requested ΔE2000 threshold.

``perceptual_policy``, ``perceptual_equal()``, and ``perceptual_equal_de2000()`` currently support mixed-type and cross-space inputs as long as both inputs can be converted by ``operations::conversion::color_cast``.

Perceptual comparison currently ignores alpha because both inputs are converted to ``cielab_t`` before ΔE2000 is evaluated.

For most user-facing applications use perceptual_policy. For rendering and graphics use epsilon_policy. For cache keys use exact_policy.


References
----------

* ISO 17321-1:2012 Color difference metrics
* "Delta E: The Color Difference" - Bruce Lindbloom
* https://en.wikipedia.org/wiki/Color_difference
