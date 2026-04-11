Color Literals
==============

User-defined C++ literals for convenient inline color creation with natural syntax.

In colorcpp
-----------

Header path: ``<colorcpp/io/literals.hpp>``

Import namespace: ``using namespace colorcpp::io::literals;``

Available literal suffixes:

* ``_rgb`` / ``_rgba`` - sRGB colors (0-255 integer values)
* ``_hsl`` / ``_hsla`` - HSL colors
* ``_hsv`` / ``_hsva`` - HSV colors
* ``_hwb`` / ``_hwba`` - HWB colors
* ``_cmyk`` - CMYK colors
* ``_oklab`` - Oklab perceptual colors
* ``_oklch`` - OkLCH cylindrical perceptual colors


Usage example:

.. code-block:: cpp

    #include <colorcpp/io/literals.hpp>

    using namespace colorcpp::io::literals;

    // Simple RGB
    auto red = 0xFF0000_rgb;
    auto transparent_blue = 0x0000FF80_rgba;

    // HSL
    auto green = 120'100'050_hsl;
    auto light_purple = 270'070'065_hsla;

    // HSV / CMYK / Oklab
    auto sky = 210'080'090_hsv;
    auto print_teal = 50'030'000'020_cmyk;
    auto neutral = 050'050'050_oklab;


Notes
-----

All literals are constexpr and can be used at compile time.

RGB-style literals use hexadecimal forms such as ``0xRRGGBB_rgb`` and ``0xRRGGBBAA_rgba``.

HSL / HSV / HWB / CMYK / Oklab literals use packed decimal groups as documented in the header comments.


References
----------

* ISO C++ Standard - User-defined literals (N3599)
* CSS Color Module Level 4 - Color syntax
