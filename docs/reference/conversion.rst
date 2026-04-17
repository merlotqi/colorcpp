Color space conversion
=======================

Typed color conversion system with automatic routing, compile-time safety and extensible registration architecture.

In colorcpp
------------

* Header: ``include/colorcpp/operations/conversion.hpp``
* Main entry point: ``colorcpp::color_cast<ToColor>(source)``

**System features**:

  * ✅ **Compile time safety**: Unsupported conversions fail at compile time with clear static_assert messages
  * ✅ **Multi-layer routing**: 4-level priority routing system with graceful fallback
  * ✅ **Compile-time graph routing**: Full Dijkstra shortest path algorithm runs during compilation
  * ✅ **Weighted edges**: Expensive conversions can be assigned higher cost for optimal path selection
  * ✅ **Hub architecture**: Hierarchical hub tree for maximum compatibility
  * ✅ **Extensible**: Add new color spaces externally without modifying core library
  * ✅ **constexpr support**: All conversions can be evaluated at compile time
  * ✅ **Zero runtime overhead**: All abstractions resolve directly to function calls with zero indirection
  * ✅ **Compile-time debugging**: Inspect conversion paths, costs and availability at compile time


Routing Architecture
--------------------

colorcpp uses a 4-level priority routing system, evaluated in order:

1. **Identity conversion (cost 0)**
   - When source and destination types are identical, value is returned directly
   - No operations performed, absolute zero cost

2. **Direct registered conversion (cost 1)**
   - Explicitly registered direct conversion edges
   - Highest priority, always preferred over indirect routes

3. **Global graph shortest path (variable cost)**
   - Full weighted graph routing using Dijkstra algorithm
   - Calculates minimal cost path across all registered edges
   - Supports multi-hop routes of arbitrary length
   - All path computation performed entirely at compile time

4. **Hub tree fallback routing (variable cost)**
   - Graceful fallback when graph routing is not available
   - Routes via hierarchical hub tree structure
   - Guaranteed connectivity for all built-in color spaces


Hub Tree Structure
------------------

All color spaces are arranged in a hierarchical hub tree:

.. code-block:: text

                        XYZ (root hub)
                       / | \
                      /  |  \
              Linear RGB  OkLab  CIELAB
                 |        |        |
                sRGB     OkLCH   CIELCH
                / | \
         HSL HSV HWB CMYK

        Display P3 → Linear RGB → XYZ

Each color space only requires a single conversion to its parent hub,
automatically enabling conversion to every other color space in the tree.


Conversion Registration
-----------------------

Conversion registration macros:

  * ``COLORCPP_REGISTER_CONVERSION(From, To, function)``
    Register single direction conversion with default cost 1

  * ``COLORCPP_REGISTER_CONVERSION_WEIGHTED(From, To, function, cost)``
    Register conversion with custom weight for path selection

  * ``COLORCPP_REGISTER_CONVERSION_BIDIR(A, B, a_to_b, b_to_a)``
    Register bidirectional conversion with default cost 1

  * ``COLORCPP_REGISTER_CONVERSION_BIDIR_WEIGHTED(A, B, a_to_b, b_to_a, cost_ab, cost_ba)``
    Register bidirectional conversion with individual weights

All registered conversions are automatically included in the global routing graph
and will be considered for shortest path calculation.


Extending The System
--------------------

To add a custom color space:

1. Specialize ``color_traits`` for your color model
2. Register conversion edges to existing hub spaces
3. Optionally add your type to ``additional_color_nodes`` for full graph routing

.. code-block:: cpp

    // Add custom color to global graph
    template <>
    struct colorcpp::operations::conversion::graph::additional_color_nodes {
      using type = node_list<my_custom_color_t>;
    };

    // Register conversion edge
    COLORCPP_REGISTER_CONVERSION(my_custom_color_t, xyz_t, my_custom_to_xyz);

Your color space will automatically be able to convert to and from all
existing color spaces without any additional code.


Debugging & Inspection
----------------------

Compile-time debugging utilities:

.. code-block:: cpp

    // Check if conversion is possible
    constexpr bool possible = colorcpp::can_convert<From, To>();

    // Get detailed conversion path information
    using info = colorcpp::operations::conversion::conversion_path_info<From, To>;

    // Verify path at compile time
    static_assert(colorcpp::operations::conversion::verify_path<hsl_t, oklab_t>());

    // Get hub type for any color
    using hub = colorcpp::operations::conversion::get_hub_t<color_type>;


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
* Alpha channels are automatically preserved when converting between formats
* Channel bit depth conversion is handled automatically with correct rounding


References
----------

* `IEC 61966-2-1:1999 sRGB Standard <https://webstore.iec.ch/en/publication/6169>`__
* `CIE 15:2018 Colorimetry <https://cie.co.at/publications/cie-0152018-colorimetry-4th-edition>`__
* `Oklab Color Space <https://bottosson.github.io/posts/oklab/>`__
* `ICC Profile Specification <https://www.color.org/index.xalter>`__
