Serialization
==============

Overview
--------

Serialization provides adapters for converting color objects to and from structured data formats (JSON, MessagePack). The library itself has zero external dependencies — users specialize adapter traits for their chosen serialization library.

In colorcpp
-----------

**Headers:** ``include/colorcpp/io/serialization.hpp``
**Namespace:** ``colorcpp::io::serialization``

Main entry points:

- ``to_json<Json>(color)`` — serialize to JSON (compact / named / tagged format)
- ``from_json<Json, Color>(json)`` — deserialize from JSON
- ``pack_color(packer, color)`` — serialize to MessagePack stream
- ``unpack_color<Color>(unpacker)`` — deserialize from MessagePack stream

Adapter Pattern
~~~~~~~~~~~~~~~~

Users must specialize ``json_adapter<Json>`` for their JSON library. The adapter must provide:

- ``json_type`` type alias
- ``make_array()`` / ``make_object()``
- ``push_back(arr, double)``
- ``set(obj, key, value)``
- ``is_array()`` / ``is_object()`` / ``array_size()``
- ``get_double(obj, index)`` / ``get_double(obj, key)``
- ``has_key(obj, key)``

Similarly for ``msgpack_packer<Packer>`` and ``msgpack_unpacker<Unpacker>``.


✅ **Supported serialization formats**:

| Format | Status | Implementation |
|--------|--------|----------------|
| JSON | ✅ Complete | Generic adapter architecture |
| MessagePack | ✅ Complete | Streaming packer/unpacker |
| Binary | ✅ Complete | Compact fixed size format |

✅ **Supported color spaces**:
* All core color spaces: RGB, Linear RGB, HSL, HSV, HWB, CMYK
* CIE spaces: XYZ, CIELAB, CIELCH
* Modern spaces: Oklab, OkLCH, Display P3


Serialization modes:

* **Compact array mode**: ``[0.2, 0.5, 0.8, 1.0]``
  * Smallest output size
  * Standard order by channel index
  * Fastest serialization/deserialization

* **Named object mode**: ``{"r": 0.2, "g": 0.5, "b": 0.8, "a": 1.0}``
  * Human readable
  * Self documenting format
  * Channel names match color space definition

* **Type tagged mode**: ``{"space": "oklab", "L": 0.5, "a": 0.1, "b": -0.2}``
  * Includes color space information
  * Self describing format
  * Deserialize without prior type knowledge


Notes
-----

* 8-bit integer channels are automatically normalized to [0, 1] float range
* Alpha channel is automatically included when present
* Adapter pattern allows integration with any JSON / MessagePack library
* Compile time validation of adapter interface completeness
* Zero external dependencies in core library
* Streaming interface supports incremental serialization

References
----------

* `JSON Specification <https://www.json.org/>`_
* `MessagePack Specification <https://msgpack.org/>`_
* `RFC 8259 - The JavaScript Object Notation (JSON) Data Interchange Format <https://datatracker.ietf.org/doc/html/rfc8259>`_
