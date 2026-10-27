Color Comparison
================

Policy-based color equality checking with multiple comparison strategies for different use cases.

In colorcpp
-----------

Header path: ``<colorcpp/operations/compare.hpp>``

Main namespace: ``colorcpp::operations::compare``

Available comparison policies:

* **exact_policy** - Channel-wise equality (``==`` per channel; hue channels compare on the ring, so 0° and 360° are equal)
* **epsilon_policy** - Absolute tolerance per channel in that channel's own units (``|a - b| <= eps``; hue in degrees on the ring)
* **relative_policy** - Relative tolerance scaled to value magnitude (hue uses the circular distance)
* **perceptual_policy** - Perceptual equality with a selectable ΔE metric and an inclusive threshold (``distance <= threshold``)

Usage patterns:

.. code-block:: cpp

    // Default exact comparison
    bool same = compare::equal(color_a, color_b);

    // Absolute epsilon: 0.01 in each channel's own units (Lab lightness stays 0.01)
    bool close = compare::equal<compare::epsilon_policy>(color_a, color_b, 0.01f);

    // Perceptual similarity, CIEDE2000, inclusive threshold
    bool visually_same = compare::equal<compare::perceptual_policy>(color_a, color_b, 1.0f);

    // Perceptual similarity on OkLab distance (threshold defaults to oklab_jnd_typical ≈ 0.02)
    bool same_ok = compare::perceptual_equal_ok(color_a, color_b);

    // Alpha participation is explicit for perceptual comparison
    bool opaque_must_match = compare::perceptual_equal(color_a, color_b, 1.0f, compare::alpha_mode::exact);


Notes
-----

``exact_policy``, ``epsilon_policy``, and ``relative_policy`` are same-type channel comparisons.
Hue channels (HSL/HSV/HWB, CIELCh, OkLCh) are circular: equality and tolerance use the shortest
arc, so ``h = 0`` and ``h = 360`` are the same hue.

``epsilon_policy`` is an **absolute** tolerance in each channel's own units — the same ``eps``
means 0.01 on a ``[0, 1]`` RGB channel and 0.01 on a CIELAB lightness of ``[0, 100]``.

Perceptual comparison is built on ``algorithms::delta_e``. The default metric is CIEDE2000
(``perceptual_policy`` / ``perceptual_equal`` / ``perceptual_equal_de2000``, default threshold
1.0); ``perceptual_oklab_policy`` / ``perceptual_equal_ok`` use OkLab Euclidean distance with
the library-wide JND scale (default threshold ``oklab_jnd_typical`` ≈ 0.02, consistent with
gamut mapping). ΔE2000 weights ``kL``/``kC``/``kH`` can be passed through. The threshold is
inclusive: ``distance == threshold`` compares equal.

Perceptual comparison converts both inputs through non-alpha spaces, so alpha is dropped by
default; pass ``alpha_mode::exact`` to require equal alpha when both inputs carry it.
Mixed-type and cross-space inputs work as long as both convert via ``operations::conversion::color_cast``.

Guidance: use perceptual comparison for user-facing "same color?" questions (pick the metric
whose threshold scale you reason about — ΔE2000 or OkLab), and epsilon/exact for
rendering and graphics pipelines.


References
----------

* ISO 17321-1:2012 Color difference metrics
* "Delta E: The Color Difference" - Bruce Lindbloom
* https://en.wikipedia.org/wiki/Color_difference
