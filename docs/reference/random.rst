Random colors
=============

Utilities generate **uniform** random colors in **RGB**, **HSL**, or **HSV** channel ranges, plus **harmony** layouts and **golden-angle** hue spacing built on :doc:`palette` concepts.

In colorcpp
------------

* Header: ``include/colorcpp/operations/random.hpp``
* ``basic_random_generator`` — independent uniform channels (respecting min/max per channel).
* ``hsl_random_generator`` / ``hsv_random_generator`` — hue/saturation/value sampling.
* ``harmony_generator`` — ``harmony_mode`` enum (analogous, complementary, triadic, …).

Notes
-----

* Uses ``<random>`` (e.g. ``std::mt19937`` by default). **Uniform in RGB** is not **perceptually uniform** in hue/chroma.
* Integer channels smaller than ``unsigned short`` are promoted for ``std::uniform_int_distribution`` as required by the C++ standard.

References
----------

* C++ standard — ``<random>`` (`cppreference <https://en.cppreference.com/w/cpp/header/random>`__)
* Golden angle palettes: classical phyllotaxis / golden-ratio spacing (numerous tutorials; not a single normative spec)
