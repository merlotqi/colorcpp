/**
 * @file serialization.hpp
 * @brief Aggregate header for color serialization (JSON, MessagePack).
 *
 * Provides template adapters for serializing/deserializing color objects
 * to/from JSON and MessagePack formats. Users specialize the adapter traits
 * for their specific library.
 */

#pragma once

#include <colorcpp/io/serialization/json.hpp>
#include <colorcpp/io/serialization/msgpack.hpp>
