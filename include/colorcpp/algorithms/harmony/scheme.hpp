/**
 * @file scheme.hpp
 * @brief Harmony scheme enumeration.
 */

#pragma once

#include <string>

namespace colorcpp::algorithms::harmony {

/**
 * @brief Color harmony schemes.
 */
enum class harmony_scheme {
  // Special cases
  single,  ///< Single color (no harmony)
  pair,    ///< Two colors without recognized pattern
  triad,   ///< Three colors without recognized pattern
  tetrad,  ///< Four colors without recognized pattern
  custom,  ///< Custom/unrecognized pattern

  // 2-color schemes
  complementary,  ///< Complementary (180°)
  golden,         ///< Golden angle (137.5°)

  // 3-color schemes
  analogous,            ///< Analogous (30°, 30°)
  triadic,              ///< Triadic (120°, 120°)
  split_complementary,  ///< Split complementary (150°, 60°)
  split_analogous,      ///< Split analogous (60°, 30°)

  // 4-color schemes
  tetradic,              ///< Tetradic rectangle (60°, 120°, 60°); same layout as double_complementary for assess()
  square,                ///< Square (90°, 90°, 90°)
  analogous_wide,        ///< Wide analogous (20°, 20°, 20°)
  compound,              ///< Compound in palette order (30°, 150°, 30°), e.g. 0°/30°/180°/210°
  double_complementary,  ///< Rectangle / two comp. pairs (60°, 120°, 60°), e.g. 0°/60°/180°/240°
};

/**
 * @brief Convert harmony scheme to string.
 * @param scheme Harmony scheme.
 * @return String representation.
 */
inline std::string to_string(harmony_scheme scheme) {
  switch (scheme) {
    case harmony_scheme::single:
      return "single";
    case harmony_scheme::pair:
      return "pair";
    case harmony_scheme::triad:
      return "triad";
    case harmony_scheme::tetrad:
      return "tetrad";
    case harmony_scheme::custom:
      return "custom";
    case harmony_scheme::complementary:
      return "complementary";
    case harmony_scheme::golden:
      return "golden";
    case harmony_scheme::analogous:
      return "analogous";
    case harmony_scheme::triadic:
      return "triadic";
    case harmony_scheme::split_complementary:
      return "split_complementary";
    case harmony_scheme::split_analogous:
      return "split_analogous";
    case harmony_scheme::tetradic:
      return "tetradic";
    case harmony_scheme::square:
      return "square";
    case harmony_scheme::analogous_wide:
      return "analogous_wide";
    case harmony_scheme::compound:
      return "compound";
    case harmony_scheme::double_complementary:
      return "double_complementary";
    default:
      return "unknown";
  }
}

/**
 * @brief Convert string to harmony scheme.
 * @param str String representation.
 * @return Harmony scheme.
 */
inline harmony_scheme from_string(const std::string& str) {
  if (str == "single") return harmony_scheme::single;
  if (str == "pair") return harmony_scheme::pair;
  if (str == "triad") return harmony_scheme::triad;
  if (str == "tetrad") return harmony_scheme::tetrad;
  if (str == "custom") return harmony_scheme::custom;
  if (str == "complementary") return harmony_scheme::complementary;
  if (str == "golden") return harmony_scheme::golden;
  if (str == "analogous") return harmony_scheme::analogous;
  if (str == "triadic") return harmony_scheme::triadic;
  if (str == "split_complementary") return harmony_scheme::split_complementary;
  if (str == "split_analogous") return harmony_scheme::split_analogous;
  if (str == "tetradic") return harmony_scheme::tetradic;
  if (str == "square") return harmony_scheme::square;
  if (str == "analogous_wide") return harmony_scheme::analogous_wide;
  if (str == "compound") return harmony_scheme::compound;
  if (str == "double_complementary") return harmony_scheme::double_complementary;
  return harmony_scheme::custom;
}

}  // namespace colorcpp::algorithms::harmony
