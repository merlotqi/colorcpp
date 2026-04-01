Binary IO (LUT)
================

Overview
--------

Binary IO provides support for reading and writing industry-standard color lookup table (LUT) files. Currently supports the DaVinci Resolve ``.cube`` format (1D and 3D).

In colorcpp
-----------

**Headers:** ``include/colorcpp/io/binary_io.hpp``

Data Structures
~~~~~~~~~~~~~~~~

- ``binary_io::lut1d`` — 1D LUT: independent per-channel mapping with domain min/max
- ``binary_io::lut3d`` — 3D LUT: NxNxN grid with trilinear interpolation

Functions
~~~~~~~~~~

- ``binary_io::apply(lut, r, g, b)`` — apply LUT to an RGB triplet
- ``binary_io::apply_color(lut, color)`` — apply LUT to any color type
- ``binary_io::cube::read_3d(path)`` — read 3D LUT from .cube file
- ``binary_io::cube::read_1d(path)`` — read 1D LUT from .cube file
- ``binary_io::cube::write(path, lut)`` — write LUT to .cube file

Example
--------

.. code-block:: cpp

   #include <colorcpp/io/binary_io.hpp>

   // Read a .cube LUT
   auto lut = binary_io::cube::read_3d("grade.cube");

   // Apply to a color
   auto graded = binary_io::apply(*lut, 0.5f, 0.3f, 0.8f);

   // Write back
   binary_io::cube::write("output.cube", *lut, "My Grade");

Notes
-----

- ``.cube`` format supports TITLE, DOMAIN_MIN/MAX, LUT_1D_SIZE, LUT_3D_SIZE
- 3D LUT uses trilinear interpolation for sub-grid values
- Data ordering: R varies slowest, then G, then B (per .cube spec)

References
----------

- `DaVinci Resolve Cube Format <https://wwwimages2.adobe.com/content/dam/acom/en/products/speedgrade/cc/pdfs/cube-lut-specification-1.0.pdf>`_
