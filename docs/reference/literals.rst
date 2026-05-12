Color Literals
==============

User-defined C++ literals for convenient inline color creation in the supported common authoring spaces.

In colorcpp
-----------

Header path: ``<colorcpp/io/literals.hpp>``

Import namespace: ``using namespace colorcpp::io::literals;``

Available literal suffixes:

* ``_rgb`` / ``_rgba`` / ``_argb`` / ``_hex`` - RGBA8-oriented hex literals
* ``_hsl`` / ``_hsla`` - HSL colors
* ``_hsv`` / ``_hsva`` - HSV colors
* ``_hwb`` / ``_hwba`` - HWB colors
* ``_cmyk`` - CMYK colors
* ``_oklab`` - three-channel Oklab perceptual colors
* ``_oklch`` - three-channel OkLCH perceptual colors


Usage example:

.. code-block:: cpp

    #include <colorcpp/io/literals.hpp>

    using namespace colorcpp::io::literals;

    // Simple RGB
    auto red = 0xFF0000_rgb;
    auto transparent_blue = 0x0000FF80_rgba;

    // HSL
    auto green = 120'100'050_hsl;
    auto light_purple = 270'070'065'085_hsla;

    // HSV / CMYK / Oklab
    auto sky = 210'080'090_hsv;
    auto print_teal = 50'030'000'020_cmyk;
    auto neutral = 050'050'050_oklab;


Notes
-----

Numeric literal families are constexpr-friendly and use packed numeric groups documented in the header comments.

``_hex`` validates runtime string input and throws ``std::invalid_argument`` on malformed length or invalid digits.

The literals layer is intentionally curated around common authoring spaces; not every public core color model has a literal suffix.

Oklab / OkLCH currently expose three-channel literal forms only.


References
----------

* ISO C++ Standard - User-defined literals (N3599)
* CSS Color Module Level 4 - Color syntax
