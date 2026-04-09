Public API Conventions
======================

This page defines the namespace and header layout used by the public documentation and examples.

In colorcpp
-----------

* Umbrella header: ``<colorcpp/colorcpp.hpp>``
* Canonical namespaces:

  * ``colorcpp::core`` — color types, constants, stream I/O
  * ``colorcpp::operations`` — conversion, blend, compare, flow, interpolate, palette, random
  * ``colorcpp::algorithms`` — accessibility, color temperature, delta_e, gamut, gradient, harmony, vision
  * ``colorcpp::io`` — css, literals, serialization, binary_io, ansi

Canonical usage
---------------

The docs prefer the canonical nested namespaces above, even when the umbrella header is used.

Example:

.. code-block:: cpp

   #include <colorcpp/colorcpp.hpp>

   using namespace colorcpp::core;
   using namespace colorcpp::operations::conversion;
   using namespace colorcpp::algorithms::gamut;
   using namespace colorcpp::io::css;

   auto red = rgba8_t{255, 0, 0, 255};
   auto lab = color_cast<oklab_t>(red);
   auto clipped = gamut_clip(lab);
   auto parsed = parse_css_color_rgba8("coral");

Convenience re-exports
----------------------

When ``<colorcpp/colorcpp.hpp>`` is included, the aggregate headers for ``core``, ``operations``, and ``algorithms``
also provide convenience re-exports into ``colorcpp`` itself. This keeps existing code such as
``colorcpp::conversion::color_cast`` working.

Those convenience aliases remain supported, but the docs use the canonical nested namespaces above so module ownership
stays explicit.

``colorcpp::io`` is intentionally kept under ``colorcpp::io``; it is **not** flattened into the root namespace.

Header guidance
---------------

* Use ``<colorcpp/colorcpp.hpp>`` for quick start integrations, examples, and prototypes.
* Use focused module headers such as ``<colorcpp/operations/conversion.hpp>`` or ``<colorcpp/io/css.hpp>`` when you
  want a narrower include surface in application or library code.
* Use the ``include/colorcpp/...`` paths shown in these docs as the stable public include layout.
