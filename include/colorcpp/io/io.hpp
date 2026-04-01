/**
 * @file io.hpp
 * @brief Aggregates color I/O: CSS color parsing, user-defined literals, binary LUT formats, serialization
 * (JSON/MessagePack), ANSI terminal output.
 */

#pragma once

#include <colorcpp/io/ansi.hpp>
#include <colorcpp/io/binary_io.hpp>
#include <colorcpp/io/css.hpp>
#include <colorcpp/io/literals.hpp>
#include <colorcpp/io/serialization.hpp>

/**
 * @namespace colorcpp::io
 * @brief Color I/O utilities: @c css, @c literals, @c binary_io, @c serialization, @c ansi (each is a nested
 * namespace).
 */
