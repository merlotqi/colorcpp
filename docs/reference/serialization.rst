Serialization
==============

Overview
--------

Serialization provides an adapter toolkit for converting color objects to and from structured data formats such as
JSON and MessagePack. The core library stays dependency-free: users specialize adapter traits for their chosen
serialization library.

In colorcpp
-----------

**Headers:** ``include/colorcpp/io/serialization.hpp``
**Namespace:** ``colorcpp::io::serialization``

Main entry points:

- ``to_json<Json>(color)`` — serialize to JSON in compact array mode
- ``to_json<Json>(color, opts)`` — serialize to JSON in compact or generic named mode
- ``to_json<Json>(color, channel_names, opts)`` — serialize to JSON with caller-provided names
- ``from_json<Json, Color>(json)`` — deserialize from JSON array or object input
- ``from_json<Json, Color>(json, channel_names)`` — deserialize from JSON with caller-provided names
- ``pack_color(packer, color)`` — serialize to a MessagePack array stream
- ``pack_color_named(packer, color, names)`` — serialize to a MessagePack map with caller-provided keys
- ``unpack_color<Color>(unpacker)`` — deserialize from a MessagePack array stream
- ``unpack_color_named<Color>(unpacker, names)`` — deserialize from a MessagePack map with caller-provided keys

Adapter Pattern
~~~~~~~~~~~~~~~

Users must specialize ``json_adapter<Json>`` for their JSON library. The adapter must provide:

- ``json_type`` type alias
- ``make_array()`` / ``make_object()``
- ``push_back(arr, double)``
- ``set(obj, key, value)``
- ``is_array()`` / ``is_object()`` / ``array_size()``
- ``get_double(obj, index)`` / ``get_double(obj, key)``
- ``has_key(obj, key)``

Similarly for ``msgpack_packer<Packer>`` and ``msgpack_unpacker<Unpacker>``.

Supported surface
~~~~~~~~~~~~~~~~~

The current module is intentionally smaller than a turnkey serializer:

- **JSON**: adapter-based helpers for compact arrays and named objects
- **MessagePack**: lower-level packer/unpacker helpers for array and map flows
- **Binary**: not provided by this module; binary LUT I/O lives under ``colorcpp::io::binary_io``

Serialization modes
~~~~~~~~~~~~~~~~~~~

* **Compact array mode**: ``[0.2, 0.5, 0.8, 1.0]``
  * Smallest output size
  * Standard order by channel index
  * Fastest serialization/deserialization

* **Named object mode**: ``{"ch0": 0.2, "ch1": 0.5, "ch2": 0.8, "ch3": 1.0}``
  * Human readable
  * Uses generic channel keys by default
  * Custom channel names can be supplied by the caller for both JSON and MessagePack map output

Notes
-----

* 8-bit integer channels are automatically normalized to [0, 1] float range
* Alpha channel is automatically included when present
* Adapter pattern allows integration with any JSON / MessagePack library
* Compile-time validation checks whether an adapter specialization is complete
* Zero external dependencies in the core library
* MessagePack support is currently helper-based rather than options-driven parity with JSON

References
----------

* `JSON Specification <https://www.json.org/>`_
* `MessagePack Specification <https://msgpack.org/>`_
* `RFC 8259 - The JavaScript Object Notation (JSON) Data Interchange Format <https://datatracker.ietf.org/doc/html/rfc8259>`_
