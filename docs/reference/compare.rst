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
* **relative_policy** - Relative tolerance with an absolute floor: ``max(rel * max(|a|,|b|), abs_floor)`` (hue uses the circular distance)
* **perceptual_policy** - Perceptual equality with a selectable ΔE metric and an inclusive threshold (``distance <= threshold``)

Usage patterns:

.. code-block:: cpp

    // Default exact comparison
    bool same = compare::equal(color_a, color_b);

    // Absolute epsilon: 0.01 in each channel's own units (Lab lightness stays 0.01)
    bool close = compare::equal<compare::epsilon_policy>(color_a, color_b, 0.01f);

    // Relative with absolute floor (isclose-style): ratios work on integer
    // channels too (0.02f is a real 2%, not a truncated zero)
    bool scaled = compare::relative_equal(color_a, color_b, 0.02f, 1e-4f);

    // Perceptual similarity, CIEDE2000, inclusive threshold
    bool visually_same = compare::equal<compare::perceptual_policy>(color_a, color_b, 1.0f);

    // Perceptual similarity on OkLab distance (threshold defaults to oklab_jnd_typical ≈ 0.02)
    bool same_ok = compare::perceptual_equal_ok(color_a, color_b);

    // Raw distance for ranking / bucketing / closest-color search
    float d = compare::perceptual_distance(color_a, color_b);       // ΔE2000
    float d_ok = compare::perceptual_distance_ok(color_a, color_b); // ΔE_OK (JND ≈ 0.02 scale)

    // Alpha participation is explicit for perceptual comparison
    bool opaque_must_match = compare::perceptual_equal(color_a, color_b, 1.0f, compare::alpha_mode::exact);


Notes
-----

``exact_policy``, ``epsilon_policy``, and ``relative_policy`` are same-type channel comparisons.
Channel arguments must be color types; non-colors fail with a clear static_assert at the policy
boundary.

**Hue ring.** Hue channels (HSL/HSV/HWB, CIELCh, OkLCh) are circular: equality and tolerance
use the shortest arc, so ``h = 0`` and ``h = 360`` are the same hue. The hue channel is
recognized by the shared ``traits::hue_tag`` (family-local ``h_tag`` names alias it).

**Achromatic gate.** When both colors' chroma channel (shared ``traits::chroma_tag``: HSL/HSV
``S``, CIELCH/OkLCH ``C``) is exactly zero, the hue comparison short-circuits to equal — a gray
at ``h = 0`` and the same gray at ``h = 180`` are the same color. One-sided achromatic pairs
still compare chroma (the saturated side differs there anyway). Models without a chroma channel
(HWB, RGB) keep plain ring semantics; HWB's achromatic range (``W + B >= 1``) is not gated.

``epsilon_policy`` is an **absolute** tolerance in each channel's own units — the same ``eps``
means 0.01 on a ``[0, 1]`` RGB channel and 0.01 on a CIELAB lightness of ``[0, 100]``.

``relative_policy`` takes ``float`` tolerances regardless of the channel type, so integer
channels can express sub-100% ratios. The optional ``abs_floor`` covers the near-zero case
where pure relative tolerance underflows (``0`` vs ``1e-4`` at ``rel = 1e-3``: the relative
bound is ``1e-7``; ``abs_floor = 1e-3`` passes). A negative ``rel`` or ``abs_floor`` never
matches, including for identical values.

Perceptual comparison is built on ``algorithms::delta_e``. The default metric is CIEDE2000
(``perceptual_policy`` / ``perceptual_equal`` / ``perceptual_equal_de2000``, default threshold
1.0); ``perceptual_oklab_policy`` / ``perceptual_equal_ok`` use OkLab Euclidean distance with
the library-wide JND scale (default threshold ``oklab_jnd_typical`` ≈ 0.02, consistent with
gamut mapping). ΔE2000 weights ``kL``/``kC``/``kH`` can be passed through. The threshold is
inclusive: ``distance == threshold`` compares equal. ``perceptual_distance`` /
``perceptual_distance_de2000`` / ``perceptual_distance_ok`` return the raw distance for
ranking and search without re-deriving ΔE.

Perceptual comparison converts both inputs through non-alpha spaces, so alpha is dropped by
default; pass ``alpha_mode::exact`` to require equal alpha when both inputs carry it.
Mixed-type and cross-space inputs work as long as both convert via ``operations::conversion::color_cast``.

**As map keys.** Colors are usable as ``std::unordered_map`` keys (``std::hash`` is defined
per family via ``COLORCPP_ENABLE_HASH``; ``a == b`` implies equal hashes, ±0.0 agree) and as
``std::map`` keys (``operator<`` is lexicographic in channel order). NaN channel values never
compare equal and have unspecified ordering — do not use NaN-containing colors as keys.

**Header surface.** The aggregate ``<colorcpp/operations/compare.hpp>`` includes every policy
and therefore the conversion/ΔE graph. Translation units that only need exact/epsilon/relative
comparison should include the individual policy headers (``compare/exact.hpp``,
``compare/epsilon.hpp``, ``compare/relative.hpp``, ``compare/equal.hpp`` for the dispatcher) —
those stay free of the conversion graph.

Guidance: use perceptual comparison for user-facing "same color?" questions (pick the metric
whose threshold scale you reason about — ΔE2000 or OkLab), and epsilon/exact for
rendering and graphics pipelines.


References
----------

* ISO 17321-1:2012 Color difference metrics
* "Delta E: The Color Difference" - Bruce Lindbloom
* https://en.wikipedia.org/wiki/Color_difference
