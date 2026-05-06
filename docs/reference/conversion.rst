Color space conversion
=======================

Typed color conversion system with compile-time graph routing, safety checks, and registered conversion edges.

In colorcpp
------------

* Header: ``include/colorcpp/operations/conversion.hpp``
* Main entry point: ``colorcpp::operations::conversion::color_cast<ToColor>(source)``

**System features**:

  * ✅ **Compile time safety**: Unsupported conversions fail at compile time with clear static_assert messages
  * ✅ **Compile-time graph routing**: Full Dijkstra shortest path algorithm runs during compilation
  * ✅ **Weighted edges**: Expensive conversions can be assigned higher cost for optimal path selection
  * ✅ **Registered edge extensibility**: Stable public extension focuses on adding direct conversion edges
  * ✅ **constexpr support**: All conversions can be evaluated at compile time
  * ✅ **Zero runtime overhead**: All abstractions resolve directly to function calls with zero indirection
  * ✅ **Compile-time debugging**: Inspect conversion paths, costs and availability at compile time


Routing Architecture
--------------------

colorcpp routes conversions in this order:

1. **Identity conversion (cost 0)**
   - When source and destination types are identical, value is returned directly
   - No operations performed, absolute zero cost

2. **Direct registered conversion (registered edge cost)**
   - Explicitly registered direct conversion edges
   - Participates in the weighted shortest-path graph using its registered edge cost

3. **Global graph shortest path (variable cost)**
   - Full weighted graph routing using Dijkstra algorithm
   - Calculates minimal cost path across all registered edges
   - Supports multi-hop routes of arbitrary length
   - All path computation performed entirely at compile time

4. **Compile-time error if no graph path exists**
   - Unsupported conversions fail during compilation with a clear ``static_assert``
   - Register direct edges to extend the stable public contract


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

The stable public contract documented here focuses on built-in graph routing plus
registered direct conversion edges.

For downstream extensions, the supported workflow today is to register direct
conversion edges for the types you control. More advanced graph-node extension
hooks exist internally, but their downstream ergonomics are not yet a streamlined
part of the stable public API.

For example:

1. Specialize ``color_traits`` for your color model
2. Register direct conversion edges for the routes you want to support

.. code-block:: cpp

    // Register outward conversion edge
    COLORCPP_REGISTER_CONVERSION(my_custom_color_t, xyz_t, my_custom_to_xyz);

    // Register reverse edge too if callers need the opposite direction
    COLORCPP_REGISTER_CONVERSION(xyz_t, my_custom_color_t, xyz_to_my_custom);

With the registrations you provide, your color space becomes available along
the directed graph paths those edges make reachable. Conversions in the opposite
direction require their own registered edges unless an existing path already covers them.


Debugging & Inspection
----------------------

Compile-time debugging utilities:

.. code-block:: cpp

    // Check if conversion is possible
    constexpr bool possible = colorcpp::operations::conversion::can_convert<From, To>();

    // Get detailed conversion path information
    using info = colorcpp::operations::conversion::conversion_path_info<From, To>;
    static_assert(info::has_graph_path);
    static_assert(info::minimal_graph_cost < colorcpp::operations::conversion::graph::inf);

    // Verify path at compile time
    static_assert(colorcpp::operations::conversion::verify_path<hsl_float_t, oklab_t>());


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
