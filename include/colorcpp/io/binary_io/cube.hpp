/**
 * @file cube.hpp
 * @brief DaVinci Resolve .cube LUT file format reader and writer.
 *
 * Supports both 1D and 3D LUTs in the DaVinci Resolve .cube format.
 * See: https://www.daVinciresolve.com for format specification.
 *
 * Format summary:
 * - Comments: lines starting with #
 * - TITLE "name" (optional)
 * - LUT_1D_SIZE N or LUT_3D_SIZE N
 * - DOMAIN_MIN r g b (optional)
 * - DOMAIN_MAX r g b (optional)
 * - Data lines: r g b (one per line, values in domain range)
 */

#pragma once

#include <colorcpp/io/binary_io/details.hpp>
#include <colorcpp/io/binary_io/lut.hpp>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <variant>

namespace colorcpp::io::binary_io::cube {

/**
 * @brief Read a .cube file, auto-detecting 1D or 3D LUT.
 * @param is Input stream.
 * @return lut1d, lut3d, or std::monostate on error.
 */
inline std::variant<lut1d, lut3d, std::monostate> read(std::istream& is) {
  std::optional<int> lut_1d_size;
  std::optional<int> lut_3d_size;
  std::optional<std::string> title;
  std::array<float, 3> domain_min = {0.0f, 0.0f, 0.0f};
  std::array<float, 3> domain_max = {1.0f, 1.0f, 1.0f};
  std::vector<std::array<float, 3>> data;

  std::string line;
  while (std::getline(is, line)) {
    // Remove trailing \r
    if (!line.empty() && line.back() == '\r') line.pop_back();

    std::string_view sv(line);
    details::trim(sv);

    // Skip empty lines and comments
    if (sv.empty() || sv[0] == '#') continue;

    // Parse TITLE
    if (details::starts_with_ci(sv, "TITLE")) {
      sv.remove_prefix(5);
      details::trim(sv);
      // Remove surrounding quotes
      if (sv.size() >= 2 && sv.front() == '"' && sv.back() == '"') {
        sv.remove_prefix(1);
        sv.remove_suffix(1);
      }
      title = std::string(sv);
      continue;
    }

    // Parse LUT_1D_SIZE
    if (details::starts_with_ci(sv, "LUT_1D_SIZE")) {
      sv.remove_prefix(11);
      details::trim(sv);
      auto val = details::parse_int(sv);
      if (val && *val > 0) lut_1d_size = *val;
      continue;
    }

    // Parse LUT_3D_SIZE
    if (details::starts_with_ci(sv, "LUT_3D_SIZE")) {
      sv.remove_prefix(11);
      details::trim(sv);
      auto val = details::parse_int(sv);
      if (val && *val > 0) lut_3d_size = *val;
      continue;
    }

    // Parse DOMAIN_MIN
    if (details::starts_with_ci(sv, "DOMAIN_MIN")) {
      sv.remove_prefix(10);
      details::trim(sv);
      auto tokens = details::split_ws(sv);
      if (tokens.size() >= 3) {
        auto r = details::parse_float(tokens[0]);
        auto g = details::parse_float(tokens[1]);
        auto b = details::parse_float(tokens[2]);
        if (r && g && b) {
          domain_min = {*r, *g, *b};
        }
      }
      continue;
    }

    // Parse DOMAIN_MAX
    if (details::starts_with_ci(sv, "DOMAIN_MAX")) {
      sv.remove_prefix(10);
      details::trim(sv);
      auto tokens = details::split_ws(sv);
      if (tokens.size() >= 3) {
        auto r = details::parse_float(tokens[0]);
        auto g = details::parse_float(tokens[1]);
        auto b = details::parse_float(tokens[2]);
        if (r && g && b) {
          domain_max = {*r, *g, *b};
        }
      }
      continue;
    }

    // Parse data line: r g b
    auto tokens = details::split_ws(sv);
    if (tokens.size() >= 3) {
      auto r = details::parse_float(tokens[0]);
      auto g = details::parse_float(tokens[1]);
      auto b = details::parse_float(tokens[2]);
      if (r && g && b) {
        data.push_back({*r, *g, *b});
      }
    }
  }

  // Build LUT based on detected size
  if (lut_3d_size) {
    lut3d lut;
    lut.size = static_cast<std::size_t>(*lut_3d_size);
    lut.domain_min = domain_min;
    lut.domain_max = domain_max;
    if (title) lut.title = *title;
    lut.data = std::move(data);
    if (lut.valid()) return lut;
  }

  if (lut_1d_size) {
    lut1d lut;
    lut.size = static_cast<std::size_t>(*lut_1d_size);
    lut.domain_min = domain_min;
    lut.domain_max = domain_max;
    if (title) lut.title = *title;

    // Split interleaved data into per-channel arrays
    lut.r.reserve(lut.size);
    lut.g.reserve(lut.size);
    lut.b.reserve(lut.size);
    for (const auto& entry : data) {
      lut.r.push_back(entry[0]);
      lut.g.push_back(entry[1]);
      lut.b.push_back(entry[2]);
    }
    if (lut.valid()) return lut;
  }

  return std::monostate{};
}

/**
 * @brief Read a .cube file from a file path.
 */
inline std::variant<lut1d, lut3d, std::monostate> read(const std::filesystem::path& path) {
  std::ifstream file(path);
  if (!file.is_open()) return std::monostate{};
  return read(file);
}

/**
 * @brief Read only a 3D LUT from a .cube file.
 */
inline std::optional<lut3d> read_3d(std::istream& is) {
  auto result = read(is);
  if (auto* lut = std::get_if<lut3d>(&result)) return std::move(*lut);
  return std::nullopt;
}

/**
 * @brief Read only a 3D LUT from a .cube file path.
 */
inline std::optional<lut3d> read_3d(const std::filesystem::path& path) {
  std::ifstream file(path);
  if (!file.is_open()) return std::nullopt;
  return read_3d(file);
}

/**
 * @brief Read only a 1D LUT from a .cube file.
 */
inline std::optional<lut1d> read_1d(std::istream& is) {
  auto result = read(is);
  if (auto* lut = std::get_if<lut1d>(&result)) return std::move(*lut);
  return std::nullopt;
}

/**
 * @brief Read only a 1D LUT from a .cube file path.
 */
inline std::optional<lut1d> read_1d(const std::filesystem::path& path) {
  std::ifstream file(path);
  if (!file.is_open()) return std::nullopt;
  return read_1d(file);
}

/**
 * @brief Write a 3D LUT to a .cube file.
 * @param os Output stream.
 * @param lut The 3D LUT to write.
 * @param title Optional title.
 * @return true on success.
 */
inline bool write(std::ostream& os, const lut3d& lut, std::string_view title = "") {
  if (!lut.valid()) return false;

  if (!title.empty()) {
    os << "TITLE \"" << title << "\"\n";
  } else if (!lut.title.empty()) {
    os << "TITLE \"" << lut.title << "\"\n";
  }

  // Write domain if not default
  if (lut.domain_min[0] != 0.0f || lut.domain_min[1] != 0.0f || lut.domain_min[2] != 0.0f) {
    os << "DOMAIN_MIN " << lut.domain_min[0] << " " << lut.domain_min[1] << " " << lut.domain_min[2] << "\n";
  }
  if (lut.domain_max[0] != 1.0f || lut.domain_max[1] != 1.0f || lut.domain_max[2] != 1.0f) {
    os << "DOMAIN_MAX " << lut.domain_max[0] << " " << lut.domain_max[1] << " " << lut.domain_max[2] << "\n";
  }

  os << "LUT_3D_SIZE " << lut.size << "\n";

  // .cube format: R varies slowest, then G, then B
  for (std::size_t r = 0; r < lut.size; ++r) {
    for (std::size_t g = 0; g < lut.size; ++g) {
      for (std::size_t b = 0; b < lut.size; ++b) {
        const auto& c = lut.at(r, g, b);
        os << c[0] << " " << c[1] << " " << c[2] << "\n";
      }
    }
  }

  return os.good();
}

/**
 * @brief Write a 1D LUT to a .cube file.
 * @param os Output stream.
 * @param lut The 1D LUT to write.
 * @param title Optional title.
 * @return true on success.
 */
inline bool write(std::ostream& os, const lut1d& lut, std::string_view title = "") {
  if (!lut.valid()) return false;

  if (!title.empty()) {
    os << "TITLE \"" << title << "\"\n";
  } else if (!lut.title.empty()) {
    os << "TITLE \"" << lut.title << "\"\n";
  }

  if (lut.domain_min[0] != 0.0f || lut.domain_min[1] != 0.0f || lut.domain_min[2] != 0.0f) {
    os << "DOMAIN_MIN " << lut.domain_min[0] << " " << lut.domain_min[1] << " " << lut.domain_min[2] << "\n";
  }
  if (lut.domain_max[0] != 1.0f || lut.domain_max[1] != 1.0f || lut.domain_max[2] != 1.0f) {
    os << "DOMAIN_MAX " << lut.domain_max[0] << " " << lut.domain_max[1] << " " << lut.domain_max[2] << "\n";
  }

  os << "LUT_1D_SIZE " << lut.size << "\n";

  for (std::size_t i = 0; i < lut.size; ++i) {
    os << lut.r[i] << " " << lut.g[i] << " " << lut.b[i] << "\n";
  }

  return os.good();
}

/**
 * @brief Write a 3D LUT to a .cube file path.
 */
inline bool write(const std::filesystem::path& path, const lut3d& lut, std::string_view title = "") {
  std::ofstream file(path);
  if (!file.is_open()) return false;
  return write(file, lut, title);
}

/**
 * @brief Write a 1D LUT to a .cube file path.
 */
inline bool write(const std::filesystem::path& path, const lut1d& lut, std::string_view title = "") {
  std::ofstream file(path);
  if (!file.is_open()) return false;
  return write(file, lut, title);
}

}  // namespace colorcpp::io::binary_io::cube
