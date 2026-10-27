Color Literals
==============

User-defined C++ literals for convenient inline color creation, plus constructor
spelling for every other color space.

In colorcpp
-----------

Header path: ``<colorcpp/io/literals.hpp>``

Import namespace: ``using namespace colorcpp::io::literals;``

Available literal suffixes:

* ``_rgb`` / ``_rgba`` / ``_argb`` - packed hex numeric literals (6 or 8 hex digits)
* ``_hex`` - CSS-style hex string (``#RGB``, ``#RGBA``, ``#RRGGBB``, ``#RRGGBBAA``)

Usage example:

.. code-block:: cpp

    #include <colorcpp/io/literals.hpp>

    using namespace colorcpp::io::literals;

    // Simple RGB
    auto red = 0xFF0000_rgb;
    auto transparent_blue = 0x0000FF80_rgba;
    auto red_again = 0xFFFF0000_argb;

    // Hex string
    auto tomato = "#FF6347"_hex;


Hex literal rules
-----------------

* ``_rgb`` requires exactly 6 hex digits; ``_rgba`` and ``_argb`` require exactly 8.
  Any other digit count is a compile error — the literals decode through a
  ``template <char...>`` operator whose ``static_assert`` checks the digit count,
  so e.g. ``0xFF0000_rgba`` can never silently decode to the wrong color.
* The ``0x`` / ``0X`` prefix and digit separators (``0xFF'0000_rgb``) are accepted
  and skipped; every remaining character is read as a **hex** digit.
* ``_hex`` validates runtime string input and throws ``std::invalid_argument`` on
  malformed length or invalid digits.

Non-RGB color spaces: use constructors
--------------------------------------

HSL, HSV, HWB, CMYK, Oklab and OkLCH are created with their constructors.
(The decimal packed suffixes ``_hsl`` / ``_hsv`` / ``_hwb`` / ``_cmyk`` /
``_oklab`` / ``_oklch`` were removed: their digit groups collided with C++
octal lexing — ``050'...`` could fail to compile before the library ever saw
the token — and packed decimal groups were easy to misread.)

.. code-block:: cpp

    using namespace colorcpp;

    // HSL / HSV / HWB: float channels, nominal ranges (hue in degrees, rest in [0, 1])
    auto green = hsl_float_t{120.0f, 1.0f, 0.5f};          // H, S, L
    auto light_purple = hsla_float_t{270.0f, 0.7f, 0.65f, 0.85f};
    auto sky = hsv_float_t{210.0f, 0.8f, 0.9f};            // H, S, V

    // CMYK: integer channels 0-100
    auto print_teal = cmyk8_t{50, 30, 0, 20};

    // Oklab / OkLCH
    auto neutral = oklab_t{0.5f, 0.0f, 0.0f};

Plain construction stores values verbatim. For validation use the factories:
``make_checked<T>(...)`` rejects out-of-range channels (a ``throw`` inside a
``constexpr`` expression, so a constant-expression call is a compile error) and
``clamped<T>(...)`` clamps them into range. See :doc:`api_conventions` for the
full boundary policy.


Notes
-----

Numeric literal families are constexpr-friendly.

The literals layer is intentionally curated around common authoring spaces; not
every public core color model has a literal suffix.


References
----------

* ISO C++ Standard - User-defined literals (N3599)
* CSS Color Module Level 4 - Color syntax
