ANSI Terminal Output
=====================

Overview
--------

ANSI terminal utilities for debugging and visualizing colors directly in the terminal. Provides colored swatches, palettes, gradients, and WCAG contrast previews using 24-bit ANSI escape codes.

In colorcpp
-----------

**Headers:** ``include/colorcpp/io/ansi.hpp``

Functions
~~~~~~~~~~

- ``ansi::fg(r, g, b)`` / ``ansi::bg(r, g, b)`` — ANSI escape sequences for foreground/background
- ``ansi::print_swatch(os, color, width)`` — print a colored block
- ``ansi::print_color(os, color, label)`` — print swatch + hex + RGB
- ``ansi::print_color_verbose(os, color, label)`` — print swatch + hex + RGB + HSL
- ``ansi::print_palette(os, colors, count)`` — print multiple swatches in a row
- ``ansi::print_gradient(os, a, b, steps)`` — print a gradient between two colors
- ``ansi::print_contrast(os, fg, bg)`` — print WCAG contrast check with preview

Contrast Utilities
~~~~~~~~~~~~~~~~~~~

- ``ansi::relative_luminance(r, g, b)`` — WCAG 2.0 relative luminance
- ``ansi::contrast_ratio(a, b)`` — WCAG contrast ratio
- ``ansi::wcag_level(ratio)`` — returns "AAA", "AA", "AA Large", or "Fail"

Example
--------

.. code-block:: cpp

   #include <colorcpp/io/ansi.hpp>

   using namespace colorcpp::io::ansi;

   // Print color info
   print_color(std::cout, "coral"_color, "coral");
   // Output: ██████ coral #ff7f50ff  RGB(255,127,80)

   // Print gradient
   print_gradient(std::cout, "red"_color, "blue"_color);

   // Check contrast
   print_contrast(std::cout, "white"_color, "navy"_color);
   // Output:  Aa  #fff on #000080  contrast: 14.4:1  AAA

Notes
-----

- Uses 24-bit (true color) ANSI escape codes — requires a modern terminal
- Any color type is accepted (automatically converted to rgba8_t for display)
- HSL values in verbose mode are computed from RGB (not from the original color space)

References
----------

- `ANSI Escape Codes <https://en.wikipedia.org/wiki/ANSI_escape_code>`_
- `WCAG 2.0 Contrast <https://www.w3.org/TR/WCAG20/#visual-audio-contrast-contrast>`_
