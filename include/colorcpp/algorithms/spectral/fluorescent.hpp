/**
 * @file fluorescent.hpp
 * @brief CIE standard fluorescent illuminant series F1-F12.
 *
 * Spectral distributions transcribed from the colour-science dataset
 * (BSD-3-Clause; there named FL1-FL12), covering the CIE 1931
 * standard fluorescent illuminants at 5 nm, 380-780 nm. The CIE
 * "F3.x" variant series and high-pressure discharge lamps are out of
 * scope here.
 */

#pragma once

#include <colorcpp/algorithms/spectral/spd.hpp>
#include <vector>

namespace colorcpp::algorithms::spectral::illuminants {

/** @brief CIE fluorescent F1 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f1() {
  static constexpr float values[] = {
    1.87f, 2.36f, 2.94f, 3.47f, 5.17f, 19.49f, 6.13f, 6.24f,
    7.01f, 7.79f, 8.56f, 43.67f, 16.94f, 10.72f, 11.35f, 11.89f,
    12.37f, 12.75f, 13.0f, 13.15f, 13.23f, 13.17f, 13.13f, 12.85f,
    12.52f, 12.2f, 11.83f, 11.5f, 11.22f, 11.05f, 11.03f, 11.18f,
    11.53f, 27.74f, 17.05f, 13.55f, 14.33f, 15.01f, 15.52f, 18.29f,
    19.55f, 15.48f, 14.91f, 14.15f, 13.22f, 12.19f, 11.12f, 10.03f,
    8.95f, 7.96f, 7.02f, 6.2f, 5.42f, 4.73f, 4.15f, 3.64f,
    3.2f, 2.81f, 2.47f, 2.18f, 1.93f, 1.72f, 1.67f, 1.43f,
    1.29f, 1.19f, 1.08f, 0.96f, 0.88f, 0.81f, 0.77f, 0.75f,
    0.73f, 0.68f, 0.69f, 0.64f, 0.68f, 0.69f, 0.61f, 0.52f,
    0.43f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F2 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f2() {
  static constexpr float values[] = {
    1.18f, 1.48f, 1.84f, 2.15f, 3.44f, 15.69f, 3.85f, 3.74f,
    4.19f, 4.62f, 5.06f, 34.98f, 11.81f, 6.27f, 6.63f, 6.93f,
    7.19f, 7.4f, 7.54f, 7.62f, 7.65f, 7.62f, 7.62f, 7.45f,
    7.28f, 7.15f, 7.05f, 7.04f, 7.16f, 7.47f, 8.04f, 8.88f,
    10.01f, 24.88f, 16.64f, 14.59f, 16.16f, 17.56f, 18.62f, 21.47f,
    22.79f, 19.29f, 18.66f, 17.73f, 16.54f, 15.21f, 13.8f, 12.36f,
    10.95f, 9.65f, 8.4f, 7.32f, 6.31f, 5.43f, 4.68f, 4.02f,
    3.45f, 2.96f, 2.55f, 2.19f, 1.89f, 1.64f, 1.53f, 1.27f,
    1.1f, 0.99f, 0.88f, 0.76f, 0.68f, 0.61f, 0.56f, 0.54f,
    0.51f, 0.47f, 0.47f, 0.43f, 0.46f, 0.47f, 0.4f, 0.33f,
    0.27f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F3 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f3() {
  static constexpr float values[] = {
    0.82f, 1.02f, 1.26f, 1.44f, 2.57f, 14.36f, 2.7f, 2.45f,
    2.73f, 3.0f, 3.28f, 31.85f, 9.47f, 4.02f, 4.25f, 4.44f,
    4.59f, 4.72f, 4.8f, 4.86f, 4.87f, 4.85f, 4.88f, 4.77f,
    4.67f, 4.62f, 4.62f, 4.73f, 4.99f, 5.48f, 6.25f, 7.34f,
    8.78f, 23.82f, 16.14f, 14.59f, 16.63f, 18.49f, 19.95f, 23.11f,
    24.69f, 21.41f, 20.85f, 19.93f, 18.67f, 17.22f, 15.65f, 14.04f,
    12.45f, 10.95f, 9.51f, 8.27f, 7.11f, 6.09f, 5.22f, 4.45f,
    3.8f, 3.23f, 2.75f, 2.33f, 1.99f, 1.7f, 1.55f, 1.27f,
    1.09f, 0.96f, 0.83f, 0.71f, 0.62f, 0.54f, 0.49f, 0.46f,
    0.43f, 0.39f, 0.39f, 0.35f, 0.38f, 0.39f, 0.33f, 0.28f,
    0.21f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F4 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f4() {
  static constexpr float values[] = {
    0.57f, 0.7f, 0.87f, 0.98f, 2.01f, 13.75f, 1.95f, 1.59f,
    1.76f, 1.93f, 2.1f, 30.28f, 8.03f, 2.55f, 2.7f, 2.82f,
    2.91f, 2.99f, 3.04f, 3.08f, 3.09f, 3.09f, 3.14f, 3.06f,
    3.0f, 2.98f, 3.01f, 3.14f, 3.41f, 3.9f, 4.69f, 5.81f,
    7.32f, 22.59f, 15.11f, 13.88f, 16.33f, 18.68f, 20.64f, 24.28f,
    26.26f, 23.28f, 22.94f, 22.14f, 20.91f, 19.43f, 17.74f, 16.0f,
    14.42f, 12.56f, 10.93f, 9.52f, 8.18f, 7.01f, 6.0f, 5.11f,
    4.36f, 3.69f, 3.13f, 2.64f, 2.24f, 1.91f, 1.7f, 1.39f,
    1.18f, 1.03f, 0.88f, 0.74f, 0.64f, 0.54f, 0.49f, 0.46f,
    0.42f, 0.37f, 0.37f, 0.33f, 0.35f, 0.36f, 0.31f, 0.26f,
    0.19f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F5 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f5() {
  static constexpr float values[] = {
    1.87f, 2.35f, 2.92f, 3.45f, 5.1f, 18.91f, 6.0f, 6.11f,
    6.85f, 7.58f, 8.31f, 40.76f, 16.06f, 10.32f, 10.91f, 11.4f,
    11.83f, 12.17f, 12.4f, 12.54f, 12.58f, 12.52f, 12.47f, 12.2f,
    11.89f, 11.61f, 11.33f, 11.1f, 10.96f, 10.97f, 11.16f, 11.54f,
    12.12f, 27.78f, 17.73f, 14.47f, 15.2f, 15.77f, 16.1f, 18.54f,
    19.5f, 15.39f, 14.64f, 13.72f, 12.69f, 11.57f, 10.45f, 9.35f,
    8.29f, 7.32f, 6.41f, 5.63f, 4.9f, 4.26f, 3.72f, 3.25f,
    2.83f, 2.49f, 2.19f, 1.93f, 1.71f, 1.52f, 1.48f, 1.26f,
    1.13f, 1.05f, 0.96f, 0.85f, 0.78f, 0.72f, 0.68f, 0.67f,
    0.65f, 0.61f, 0.62f, 0.59f, 0.62f, 0.64f, 0.55f, 0.47f,
    0.4f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F6 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f6() {
  static constexpr float values[] = {
    1.05f, 1.31f, 1.63f, 1.9f, 3.11f, 14.8f, 3.43f, 3.3f,
    3.68f, 4.07f, 4.45f, 32.61f, 10.74f, 5.48f, 5.78f, 6.03f,
    6.25f, 6.41f, 6.52f, 6.58f, 6.59f, 6.56f, 6.56f, 6.42f,
    6.28f, 6.2f, 6.19f, 6.3f, 6.6f, 7.12f, 7.94f, 9.07f,
    10.49f, 25.22f, 17.46f, 15.63f, 17.22f, 18.53f, 19.43f, 21.97f,
    23.01f, 19.41f, 18.56f, 17.42f, 16.09f, 14.64f, 13.15f, 11.68f,
    10.25f, 8.95f, 7.74f, 6.69f, 5.71f, 4.87f, 4.16f, 3.55f,
    3.02f, 2.57f, 2.2f, 1.87f, 1.6f, 1.37f, 1.29f, 1.05f,
    0.91f, 0.81f, 0.71f, 0.61f, 0.54f, 0.48f, 0.44f, 0.43f,
    0.4f, 0.37f, 0.38f, 0.35f, 0.39f, 0.41f, 0.33f, 0.26f,
    0.21f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F7 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f7() {
  static constexpr float values[] = {
    2.56f, 3.18f, 3.84f, 4.53f, 6.15f, 19.37f, 7.37f, 7.05f,
    7.71f, 8.41f, 9.15f, 44.14f, 17.52f, 11.35f, 12.0f, 12.58f,
    13.08f, 13.45f, 13.71f, 13.88f, 13.95f, 13.93f, 13.82f, 13.64f,
    13.43f, 13.25f, 13.08f, 12.93f, 12.78f, 12.6f, 12.44f, 12.33f,
    12.26f, 29.52f, 17.05f, 12.44f, 12.58f, 12.72f, 12.83f, 15.46f,
    16.75f, 12.83f, 12.67f, 12.45f, 12.19f, 11.89f, 11.6f, 11.35f,
    11.12f, 10.95f, 10.76f, 10.42f, 10.11f, 10.04f, 10.02f, 10.11f,
    9.87f, 8.65f, 7.27f, 6.44f, 5.83f, 5.41f, 5.04f, 4.57f,
    4.12f, 3.77f, 3.46f, 3.08f, 2.73f, 2.47f, 2.25f, 2.06f,
    1.9f, 1.75f, 1.62f, 1.54f, 1.45f, 1.32f, 1.17f, 0.99f,
    0.81f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F8 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f8() {
  static constexpr float values[] = {
    1.21f, 1.5f, 1.81f, 2.13f, 3.17f, 13.08f, 3.83f, 3.45f,
    3.86f, 4.42f, 5.09f, 34.1f, 12.42f, 7.68f, 8.6f, 9.46f,
    10.24f, 10.84f, 11.33f, 11.71f, 11.98f, 12.17f, 12.28f, 12.32f,
    12.35f, 12.44f, 12.55f, 12.68f, 12.77f, 12.72f, 12.6f, 12.43f,
    12.22f, 28.96f, 16.51f, 11.79f, 11.76f, 11.77f, 11.84f, 14.61f,
    16.11f, 12.34f, 12.53f, 12.72f, 12.92f, 13.12f, 13.34f, 13.61f,
    13.87f, 14.07f, 14.2f, 14.16f, 14.13f, 14.34f, 14.5f, 14.46f,
    14.0f, 12.58f, 10.99f, 9.98f, 9.22f, 8.62f, 8.07f, 7.39f,
    6.71f, 6.16f, 5.63f, 5.03f, 4.46f, 4.02f, 3.66f, 3.36f,
    3.09f, 2.85f, 2.65f, 2.51f, 2.37f, 2.15f, 1.89f, 1.61f,
    1.32f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F9 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f9() {
  static constexpr float values[] = {
    0.9f, 1.12f, 1.36f, 1.6f, 2.59f, 12.8f, 3.05f, 2.56f,
    2.86f, 3.3f, 3.82f, 32.62f, 10.77f, 5.84f, 6.57f, 7.25f,
    7.86f, 8.35f, 8.75f, 9.06f, 9.31f, 9.48f, 9.61f, 9.68f,
    9.74f, 9.88f, 10.04f, 10.26f, 10.48f, 10.63f, 10.78f, 10.96f,
    11.18f, 27.71f, 16.29f, 12.28f, 12.74f, 13.21f, 13.65f, 16.57f,
    18.14f, 14.55f, 14.65f, 14.66f, 14.61f, 14.5f, 14.39f, 14.4f,
    14.47f, 14.62f, 14.72f, 14.55f, 14.4f, 14.58f, 14.88f, 15.51f,
    15.47f, 13.2f, 10.57f, 9.18f, 8.25f, 7.57f, 7.03f, 6.35f,
    5.72f, 5.25f, 4.8f, 4.29f, 3.8f, 3.43f, 3.12f, 2.86f,
    2.64f, 2.43f, 2.26f, 2.14f, 2.02f, 1.83f, 1.61f, 1.38f,
    1.12f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F10 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f10() {
  static constexpr float values[] = {
    1.11f, 0.8f, 0.62f, 0.57f, 1.48f, 12.16f, 2.12f, 2.7f,
    3.74f, 5.14f, 6.75f, 34.39f, 14.86f, 10.4f, 10.76f, 10.67f,
    10.11f, 9.27f, 8.29f, 7.29f, 7.91f, 16.64f, 16.73f, 10.44f,
    5.94f, 3.34f, 2.35f, 1.88f, 1.59f, 1.47f, 1.8f, 5.71f,
    40.98f, 73.69f, 33.61f, 8.24f, 3.38f, 2.47f, 2.14f, 4.86f,
    11.45f, 14.79f, 12.16f, 8.97f, 6.52f, 8.31f, 44.12f, 34.55f,
    12.09f, 12.15f, 10.52f, 4.43f, 1.95f, 2.19f, 3.19f, 2.77f,
    2.29f, 2.0f, 1.52f, 1.35f, 1.47f, 1.79f, 1.74f, 1.02f,
    1.14f, 3.32f, 4.49f, 2.05f, 0.49f, 0.24f, 0.21f, 0.21f,
    0.24f, 0.24f, 0.21f, 0.17f, 0.21f, 0.22f, 0.17f, 0.12f,
    0.09f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F11 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f11() {
  static constexpr float values[] = {
    0.91f, 0.63f, 0.46f, 0.37f, 1.29f, 12.68f, 1.59f, 1.79f,
    2.46f, 3.33f, 4.49f, 33.94f, 12.13f, 6.95f, 7.19f, 7.12f,
    6.72f, 6.13f, 5.46f, 4.79f, 5.66f, 14.29f, 14.96f, 8.97f,
    4.72f, 2.33f, 1.47f, 1.1f, 0.89f, 0.83f, 1.18f, 4.9f,
    39.59f, 72.84f, 32.61f, 7.52f, 2.83f, 1.96f, 1.67f, 4.43f,
    11.28f, 14.76f, 12.73f, 9.74f, 7.33f, 9.72f, 55.27f, 42.58f,
    13.18f, 13.16f, 12.26f, 5.11f, 2.07f, 2.34f, 3.58f, 3.01f,
    2.48f, 2.14f, 1.54f, 1.33f, 1.46f, 1.94f, 2.0f, 1.2f,
    1.35f, 4.1f, 5.58f, 2.51f, 0.57f, 0.27f, 0.23f, 0.21f,
    0.24f, 0.24f, 0.2f, 0.24f, 0.32f, 0.26f, 0.16f, 0.12f,
    0.09f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE fluorescent F12 (380-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd f12() {
  static constexpr float values[] = {
    0.96f, 0.64f, 0.4f, 0.33f, 1.19f, 12.48f, 1.12f, 0.94f,
    1.08f, 1.37f, 1.78f, 29.05f, 7.9f, 2.65f, 2.71f, 2.65f,
    2.49f, 2.33f, 2.1f, 1.91f, 3.01f, 10.83f, 11.88f, 6.88f,
    3.43f, 1.49f, 0.92f, 0.71f, 0.6f, 0.63f, 1.1f, 4.56f,
    34.4f, 65.4f, 29.48f, 7.16f, 3.08f, 2.47f, 2.27f, 5.09f,
    11.96f, 15.32f, 14.27f, 11.86f, 9.28f, 12.31f, 68.53f, 53.02f,
    14.67f, 14.38f, 14.71f, 6.46f, 2.57f, 2.75f, 4.18f, 3.44f,
    2.81f, 2.42f, 1.64f, 1.36f, 1.49f, 2.14f, 2.34f, 1.42f,
    1.61f, 5.04f, 6.98f, 3.19f, 0.71f, 0.3f, 0.26f, 0.23f,
    0.28f, 0.28f, 0.21f, 0.17f, 0.21f, 0.19f, 0.15f, 0.1f,
    0.05f,
  };
  return spd(380.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

}  // namespace colorcpp::algorithms::spectral::illuminants
