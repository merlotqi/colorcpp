Binary IO (LUT)
================

Overview
--------

Binary IO provides LUT data structures, LUT application helpers, and file IO for the DaVinci Resolve ``.cube`` format.
Only ``.cube`` reader/writer support is currently implemented (1D and 3D).

In colorcpp
-----------

**Headers:** ``include/colorcpp/io/binary_io.hpp``

Data Structures
~~~~~~~~~~~~~~~~

- ``binary_io::lut1d`` — 1D LUT: independent per-channel mapping with domain min/max
- ``binary_io::lut3d`` — 3D LUT: NxNxN grid with trilinear interpolation

Functions
~~~~~~~~~~

- ``binary_io::apply(lut, r, g, b)`` — apply LUT to a normalized RGB triplet
- ``binary_io::apply_color(lut, color)`` — apply LUT to any color type convertible through ``core::rgbaf_t`` via ``operations::conversion::color_cast``
- ``binary_io::cube::read(is|path)`` — auto-detect 1D/3D ``.cube`` LUT
- ``binary_io::cube::read_3d(is|path)`` — read 3D ``.cube`` LUT
- ``binary_io::cube::read_1d(is|path)`` — read 1D ``.cube`` LUT
- ``binary_io::cube::write(os|path, lut, title)`` — write 1D or 3D ``.cube`` LUT

Example
--------

.. code-block:: cpp

   #include <colorcpp/io/binary_io.hpp>

   // Read a .cube LUT
   auto lut = binary_io::cube::read_3d("grade.cube");

   // Apply in normalized RGB space
   auto graded = binary_io::apply(*lut, 0.5f, 0.3f, 0.8f);

   // Write back
   binary_io::cube::write("output.cube", *lut, "My Grade");

Notes
-----

- ``.cube`` format supports TITLE, DOMAIN_MIN/MAX, LUT_1D_SIZE, LUT_3D_SIZE
- ``apply()`` and ``apply_color()`` evaluate LUTs in normalized RGB space
- ``apply_color()`` converts input -> ``core::rgbaf_t`` -> output type using ``operations::conversion::color_cast``
- Invalid LUTs (size/data mismatch or empty) are passthrough and return input unchanged
- A valid single-cell 3D LUT (size ``== 1``) returns its only stored RGB triplet
- 3D LUT uses trilinear interpolation for sub-grid values
- Data ordering: R varies slowest, then G, then B (per .cube spec)

References
----------

- `DaVinci Resolve Cube Format <https://wwwimages2.adobe.com/content/dam/acom/en/products/speedgrade/cc/pdfs/cube-lut-specification-1.0.pdf>`_
