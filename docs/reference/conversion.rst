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

Custom color types can join the conversion graph through two stable hooks:

1. **Direct registered edges** — make ``color_cast`` work between your type and
   any type it is registered with.
2. **Graph node registration** — add your type as a full graph node so
   multi-hop shortest-path routing works automatically in both directions.

Registration macros open the conversion namespace themselves and may be used at
global scope:

  * ``COLORCPP_REGISTER_CONVERSION(From, To, function)``
  * ``COLORCPP_REGISTER_CONVERSION_WEIGHTED(From, To, function, cost)``

Include order matters: the node set must be configured **before** any header
that runs concrete conversions at namespace scope (for example
``operations/palette/theme.hpp`` or the ``io/css`` headers) is included —
otherwise the graph is locked to the built-in nodes. The conversion
infrastructure header alone never locks it:

.. code-block:: cpp

    // 1. Graph infrastructure only — safe to configure afterwards
    #include <colorcpp/operations/conversion.hpp>

    struct my_uv { float u, v; };

    my_uv rgbf_to_my_uv(const colorcpp::core::rgbf_t& c);
    colorcpp::core::xyz_t my_uv_to_xyz(const my_uv& c);

    // 2. Add the type as a graph node (multi-hop participation)
    template <>
    struct colorcpp::operations::conversion::graph::additional_color_nodes<> {
      using type = colorcpp::operations::conversion::graph::node_list<my_uv>;
    };

    // 3. Register the conversion edges (weighted edges steer route choice)
    COLORCPP_REGISTER_CONVERSION(colorcpp::core::rgbf_t, my_uv, rgbf_to_my_uv);
    COLORCPP_REGISTER_CONVERSION(my_uv, colorcpp::core::rgbf_t, my_uv_to_rgbf);
    COLORCPP_REGISTER_CONVERSION_WEIGHTED(my_uv, colorcpp::core::xyz_t, my_uv_to_xyz, 4);

    // 4. Rest of the library — routes now include my_uv automatically
    #include <colorcpp/colorcpp.hpp>

With the node registered, ``color_cast<xyz_t>(my_uv)`` routes through the
weighted graph (here preferring the cheaper ``my_uv → rgbf → linear rgb → xyz``
chain over the cost-4 direct edge), and casts in the reverse direction work
with no extra registration.


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
