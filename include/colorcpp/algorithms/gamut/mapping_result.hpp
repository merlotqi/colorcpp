/**
 * @file mapping_result.hpp
 * @brief Gamut mapping result information structure.
 */

#pragma once

#include <colorcpp/core/color_base.hpp>

namespace colorcpp::algorithms::gamut {

/**
 * @brief Result of gamut mapping operation.
 * @tparam Color The color type being mapped.
 */
template <typename Color>
struct gamut_mapping_result {
  Color mapped_color;     ///< The mapped color (in gamut)
  float delta_e;          ///< Perceptual color difference (ΔE_OK)
  float original_chroma;  ///< Original chroma value
  float mapped_chroma;    ///< Chroma after mapping
  bool was_in_gamut;      ///< Whether the original color was in gamut

  /**
   * @brief Check if the mapping was successful (color is now in gamut).
   */
  bool is_valid() const { return delta_e >= 0.0f; }

  /**
   * @brief Get the percentage of chroma reduction.
   * @return Percentage (0-100) of chroma reduction. Returns 0 if no reduction was needed.
   */
  float chroma_reduction_percent() const {
    if (original_chroma <= 0.0f) {
      return 0.0f;
    }
    return ((original_chroma - mapped_chroma) / original_chroma) * 100.0f;
  }
};

}  // namespace colorcpp::algorithms::gamut
