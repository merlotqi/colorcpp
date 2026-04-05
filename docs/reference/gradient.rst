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
* **Gradient sequence** - multi-point gradient chain

Features:

* Custom stop positions with color values
* Built-in easing functions (linear, ease-in, ease-out, ease-in-out, smoothstep)
* Support for all color spaces
* Hue interpolation handling (shorter/longer path)
* Stop position normalization
* Gamut clamping options


Notes
-----

Gradients operate in the working color space they are created in. For perceptually uniform results use Oklab or CIELAB color spaces.

Gradients are lazy evaluated - color sampling happens at evaluation time, no precomputed lookup tables are created by default.


References
----------

* CSS Images Module Level 4 - Gradients
* https://www.w3.org/TR/css-images-4/#gradients
* "Perceptual uniformity in digital color imaging"
