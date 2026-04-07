Color space conversion
=======================

Typed color conversion system with automatic routing, compile-time safety and extensible registration architecture.

In colorcpp
------------

* Header: ``include/colorcpp/operations/conversion.hpp``
* Main entry point: ``colorcpp::color_cast<ToColor>(source)``

**System features**:

  * ✅ **Compile time safety**: Unsupported conversions fail at compile time with clear static_assert messages
  * ✅ **Automatic routing**: Direct conversions used when available, otherwise routes via hub spaces
  * ✅ **Hub architecture**: Linear sRGB → XYZ → Oklab conversion graph
  * ✅ **Extensible**: Add new color spaces externally without modifying core library
  * ✅ **constexpr support**: All conversions can be evaluated at compile time
  * ✅ **No runtime overhead**: Zero cost abstractions resolve to direct function calls


Conversion registration macros:

  * ``COLORCPP_REGISTER_CONVERSION(From, To, function)`` - Register single direction conversion
  * ``COLORCPP_REGISTER_CONVERSION_BIDIR(A, B, a_to_b, b_to_a)`` - Register bidirectional conversion
  * All registered conversions are automatically included in routing graph


Supported color space conversions matrix:

| ↓ From \ To → | RGB | Linear RGB | HSL | HSV | HWB | CMYK | XYZ | LAB | Oklab | OkLCH | Display P3 |
|---------------|-----|------------|-----|-----|-----|------|-----|-----|-------|-------|------------|
| RGB           | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Linear RGB    | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| HSL           | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| HSV           | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| HWB           | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| CMYK          | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| XYZ           | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| CIELAB        | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Oklab         | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| OkLCH         | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Display P3    | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |


Notes
-----

* All conversions use D65 standard illuminant
* sRGB transfer curve follows IEC 61966-2-1 piecewise gamma
* Clamping is applied to destination channel ranges by default
* Conversions are lossless where mathematically possible
* Path selection always chooses the shortest available route

References
----------

* `IEC 61966-2-1:1999 sRGB Standard <https://webstore.iec.ch/en/publication/6169>`__
* `CIE 15:2018 Colorimetry <https://cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`__
* `Oklab Color Space <https://bottosson.github.io/posts/oklab/>`__
* `ICC Profile Specification <https://www.color.org/index.xalter>`__
