/**
 * @file fuzz_read_cube.cpp
 * @brief libFuzzer entry for the DaVinci Resolve .cube LUT reader.
 *
 * read()/read_3d() must reject malformed input (variant monostate /
 * std::nullopt), never crash, and never over-read the stream.
 */

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include <colorcpp/colorcpp.hpp>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
  const std::string bytes(reinterpret_cast<const char*>(data), size);

  // Full reader (auto-detects 1D/3D).
  std::istringstream is_full(bytes);
  (void)colorcpp::io::binary_io::cube::read(is_full);

  // Explicit 3D path.
  std::istringstream is_3d(bytes);
  (void)colorcpp::io::binary_io::cube::read_3d(is_3d);

  return 0;
}
