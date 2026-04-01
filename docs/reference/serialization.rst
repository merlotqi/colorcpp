Serialization
==============

Overview
--------

Serialization provides adapters for converting color objects to and from structured data formats (JSON, MessagePack). The library itself has zero external dependencies — users specialize adapter traits for their chosen serialization library.

In colorcpp
-----------

**Headers:** ``include/colorcpp/io/serialization.hpp``

Main entry points:

- ``serialization::to_json<Json>(color)`` — serialize to JSON (compact or named format)
- ``serialization::from_json<Json, Color>(json)`` — deserialize from JSON
- ``serialization::pack_color(packer, color)`` — serialize to MessagePack
- ``serialization::unpack_color<Color>(unpacker)`` — deserialize from MessagePack

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

Formats
~~~~~~~

- **Compact:** ``[r, g, b, a]`` — array of channel values (float, 0–1)
- **Named:** ``{"ch0": r, "ch1": g, ...}`` — object with generic or custom channel names

Notes
-----

- 8-bit channels are normalized to [0, 1] for serialization
- The SFINAE trait ``is_json_adapter_complete`` validates adapter specializations at compile time
- Named colors, lab/lch/hwb parsing, and display-p3 are all supported via the CSS parsing module

References
----------

- `JSON <https://www.json.org/>`_
- `MessagePack <https://msgpack.org/>`_
