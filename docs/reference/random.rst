Random colors
=============

Utilities generate **uniform** random colors in **RGB**, **HSL**, or **HSV** channel ranges, plus **harmony** layouts and **golden-angle** hue spacing built on :doc:`palette` concepts.

In colorcpp
------------

* Header: ``include/colorcpp/operations/random.hpp``
* Generator namespace: ``colorcpp::operations::random``
* Convenience functions: ``colorcpp::operations::random_color`` / ``colorcpp::operations::random_colors``

Thread safety
-------------

* A **single generator object** is not thread-safe: the implementation keeps a random engine in ``mutable`` state, and ``const`` methods such as ``next()`` still advance it. Use one generator per thread, external locking, or separate instances.
* ``harmony_generator::next_poisson`` also modifies the caller’s ``samples`` vector; synchronize that vector if it is shared across threads.
* ``random_color`` / ``random_colors`` construct a **new** generator each call; different calls do not share state. The default seed uses ``std::random_device{}()`` **per call**, which is implementation-defined (entropy, blocking, performance).

Per-channel and perceptual spaces
---------------------------------

* ``basic_random_generator`` draws each channel **uniformly** in its model min/max. For **OkLab, CIELAB, OkLCH, CIELCH**, etc., that is an **axis-aligned box** in those coordinates; results are **not** clamped to sRGB, Display P3, or other display gamuts.
* ``std::uniform_real_distribution`` produces values in **[a, b)**; the upper bound is not included (usually negligible for floats).

HSL / HSV / HWB and alpha
--------------------------

* ``basic_hsl_generator`` supports optional ranges for H, S, and L (or V/W). For **four-channel** types (HSLA, HSVA, HWBA), **alpha** is random in ``[a_min, a_max]`` (defaults ``[0, 1]``). ``set_options`` validates ``a_min <= a_max`` for those types.
* ``set_options`` throws ``std::invalid_argument`` if any configured min exceeds max (same rules as construction).

Golden angle
------------

* ``golden_angle_generator``: the **first** ``next()`` returns the **initial random** hue chosen at construction; each subsequent ``next()`` adds the golden angle (137.50776405°) on ``[0, 360)``.

Poisson disk (hue)
------------------

* ``next_poisson`` appends to ``samples`` only when a hue satisfies the minimum angular spacing. If no hue is found after the main attempts plus a second hue-only phase, it returns a random color **without** appending a bad hue to ``samples``.

Available generators:

  * **Basic uniform generators**:
    * ``random_color<ColorType>()`` — single random color
    * ``basic_random_generator`` — generic per-channel uniform
    * Supports many color spaces (see header typedefs)

  * **HSL / HSV / HWB generators**:
    * ``basic_hsl_generator`` / ``hsl_generator``, ``hsva_generator``, etc.
    * Optional saturation / lightness (or value/whiteness) range limits; alpha range for 4-channel models

  * **Advanced generators**:
    * ``golden_angle_generator`` — golden-angle hue stepping from a random start
    * ``harmony_generator`` — palettes following harmony rules (see :doc:`harmony`)

  * **Seed control**:
    * Generators accept a seed or a copied engine for reproducibility


Generator properties:

| Generator | Uniformity | Hue Distribution | Typical Use |
|-----------|------------|-------------------|-------------|
| RGB uniform | Channel uniform | Clustered at primaries | Technical testing |
| HSL uniform | Angular uniform | Evenly distributed | UI design |
| Golden angle | Optimal spacing | Maximally separated | Palette generation |
| Harmony generator | Constrained | Following scheme | Design layouts |


Notes
-----

* Default random engine is ``std::mt19937`` Mersenne Twister
* Uniform RGB distribution is not perceptually uniform
* Harmony generator preserves non-hue channels from the base when building partner hues

References
----------

* C++ standard — ``<random>`` (`cppreference <https://en.cppreference.com/w/cpp/header/random>`__)
* Golden angle palettes: classical phyllotaxis / golden-ratio spacing (numerous tutorials; not a single normative spec)
