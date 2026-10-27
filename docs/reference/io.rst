Stream I/O for colors
=====================

Text **input/output** for ``basic_color`` types: human-readable tuples with optional **model prefix**, or **hex** via ``std::hex`` conventions as documented in the header.

In colorcpp
------------

* Header: ``include/colorcpp/core/io.hpp``
* Namespace: ``colorcpp::core::io``

Notes
-----

* This is **generic stream formatting**, not a full CSS parser; for **CSS strings** use :doc:`css_color`.
* Channel printing may promote small integer types for readable decimal output (see implementation).
* **Output**: ``prefix(ch0, ch1, …)`` using the model prefix. With ``std::hex`` set, **RGB-family** models
  (``rgb``, ``linear_rgb``, ``adobe_rgb``, ``display_p3``, ``prophoto_rgb``, ``rec2020``) print ``#RRGGBB(AA)``
  with channel bytes rounded to nearest; every other model keeps the text form.
* **Input**: optional prefix and comma/space-separated channel values. For alpha models a missing trailing
  **alpha** defaults to max (opaque); a missing non-alpha channel sets ``failbit``. A channel value outside the
  model's ``[min, max]`` range sets ``failbit`` — no exception is thrown, so parsing is safe under
  ``-fno-exceptions``.
* The stream operators live in ``colorcpp::core`` and are found by ADL, so ``os << c`` works from any user
  namespace. Channel access by tag or index uses ``colorcpp::core::get``; structured bindings decompose the
  channels (``auto [r, g, b, a] = c;``).

References
----------

* C++ iostreams — `std::istream` / `std::ostream` (`cppreference <https://en.cppreference.com/w/cpp/io>`__)
* Hex RGB conventions align loosely with common ``#RRGGBB`` culture; exact CSS compatibility is the responsibility of :doc:`css_color`.
