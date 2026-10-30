/**
 * @file fuzz_read_cube.cpp
 * @brief libFuzzer entry for the DaVinci Resolve .cube LUT reader.
 *
 * read()/read_3d() must reject malformed input (variant monostate /
 * std::nullopt), never crash, and never over-read the stream. Accepted LUTs
 * are then exercised through apply() with hostile float inputs (NaN, ±inf,
 * out-of-domain), because a reader-accepted LUT reaching apply()'s
 * float→size_t index cast was the #64 undefined-behavior path.
 */

#include <colorcpp/colorcpp.hpp>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>

namespace {

// NaN/inf/out-of-range probes in every channel slot: apply() must map them
// to defined values (clamp01's NaN policy) before any index arithmetic.
template <typename Lut>
void exercise_apply(const Lut& lut) {
  const float nan = std::numeric_limits<float>::quiet_NaN();
  const float inf = std::numeric_limits<float>::infinity();
  for (float v : {0.0f, 0.5f, 1.0f, -1.0f, 2.0f, nan, inf, -inf}) {
    (void)colorcpp::io::binary_io::apply(lut, v, 0.5f, 0.5f);
    (void)colorcpp::io::binary_io::apply(lut, 0.5f, v, 0.5f);
    (void)colorcpp::io::binary_io::apply(lut, 0.5f, 0.5f, v);
    (void)colorcpp::io::binary_io::apply(lut, v, v, v);
  }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
  const std::string bytes(reinterpret_cast<const char*>(data), size);

  // Full reader (auto-detects 1D/3D), then apply() on anything accepted.
  std::istringstream is_full(bytes);
  auto lut = colorcpp::io::binary_io::cube::read(is_full);
  if (const auto* l3 = std::get_if<colorcpp::io::binary_io::lut3d>(&lut)) {
    exercise_apply(*l3);
  } else if (const auto* l1 = std::get_if<colorcpp::io::binary_io::lut1d>(&lut)) {
    exercise_apply(*l1);
  }

  // Explicit 3D path.
  std::istringstream is_3d(bytes);
  if (auto lut3d_opt = colorcpp::io::binary_io::cube::read_3d(is_3d)) {
    exercise_apply(*lut3d_opt);
  }

  return 0;
}
