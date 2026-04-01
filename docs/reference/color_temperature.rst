Color Temperature
==================

Overview
--------

Converts correlated color temperature (CCT) in Kelvin to RGB values. Uses Tanner Helland's approximation algorithm based on CIE 1931 xy chromaticity coordinates for the Planckian locus.

In colorcpp
-----------

**Headers:** ``include/colorcpp/algorithms/color_temperature.hpp``

Functions
~~~~~~~~~~

- ``color_temperature::kelvin_to_linear_rgb(kelvin)`` — Kelvin → linear RGB [0, 1]
- ``color_temperature::kelvin_to_rgb(kelvin)`` — Kelvin → gamma-corrected sRGB [0, 1]
- ``color_temperature::kelvin_to_rgba8(kelvin, alpha)`` — Kelvin → 8-bit sRGB

Valid range: 1000K – 40000K.

Example
--------

.. code-block:: cpp

   #include <colorcpp/algorithms/color_temperature.hpp>

   using namespace colorcpp::algorithms::color_temperature;

   auto warm = kelvin_to_rgba8(2700.0f);   // Warm white (tungsten)
   auto daylight = kelvin_to_rgba8(6500.0f); // D65 white point
   auto cool = kelvin_to_rgba8(10000.0f);  // Overcast sky

Notes
-----

- The algorithm produces approximate sRGB values; for scientific accuracy, use spectral data
- Values are clamped to [0, 1] before conversion
- The output is in sRGB color space (gamma-encoded)

References
----------

- `Tanner Helland's Algorithm <https://tannerhelland.com/2012/09/18/convert-temperature-rgb-algorithm-code.html>`_
- `CIE 1931 Color Space <https://en.wikipedia.org/wiki/CIE_1931_color_space>`_
- `Planckian Locus <https://en.wikipedia.org/wiki/Planckian_locus>`_
