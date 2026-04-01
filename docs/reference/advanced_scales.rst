Advanced gradient scales
========================

Advanced gradient scales provide sophisticated gradient generation with easing functions, spline curves, and multi-color transitions beyond basic linear interpolation.

In colorcpp
-----------

* Header: ``include/colorcpp/operations/palette/advanced_scales.hpp``
* Namespace: ``colorcpp::operations::palette``

Easing scale
------------

``easing_scale()`` generates gradients with easing functions for non-linear transitions:

.. code-block:: cpp

  easing_scale(start, end, count, easing_type, lerp_method)

**Easing types:**

* ``linear`` — constant speed
* ``ease_in`` — slow start (quadratic)
* ``ease_out`` — slow end (quadratic)
* ``ease_in_out`` — slow start and end (quadratic)
* ``cubic_in`` / ``cubic_out`` / ``cubic_in_out`` — cubic easing
* ``quartic_in`` / ``quartic_out`` / ``quartic_in_out`` — quartic easing
* ``elastic`` — elastic bounce effect
* ``bounce`` — bouncing effect

**Interpolation methods:**

* ``lerp_method::rgb`` — RGB linear interpolation
* ``lerp_method::hsl`` — HSL interpolation
* ``lerp_method::hsv`` — HSV interpolation
* ``lerp_method::oklab`` — OkLab perceptual interpolation (default)

Example:

.. code-block:: cpp

  // Ease-in gradient from red to blue
  auto gradient = easing_scale(red, blue, 16, easing_type::ease_in);
  
  // Bounce effect with HSL interpolation
  auto bounce = easing_scale(red, blue, 16, easing_type::bounce, lerp_method::hsl);

Spline scale
------------

``spline_scale()`` generates smooth curves through multiple control points using Catmull-Rom splines:

.. code-block:: cpp

  spline_scale(control_points, count, cr_mode)

**Parameterization modes:**

* ``cr_mode::uniform`` — standard Catmull-Rom
* ``cr_mode::centripetal`` — prevents cusps (default)
* ``cr_mode::chordal`` — more even spacing

The spline operates in OkLab space for perceptually smooth transitions. Values are clamped to valid ranges to prevent out-of-gamut issues.

Example:

.. code-block:: cpp

  std::vector<rgba8_t> points = {red, yellow, green, blue};
  auto curve = spline_scale(points, 16);  // Smooth curve through 4 points
  auto curve2 = spline_scale(points, 16, cr_mode::chordal);

Multi-color scale
-----------------

``multi_scale()`` generates smooth transitions through multiple colors:

.. code-block:: cpp

  multi_scale(colors, count, lerp_method)

Colors are evenly distributed across the output, with smooth interpolation between each pair.

Example:

.. code-block:: cpp

  std::vector<rgba8_t> rainbow = {red, yellow, green, cyan, blue};
  auto gradient = multi_scale(rainbow, 20);  // 20 colors through 5 stops
  auto hsl_gradient = multi_scale(rainbow, 20, lerp_method::hsl);

Notes
-----

* ``spline_scale`` uses Catmull-Rom interpolation in OkLab space for perceptual uniformity
* Values are automatically clamped to valid ranges (L: 0-1, a/b: -0.5 to 0.5)
* For 2 control points, ``spline_scale`` falls back to linear interpolation
* ``multi_scale`` distributes samples evenly across segments

References
----------

* Catmull-Rom spline: https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline
* Easing functions: https://easings.net/
* OkLab color space: https://bottosson.github.io/posts/oklab/
