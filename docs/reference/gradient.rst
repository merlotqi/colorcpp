Gradients
=========

Generates smooth color transitions with multiple gradient types, easing functions and stop management.

In colorcpp
-----------

Header path: ``<colorcpp/algorithms/gradient.hpp>``

Main namespace: ``colorcpp::algorithms::gradient``

Available gradient types:

* **Linear gradient** - standard straight line interpolation
* **Radial gradient** - circular gradient with center and radius
* **Angular gradient** - conical gradient rotating around center point
* **Box gradient** - square gradient using Chebyshev distance from center
* **Diamond gradient** - diamond gradient using Manhattan distance from center
* **Gradient sequence** - multi-point gradient chain
* **Stepped gradient** - quantized wrapper that buckets sample positions into discrete levels
* **Presets** - built-in named gradients (viridis, magma, plasma, inferno, cividis, rainbow, spectral, ocean, forest, sunset, night)

Features:

* Custom stop positions with color values
* Built-in easing functions: linear, smooth_step, smoother_step, cubic-bézier, plus ease-in/out/in-out families (sine, cubic, quad, expo, circ)
* Support for all color spaces
* Stop position normalization


Notes
-----

Gradients operate in the working color space they are created in. For perceptually uniform results use Oklab or CIELAB color spaces.

Gradients are lazy evaluated - color sampling happens at evaluation time, no precomputed lookup tables are created by default.


References
----------

* CSS Images Module Level 4 - Gradients
* https://www.w3.org/TR/css-images-4/#gradients
* "Perceptual uniformity in digital color imaging"
