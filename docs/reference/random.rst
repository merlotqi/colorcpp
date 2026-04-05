Random colors
=============

Utilities generate **uniform** random colors in **RGB**, **HSL**, or **HSV** channel ranges, plus **harmony** layouts and **golden-angle** hue spacing built on :doc:`palette` concepts.

In colorcpp
------------

* Header: ``include/colorcpp/operations/random.hpp``
* Namespace: ``colorcpp::operations::random``

Available generators:

  * **Basic uniform generators**:
    * ``random_color<ColorType>()`` - Generate single random color
    * ``basic_random_generator`` - Generic random color generator
    * Supports all color spaces

  * **HSL / HSV generators**:
    * ``hsl_random_generator`` - Hue constrained generation
    * ``hsv_random_generator``
    * Optional saturation / lightness range limits

  * **Advanced generators**:
    * ``golden_angle_generator`` - Evenly distributed hue spacing
    * ``harmony_generator`` - Generate color sets following harmony rules
    * Supports all harmony schemes from :doc:`harmony`

  * **Seed control**:
    * All generators accept custom random engines
    * Reproducible results with fixed seed


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
* Golden angle generator provides optimal hue separation for any number of colors
* Harmony generator preserves relative luminance when creating palettes
* All generators respect native channel ranges for each color space

References
----------

* C++ standard — ``<random>`` (`cppreference <https://en.cppreference.com/w/cpp/header/random>`__)
* Golden angle palettes: classical phyllotaxis / golden-ratio spacing (numerous tutorials; not a single normative spec)
