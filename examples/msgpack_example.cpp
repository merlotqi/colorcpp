/**
 * @file msgpack_example.cpp
 * @brief Example: serializing colors to MessagePack using the serialization module.
 *
 * Demonstrates how to specialize msgpack_packer / msgpack_unpacker for a
 * simple in-memory buffer packer. In production, replace with msgpack-cpp or
 * another MessagePack library.
 */

#include <colorcpp/colorcpp.hpp>
#include <cstring>
#include <iostream>
#include <vector>

using namespace colorcpp;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::serialization;
using namespace colorcpp::io::ansi;

// =============================================================================
// Minimal in-memory MessagePack packer (for demonstration only)
// =============================================================================

struct simple_packer {
  std::vector<uint8_t> buf;

  void pack_nil() { buf.push_back(0xc0); }

  void pack_double(double v) {
    // MessagePack float64: 0xcb + 8 bytes big-endian
    buf.push_back(0xcb);
    uint64_t bits;
    std::memcpy(&bits, &v, sizeof(v));
    for (int i = 7; i >= 0; --i) {
      buf.push_back(static_cast<uint8_t>((bits >> (i * 8)) & 0xff));
    }
  }

  void pack_string(const char* s, std::size_t len) {
    if (len < 32) {
      buf.push_back(static_cast<uint8_t>(0xa0 | len));
    } else if (len < 256) {
      buf.push_back(0xd9);
      buf.push_back(static_cast<uint8_t>(len));
    } else {
      buf.push_back(0xda);
      buf.push_back(static_cast<uint8_t>(len >> 8));
      buf.push_back(static_cast<uint8_t>(len & 0xff));
    }
    buf.insert(buf.end(), s, s + len);
  }

  void pack_array_header(std::size_t n) {
    if (n < 16) {
      buf.push_back(static_cast<uint8_t>(0x90 | n));
    } else if (n < 65536) {
      buf.push_back(0xdc);
      buf.push_back(static_cast<uint8_t>(n >> 8));
      buf.push_back(static_cast<uint8_t>(n & 0xff));
    }
  }

  void pack_map_header(std::size_t n) {
    if (n < 16) {
      buf.push_back(static_cast<uint8_t>(0x80 | n));
    } else if (n < 65536) {
      buf.push_back(0xde);
      buf.push_back(static_cast<uint8_t>(n >> 8));
      buf.push_back(static_cast<uint8_t>(n & 0xff));
    }
  }
};

// =============================================================================
// Minimal in-memory MessagePack unpacker (for demonstration only)
// =============================================================================

struct simple_unpacker {
  const uint8_t* data;
  std::size_t size;
  std::size_t pos = 0;

  uint8_t peek() const { return pos < size ? data[pos] : 0; }

  uint8_t read_u8() { return data[pos++]; }

  uint16_t read_u16() {
    uint16_t v = (static_cast<uint16_t>(data[pos]) << 8) | data[pos + 1];
    pos += 2;
    return v;
  }

  double read_double() {
    pos++;  // skip 0xcb marker
    uint64_t bits = 0;
    for (int i = 0; i < 8; ++i) bits = (bits << 8) | data[pos++];
    double v;
    std::memcpy(&v, &bits, sizeof(v));
    return v;
  }

  std::string read_string() {
    uint8_t marker = peek();
    std::size_t len = 0;
    if ((marker & 0xe0) == 0xa0) {
      len = read_u8() & 0x1f;
    } else if (marker == 0xd9) {
      read_u8();
      len = read_u8();
    } else if (marker == 0xda) {
      read_u8();
      len = read_u16();
    }
    std::string s(reinterpret_cast<const char*>(data + pos), len);
    pos += len;
    return s;
  }

  bool is_array() const {
    uint8_t m = peek();
    return (m & 0xf0) == 0x90 || m == 0xdc;
  }

  bool is_map() const {
    uint8_t m = peek();
    return (m & 0xf0) == 0x80 || m == 0xde;
  }

  std::size_t array_size() {
    uint8_t m = read_u8();
    if ((m & 0xf0) == 0x90) return m & 0x0f;
    if (m == 0xdc) return read_u16();
    return 0;
  }

  std::size_t map_size() {
    uint8_t m = read_u8();
    if ((m & 0xf0) == 0x80) return m & 0x0f;
    if (m == 0xde) return read_u16();
    return 0;
  }

  double read_value_double() {
    uint8_t m = peek();
    if (m == 0xcb) return read_double();
    // fixfloat
    if ((m & 0xe0) == 0xe0) {
      // negative fixint
      return static_cast<double>(static_cast<int8_t>(read_u8()));
    }
    if (m <= 0x7f) return static_cast<double>(read_u8());  // positive fixint
    return 0.0;
  }

  void skip_value() {
    uint8_t m = peek();
    if (m <= 0x7f || m >= 0xe0) {
      pos++;
    } else if (m == 0xcb) {
      pos += 9;
    } else if ((m & 0xe0) == 0xa0) {
      std::size_t len = read_u8() & 0x1f;
      pos += len;
    } else if (m == 0xd9) {
      read_u8();
      std::size_t len = read_u8();
      pos += len;
    } else {
      pos++;
    }
  }
};

// =============================================================================
// Specialize msgpack_packer / msgpack_unpacker
// =============================================================================

template <>
struct colorcpp::io::serialization::msgpack_packer<simple_packer> {
  using packer_type = simple_packer;
  static void pack_array(simple_packer& p, std::size_t n) { p.pack_array_header(n); }
  static void pack_map(simple_packer& p, std::size_t n) { p.pack_map_header(n); }
  static void pack_double(simple_packer& p, double v) { p.pack_double(v); }
  static void pack_string(simple_packer& p, std::string_view s) { p.pack_string(s.data(), s.size()); }
};

template <>
struct colorcpp::io::serialization::msgpack_unpacker<simple_unpacker> {
  using unpacker_type = simple_unpacker;
  static bool is_array(const simple_unpacker& u) { return u.is_array(); }
  static bool is_map(const simple_unpacker& u) { return u.is_map(); }
  static std::size_t array_size(simple_unpacker& u) { return u.array_size(); }
  static double get_double(simple_unpacker& u, std::size_t /*index*/) {
    // Read next element sequentially (position advances with each call)
    return u.read_value_double();
  }
  static double get_double(simple_unpacker& u, std::string_view key) {
    // Scan pairs without consuming map header (caller already read it)
    while (u.pos < u.size) {
      auto k = u.read_string();
      if (k == key) return u.read_value_double();
      u.skip_value();
    }
    return 0.0;
  }
  static bool has_key(simple_unpacker& u, std::string_view key) {
    // Scan pairs without consuming map header
    auto saved = u.pos;
    while (u.pos < u.size) {
      auto k = u.read_string();
      if (k == key) {
        u.pos = saved;
        return true;
      }
      u.skip_value();
    }
    u.pos = saved;
    return false;
  }
};

// =============================================================================
// Usage
// =============================================================================

static void section(const char* title) {
  std::cout << '\n' << bold() << "── " << title << " ──" << reset() << '\n';
}

int main() {
  section("Pack color to MessagePack (array format)");

  rgba8_t coral = constants::coral;
  simple_packer packer;
  pack_color(packer, coral);

  std::cout << "packed " << packer.buf.size() << " bytes: ";
  for (auto b : packer.buf) printf("%02x ", b);
  std::cout << "\n";

  section("Unpack color from MessagePack");

  simple_unpacker unpacker{packer.buf.data(), packer.buf.size()};
  auto recovered = unpack_color<simple_unpacker, rgba8_t>(unpacker);
  if (recovered) {
    std::cout << "recovered: ";
    print_color(std::cout, *recovered);
    std::cout << "  (" << std::hex << *recovered << std::dec << ")\n";
  }

  section("Pack color (named map format)");

  simple_packer packer2;
  std::string names[] = {"r", "g", "b", "a"};
  pack_color_named(packer2, coral, names);

  std::cout << "packed " << packer2.buf.size() << " bytes: ";
  for (auto b : packer2.buf) printf("%02x ", b);
  std::cout << "\n";

  section("Unpack color (named map format)");

  simple_unpacker unpacker2{packer2.buf.data(), packer2.buf.size()};
  auto recovered2 = unpack_color_named<simple_unpacker, rgba8_t>(unpacker2, names);
  if (recovered2) {
    std::cout << "recovered: ";
    print_color(std::cout, *recovered2);
    std::cout << "  (" << std::hex << *recovered2 << std::dec << ")\n";
  }

  return 0;
}
