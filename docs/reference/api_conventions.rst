Public API Conventions
======================

This page defines the namespace and header layout used by the public documentation and examples.

In colorcpp
-----------

* Umbrella header: ``<colorcpp/colorcpp.hpp>``
* Canonical namespaces:

  * ``colorcpp::core`` — color types, constants, stream I/O
  * ``colorcpp::operations`` — conversion, blend, compare, interpolate, palette, random
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

Value initialization
--------------------

Colors are value types with contiguous channel storage. Default construction zero-initializes **every**
channel, so ``rgbaf_t{}`` is transparent black (``r = g = b = 0``, ``a = 0``) — not opaque black. When you
need an opaque default, construct it explicitly (e.g. ``rgbaf_t{0.f, 0.f, 0.f, 1.f}``) or set alpha
afterwards.

Construction and boundary policy
--------------------------------

Colors are plain values: the constructor and ``set<Tag>`` store channel values verbatim and never
validate or throw (integral value types narrow with a ``static_cast``). The nominal per-channel range
(``Channel::min`` / ``Channel::max``) drives normalization, stream I/O, and the opt-in factories — it
is not enforced at construction:

* **Stream input** is the text boundary: ``operator>>`` reads into a temporary and assigns only on
  success; an out-of-range channel or a missing non-alpha channel sets ``failbit`` and leaves the
  destination unchanged. It never throws (``-fno-exceptions`` compatible).
* **``clamped<Color>(…)``** constructs with every channel clamped into its nominal range, rounding
  to nearest for integral channels.
* **``make_checked<Color>(…)``** validates before any narrowing conversion and throws
  ``std::out_of_range`` — for callers that can use exceptions.

.. code-block:: cpp

   rgb8_t a{300, 0, 0};                   // stores r = 44 (narrowing cast, no error)
   auto b = clamped<rgb8_t>(300, 0, 0);    // r = 255
   // make_checked<rgb8_t>(300, 0, 0);     // throws std::out_of_range (checked pre-narrowing)

   rgb8_t c{};
   std::istringstream in("300 0 0");
   in >> c;                               // failbit set; c stays {}

Hue channels declare ``[0, 360]`` and construction stores the angle as given — ``oklch_t{0.5f, 0.1f,
360.0f}`` keeps ``h() == 360.0f``. ``0°`` and ``360°`` denote the same hue (CSS Color 4), but exact
``==`` compares stored channel values, so hues that differ only by the wrap point compare unequal.
Hue-aware operations (interpolation, harmony, rotation) normalize angles internally.

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
