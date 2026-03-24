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

References
----------

* C++ iostreams — `std::istream` / `std::ostream` (`cppreference <https://en.cppreference.com/w/cpp/io>`__)
* Hex RGB conventions align loosely with common ``#RRGGBB`` culture; exact CSS compatibility is the responsibility of :doc:`css_color`.
