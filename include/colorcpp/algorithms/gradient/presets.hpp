/**
 * @file presets.hpp
 * @brief Ready-to-use professional gradient presets.
 *
 * Provides curated gradient presets for scientific visualization and UI design.
 * Each preset can be returned at its canonical stop count or resampled to an arbitrary number of stops.
 */

#pragma once

#include <colorcpp/algorithms/gradient/linear.hpp>
#include <colorcpp/io/literals.hpp>
#include <colorcpp/operations/conversion.hpp>

namespace colorcpp::algorithms::gradient::preset {

namespace details {

template <typename Color>
linear_gradient<Color> make_linear_preset(std::initializer_list<color_stop<core::rgba8_t>> stops,
                                          std::size_t stop_count) {
  typename color_stops<Color>::container_type converted;
  converted.reserve(stops.size());
  for (const auto& stop : stops) {
    converted.emplace_back(stop.position, operations::conversion::color_cast<Color>(stop.color));
  }

  linear_gradient<Color> base{color_stops<Color>(converted)};
  if (stop_count == 0 || stop_count == converted.size()) {
    return base;
  }
  if (stop_count == 1) {
    return linear_gradient<Color>({{0.5f, base.sample(0.5f)}});
  }

  typename color_stops<Color>::container_type resampled;
  resampled.reserve(stop_count);
  for (std::size_t i = 0; i < stop_count; ++i) {
    float t = static_cast<float>(i) / static_cast<float>(stop_count - 1);
    resampled.emplace_back(t, base.sample(t));
  }
  return linear_gradient<Color>(color_stops<Color>(resampled));
}

}  // namespace details

/**
 * @brief Viridis - The gold standard scientific colormap.
 *
 * The default recommended colormap for all scientific visualization.
 * Perceptually uniform, colorblind friendly, print friendly, monotonically increasing brightness.
 * This is the most carefully designed colormap available.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> viridis(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x440154_rgb},
          {0.25f, 0x31688e_rgb},
          {0.50f, 0x21918c_rgb},
          {0.75f, 0x5ec962_rgb},
          {1.00f, 0xfde725_rgb},
      },
      stop_count);
}

/**
 * @brief Plasma - High contrast warm colormap.
 *
 * Bright, vibrant, high contrast alternative to viridis.
 * Excellent for heatmaps, density plots and when you need maximum visual separation.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> plasma(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x0d0887_rgb},
          {0.25f, 0x7e03a8_rgb},
          {0.50f, 0xcc4778_rgb},
          {0.75f, 0xf89540_rgb},
          {1.00f, 0xf0f921_rgb},
      },
      stop_count);
}

/**
 * @brief Inferno - Black body radiation colormap.
 *
 * Mimics the colors of heating metal from black through red to yellow-white.
 * Excellent for dark backgrounds, astronomy and thermal visualization.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> inferno(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x000004_rgb},
          {0.25f, 0x420a68_rgb},
          {0.50f, 0x932667_rgb},
          {0.75f, 0xdd513a_rgb},
          {1.00f, 0xfcffa4_rgb},
      },
      stop_count);
}

/**
 * @brief Magma - Magenta to yellow colormap.
 *
 * Very similar to inferno but with stronger purple/magenta tones.
 * Good for aesthetic visualizations and artistic use.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> magma(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x000004_rgb},
          {0.25f, 0x451077_rgb},
          {0.50f, 0x9f2f7f_rgb},
          {0.75f, 0xf66e5c_rgb},
          {1.00f, 0xfcfdbf_rgb},
      },
      stop_count);
}

/**
 * @brief Cividis - Optimized for red-green color blindness.
 *
 * Specifically designed to be perfectly distinguishable by people with
 * protanopia and deuteranopia (the most common forms of color blindness).
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> cividis(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x00204c_rgb},
          {0.25f, 0x364b73_rgb},
          {0.50f, 0x7d7c78_rgb},
          {0.75f, 0xbcab63_rgb},
          {1.00f, 0xffe945_rgb},
      },
      stop_count);
}

/**
 * @brief Rainbow - Classic spectral rainbow gradient.
 *
 * Traditional full spectrum rainbow from red through violet.
 * Not recommended for scientific visualization, use for UI design only.
 *
 * @param stop_count Number of stops to resample to (0 = use native 7 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> rainbow(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0xef4444_rgb},
          {0.16f, 0xf97316_rgb},
          {0.33f, 0xeab308_rgb},
          {0.50f, 0x22c55e_rgb},
          {0.66f, 0x3b82f6_rgb},
          {0.83f, 0x8b5cf6_rgb},
          {1.00f, 0xec4899_rgb},
      },
      stop_count);
}

/**
 * @brief Spectral - Diverging red-blue colormap.
 *
 * Professional diverging colormap going from red through yellow to blue.
 * Ideal for showing deviation from a center value, differences and contrast.
 *
 * @param stop_count Number of stops to resample to (0 = use native 7 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> spectral(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x9e0142_rgb},
          {0.16f, 0xd53e4f_rgb},
          {0.33f, 0xf46d43_rgb},
          {0.50f, 0xfee08b_rgb},
          {0.66f, 0xabdda4_rgb},
          {0.83f, 0x3288bd_rgb},
          {1.00f, 0x5e4fa2_rgb},
      },
      stop_count);
}

/**
 * @brief Ocean - Deep blue to light cyan gradient.
 *
 * Professional UI design gradient with modern blue tones.
 * Great for backgrounds, progress bars and water related visualizations.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> ocean(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x030712_rgb},
          {0.25f, 0x075985_rgb},
          {0.50f, 0x0284c7_rgb},
          {0.75f, 0x38bdf8_rgb},
          {1.00f, 0xbae6fd_rgb},
      },
      stop_count);
}

/**
 * @brief Forest - Dark green through bright green to yellow.
 *
 * Natural forest tones, good for nature, environment and ecological visualizations.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> forest(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x052e16_rgb},
          {0.25f, 0x166534_rgb},
          {0.50f, 0x22c55e_rgb},
          {0.75f, 0xa3e635_rgb},
          {1.00f, 0xfef08a_rgb},
      },
      stop_count);
}

/**
 * @brief Sunset - Deep purple through magenta to warm orange.
 *
 * Beautiful sunset colors, one of the most popular design gradients.
 * Excellent for hero backgrounds, banners and decorative use.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> sunset(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x312e81_rgb},
          {0.25f, 0x7c3aed_rgb},
          {0.50f, 0xdb2777_rgb},
          {0.75f, 0xf97316_rgb},
          {1.00f, 0xfde047_rgb},
      },
      stop_count);
}

/**
 * @brief Night - Deep space blue purple gradient.
 *
 * Dark atmospheric night sky tones, perfect for dark mode interfaces.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> night(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x020617_rgb},
          {0.25f, 0x172554_rgb},
          {0.50f, 0x312e81_rgb},
          {0.75f, 0x6d28d9_rgb},
          {1.00f, 0xc4b5fd_rgb},
      },
      stop_count);
}

/**
 * @brief Fire - Black through red to bright yellow.
 *
 * Realistic fire and flame gradient. Great for heat, warning and danger visualizations.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> fire(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x0c0a09_rgb},
          {0.25f, 0x991b1b_rgb},
          {0.50f, 0xea580c_rgb},
          {0.75f, 0xfbbf24_rgb},
          {1.00f, 0xfef3c7_rgb},
      },
      stop_count);
}

/**
 * @brief Ice - Deep blue through cyan to white.
 *
 * Cold, clean ice and frost gradient. Perfect for winter, cold, technology themes.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> ice(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x1e3a8a_rgb},
          {0.25f, 0x0ea5e9_rgb},
          {0.50f, 0x22d3ee_rgb},
          {0.75f, 0xa5f3fc_rgb},
          {1.00f, 0xf8fafc_rgb},
      },
      stop_count);
}

/**
 * @brief Neon - Cyberpunk vaporwave gradient.
 *
 * Modern neon aesthetic with cyan, blue and pink tones.
 * Popular for retrofuturistic and cyberpunk design.
 *
 * @param stop_count Number of stops to resample to (0 = use native 5 stops)
 * @tparam Color Output color type, defaults to rgba8_t
 * @return Linear gradient preset
 */
template <typename Color = core::rgba8_t>
linear_gradient<Color> neon(std::size_t stop_count = 0) {
  using namespace colorcpp::io::literals;
  return details::make_linear_preset<Color>(
      {
          {0.00f, 0x0f172a_rgb},
          {0.25f, 0x2563eb_rgb},
          {0.50f, 0x06b6d4_rgb},
          {0.75f, 0xf472b6_rgb},
          {1.00f, 0xfef08a_rgb},
      },
      stop_count);
}

}  // namespace colorcpp::algorithms::gradient::preset
