/**
 * @file led.hpp
 * @brief Typical LED illuminant spectra (blue-pumped, RGB and violet).
 *
 * The nine representative LED distributions from the colour-science
 * dataset (BSD-3-Clause; Ohno & Sauter's measured typical LEDs), 5 nm,
 * 380-780 nm. There is no CIE standard LED illuminant set — these are
 * the widely used reference measurements.
 */

#pragma once

#include <colorcpp/algorithms/spectral/spd.hpp>
#include <vector>

namespace colorcpp::algorithms::spectral::illuminants {

/** @brief typical LED LED-B1 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_b1() {
  static constexpr float values[] = {
    0.0f, 0.01f, 0.01f, 0.02f, 0.04f, 0.07f, 0.15f, 0.28f,
    0.53f, 0.92f, 1.54f, 2.52f, 4.16f, 6.39f, 7.89f, 7.57f,
    6.25f, 5.16f, 4.37f, 3.84f, 3.7f, 3.88f, 4.26f, 4.79f,
    5.41f, 6.05f, 6.7f, 7.34f, 7.98f, 8.64f, 9.31f, 10.02f,
    10.76f, 11.53f, 12.38f, 13.28f, 14.23f, 15.2f, 16.2f, 17.19f,
    18.14f, 19.02f, 19.78f, 20.38f, 20.8f, 21.0f, 20.97f, 20.71f,
    20.23f, 19.56f, 18.76f, 17.82f, 16.78f, 15.66f, 14.49f, 13.31f,
    12.15f, 11.02f, 9.95f, 8.93f, 7.98f, 7.1f, 6.3f, 5.57f,
    4.89f, 4.26f, 3.72f, 3.25f, 2.83f, 2.47f, 2.15f, 1.87f,
    1.63f, 1.42f, 1.25f, 1.1f, 0.97f, 0.86f, 0.76f, 0.68f,
    0.61f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-B2 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_b2() {
  static constexpr float values[] = {
    0.0f, 0.0f, 0.0f, 0.01f, 0.02f, 0.05f, 0.11f, 0.24f,
    0.5f, 0.96f, 1.71f, 2.82f, 4.71f, 7.87f, 10.56f, 10.24f,
    7.94f, 6.25f, 5.23f, 4.36f, 4.0f, 4.17f, 4.59f, 5.21f,
    5.96f, 6.72f, 7.44f, 8.11f, 8.74f, 9.39f, 10.06f, 10.75f,
    11.49f, 12.25f, 13.05f, 13.86f, 14.7f, 15.5f, 16.28f, 17.0f,
    17.69f, 18.31f, 18.83f, 19.24f, 19.51f, 19.59f, 19.48f, 19.19f,
    18.72f, 18.1f, 17.34f, 16.47f, 15.51f, 14.49f, 13.42f, 12.33f,
    11.26f, 10.24f, 9.25f, 8.3f, 7.42f, 6.6f, 5.85f, 5.17f,
    4.55f, 3.99f, 3.49f, 3.04f, 2.65f, 2.31f, 2.01f, 1.75f,
    1.52f, 1.32f, 1.15f, 1.0f, 0.87f, 0.76f, 0.67f, 0.59f,
    0.52f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-B3 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_b3() {
  static constexpr float values[] = {
    0.0f, 0.0f, 0.01f, 0.02f, 0.05f, 0.11f, 0.25f, 0.56f,
    1.2f, 2.37f, 4.24f, 7.17f, 12.11f, 17.7f, 18.87f, 14.58f,
    9.9f, 7.53f, 6.02f, 5.11f, 5.17f, 5.82f, 6.77f, 7.88f,
    8.94f, 9.82f, 10.52f, 11.06f, 11.51f, 11.93f, 12.37f, 12.83f,
    13.31f, 13.79f, 14.26f, 14.68f, 15.08f, 15.42f, 15.72f, 15.97f,
    16.18f, 16.37f, 16.5f, 16.55f, 16.5f, 16.32f, 16.0f, 15.54f,
    14.96f, 14.27f, 13.48f, 12.62f, 11.71f, 10.78f, 9.85f, 8.93f,
    8.04f, 7.2f, 6.42f, 5.69f, 5.02f, 4.42f, 3.88f, 3.39f,
    2.95f, 2.56f, 2.22f, 1.92f, 1.65f, 1.43f, 1.23f, 1.06f,
    0.91f, 0.78f, 0.68f, 0.58f, 0.5f, 0.43f, 0.37f, 0.32f,
    0.28f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-B4 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_b4() {
  static constexpr float values[] = {
    0.0f, 0.0f, 0.0f, 0.01f, 0.03f, 0.08f, 0.21f, 0.54f,
    1.31f, 2.75f, 5.04f, 8.31f, 13.37f, 20.61f, 25.91f, 24.12f,
    17.47f, 11.9f, 8.62f, 6.34f, 4.89f, 4.25f, 4.19f, 4.7f,
    5.77f, 7.27f, 8.94f, 10.6f, 12.11f, 13.36f, 14.3f, 15.02f,
    15.54f, 15.9f, 16.15f, 16.33f, 16.42f, 16.44f, 16.36f, 16.2f,
    15.98f, 15.7f, 15.36f, 14.93f, 14.44f, 13.86f, 13.24f, 12.57f,
    11.84f, 11.12f, 10.38f, 9.65f, 8.91f, 8.19f, 7.51f, 6.86f,
    6.21f, 5.6f, 5.05f, 4.54f, 4.05f, 3.62f, 3.23f, 2.87f,
    2.53f, 2.24f, 1.99f, 1.76f, 1.55f, 1.37f, 1.21f, 1.07f,
    0.95f, 0.85f, 0.75f, 0.67f, 0.61f, 0.55f, 0.5f, 0.45f,
    0.42f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-B5 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_b5() {
  static constexpr float values[] = {
    0.0f, 0.01f, 0.01f, 0.03f, 0.07f, 0.16f, 0.35f, 0.78f,
    1.67f, 3.3f, 5.94f, 9.92f, 16.15f, 25.15f, 32.34f, 31.18f,
    23.46f, 16.39f, 12.08f, 9.08f, 7.21f, 6.48f, 6.49f, 7.05f,
    8.1f, 9.45f, 10.88f, 12.22f, 13.38f, 14.29f, 14.98f, 15.48f,
    15.81f, 16.01f, 16.1f, 16.13f, 16.1f, 15.98f, 15.77f, 15.5f,
    15.15f, 14.76f, 14.31f, 13.79f, 13.22f, 12.59f, 11.91f, 11.2f,
    10.47f, 9.73f, 9.0f, 8.27f, 7.57f, 6.9f, 6.25f, 5.65f,
    5.08f, 4.55f, 4.06f, 3.61f, 3.22f, 2.86f, 2.54f, 2.26f,
    2.0f, 1.75f, 1.54f, 1.36f, 1.2f, 1.05f, 0.93f, 0.82f,
    0.72f, 0.64f, 0.56f, 0.49f, 0.44f, 0.39f, 0.35f, 0.31f,
    0.27f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-BH1 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_bh1() {
  static constexpr float values[] = {
    0.0f, 0.01f, 0.02f, 0.04f, 0.08f, 0.17f, 0.41f, 0.93f,
    1.84f, 3.05f, 4.29f, 5.69f, 7.06f, 7.91f, 7.74f, 6.62f,
    5.13f, 3.85f, 2.94f, 2.34f, 1.99f, 1.92f, 2.19f, 2.87f,
    4.03f, 5.64f, 7.49f, 9.25f, 10.82f, 12.11f, 13.08f, 13.66f,
    14.01f, 14.19f, 14.28f, 14.27f, 14.19f, 14.03f, 13.82f, 13.59f,
    13.35f, 13.2f, 13.19f, 13.46f, 14.31f, 16.15f, 19.49f, 25.07f,
    33.87f, 40.75f, 35.59f, 21.59f, 11.11f, 7.27f, 5.58f, 4.64f,
    3.98f, 3.48f, 3.07f, 2.73f, 2.42f, 2.15f, 1.91f, 1.7f,
    1.5f, 1.33f, 1.18f, 1.05f, 0.93f, 0.83f, 0.74f, 0.65f,
    0.57f, 0.51f, 0.46f, 0.41f, 0.37f, 0.34f, 0.3f, 0.28f,
    0.25f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-RGB1 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_rgb1() {
  static constexpr float values[] = {
    0.0f, 0.0f, 0.0f, 0.01f, 0.02f, 0.05f, 0.11f, 0.24f,
    0.5f, 0.94f, 1.6f, 2.43f, 3.38f, 4.59f, 5.84f, 5.56f,
    4.0f, 3.14f, 2.8f, 2.68f, 3.03f, 3.98f, 5.53f, 7.84f,
    10.96f, 14.61f, 18.01f, 20.24f, 20.57f, 19.22f, 16.93f, 14.58f,
    12.61f, 11.09f, 9.93f, 9.11f, 8.6f, 8.31f, 8.21f, 8.24f,
    8.42f, 8.74f, 9.25f, 10.0f, 11.1f, 12.74f, 15.35f, 19.43f,
    26.01f, 36.47f, 51.1f, 62.69f, 52.07f, 28.62f, 15.5f, 9.58f,
    6.54f, 4.95f, 4.01f, 3.37f, 2.89f, 2.51f, 2.18f, 1.92f,
    1.67f, 1.44f, 1.25f, 1.09f, 0.93f, 0.82f, 0.71f, 0.61f,
    0.53f, 0.45f, 0.4f, 0.35f, 0.3f, 0.27f, 0.24f, 0.21f,
    0.17f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-V1 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_v1() {
  static constexpr float values[] = {
    0.01f, 0.03f, 0.14f, 0.61f, 2.62f, 8.43f, 16.51f, 17.9f,
    11.26f, 5.94f, 3.31f, 2.25f, 2.07f, 2.29f, 2.68f, 3.15f,
    3.64f, 4.18f, 4.83f, 5.57f, 6.4f, 7.27f, 8.08f, 8.73f,
    9.21f, 9.54f, 9.75f, 9.91f, 10.06f, 10.26f, 10.51f, 10.82f,
    11.17f, 11.55f, 11.92f, 12.3f, 12.68f, 13.07f, 13.52f, 14.06f,
    14.7f, 15.46f, 16.38f, 17.39f, 18.5f, 19.51f, 20.52f, 21.52f,
    22.38f, 23.07f, 23.51f, 23.71f, 23.67f, 23.35f, 22.81f, 22.01f,
    21.03f, 19.91f, 18.69f, 17.39f, 16.05f, 14.69f, 13.36f, 12.04f,
    10.8f, 9.62f, 8.54f, 7.57f, 6.68f, 5.86f, 5.12f, 4.46f,
    3.88f, 3.37f, 2.94f, 2.54f, 2.21f, 1.9f, 1.65f, 1.44f,
    1.22f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief typical LED LED-V2 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd led_v2() {
  static constexpr float values[] = {
    0.01f, 0.06f, 0.26f, 1.15f, 4.47f, 11.98f, 19.89f, 20.24f,
    14.39f, 8.95f, 5.78f, 4.65f, 4.86f, 5.78f, 6.97f, 8.1f,
    9.05f, 9.82f, 10.52f, 11.21f, 11.87f, 12.5f, 12.98f, 13.23f,
    13.3f, 13.21f, 13.05f, 12.88f, 12.74f, 12.68f, 12.7f, 12.77f,
    12.91f, 13.04f, 13.16f, 13.26f, 13.35f, 13.42f, 13.52f, 13.64f,
    13.83f, 14.07f, 14.42f, 14.82f, 15.26f, 15.64f, 16.03f, 16.4f,
    16.67f, 16.88f, 16.93f, 16.85f, 16.62f, 16.23f, 15.72f, 15.07f,
    14.31f, 13.45f, 12.57f, 11.64f, 10.69f, 9.78f, 8.87f, 8.0f,
    7.17f, 6.37f, 5.65f, 5.01f, 4.42f, 3.88f, 3.39f, 2.95f,
    2.58f, 2.24f, 1.95f, 1.7f, 1.47f, 1.3f, 1.1f, 0.96f,
    0.84f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

}  // namespace colorcpp::algorithms::spectral::illuminants
