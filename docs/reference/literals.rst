Color Literals
==============

User-defined C++ literals for convenient inline color creation with natural syntax.

In colorcpp
-----------

Header path: ``<colorcpp/io/literals.hpp>``

Import namespace: ``using namespace colorcpp::literals;``

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

    using namespace colorcpp::literals;

    // Simple RGB
    auto red = 0xff0000_rgb;
    auto transparent_blue = 0x0000ffaa_rgba;

    // HSL
    auto green = 120_hsl;
    auto light_purple = 270_deg 70% 50% _hsl;

    // Oklab
    auto neutral_gray = 0.5_oklab;


Notes
-----

All literals are constexpr and can be used at compile time.

Integer literals accept both hexadecimal (0xRRGGBB) and decimal values.

Floating point literals support percentage syntax with ``%`` suffix when used with C++14 or later.


References
----------

* ISO C++ Standard - User-defined literals (N3599)
* CSS Color Module Level 4 - Color syntax
