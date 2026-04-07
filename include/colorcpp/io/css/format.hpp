/**
 * @file format.hpp
 * @brief Serialize core colors to CSS Color 4-style @c rgb() / slash-alpha strings.
 */

#pragma once

#include <cmath>
#include <colorcpp/core/rgb.hpp>
#include <iomanip>
#include <sstream>
#include <string>

namespace colorcpp::io::css {

/** @brief CSS @c rgb() / @c rgb(… / …) with space-separated channels (integers 0–255). */
inline std::string to_css_color_string(const core::rgba8_t& c) {
  std::ostringstream os;
  if (c.a() == 255) {
    os << "rgb(" << static_cast<int>(c.r()) << ' ' << static_cast<int>(c.g()) << ' ' << static_cast<int>(c.b()) << ')';
  } else {
    const double a = static_cast<double>(c.a()) / 255.0;
    os << "rgb(" << static_cast<int>(c.r()) << ' ' << static_cast<int>(c.g()) << ' ' << static_cast<int>(c.b()) << " / "
       << std::fixed << std::setprecision(4) << a << ')';
  }
  return os.str();
}

/** @brief CSS @c rgb() with float channels in [0, 1] and optional slash alpha. */
inline std::string to_css_color_string(const core::rgbaf_t& c) {
  std::ostringstream os;
  os << std::fixed << std::setprecision(5);
  os << "rgb(" << c.r() << ' ' << c.g() << ' ' << c.b();
  if (std::fabs(static_cast<double>(c.a()) - 1.0) > 1e-5) {
    os << " / " << c.a();
  }
  os << ')';
  return os.str();
}

}  // namespace colorcpp::io::css
