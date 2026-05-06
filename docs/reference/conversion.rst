Color space conversion
=======================

Typed color conversion system with compile-time graph routing, safety checks, and extensible registration.

In colorcpp
------------

* Header: ``include/colorcpp/operations/conversion.hpp``
* Main entry point: ``colorcpp::operations::conversion::color_cast<ToColor>(source)``

**System features**:

  * ✅ **Compile time safety**: Unsupported conversions fail at compile time with clear static_assert messages
  * ✅ **Compile-time graph routing**: Full Dijkstra shortest path algorithm runs during compilation
  * ✅ **Weighted edges**: Expensive conversions can be assigned higher cost for optimal path selection
  * ✅ **Extensible**: Add new color spaces externally without modifying core library
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
   - Register a direct edge or add graph nodes to extend support


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
2. Register conversion edges to existing graph nodes
3. Optionally add your type to ``additional_color_nodes`` for full graph routing

.. code-block:: cpp

    // Add custom color to global graph
    template <>
    struct colorcpp::operations::conversion::graph::additional_color_nodes {
      using type = node_list<my_custom_color_t>;
    };

    // Register conversion edge
    COLORCPP_REGISTER_CONVERSION(my_custom_color_t, xyz_t, my_custom_to_xyz);

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
    static_assert(colorcpp::operations::conversion::verify_path<hsl_t, oklab_t>());


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
