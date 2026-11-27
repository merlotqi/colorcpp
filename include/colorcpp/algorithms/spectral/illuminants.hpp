/**
 * @file illuminants.hpp
 * @brief CIE standard illuminants A and D series, the CIE daylight locus
 *        and the S0/S1/S2 daylight SPD synthesizer.
 *
 * Spectral distributions transcribed from the colour-science
 * colour/colorimetry/datasets/illuminants/sds.py (BSD-3-Clause), which
 * reproduces the CIE data. The daylight locus (CCT -> xy) follows the
 * Wyszecki & Stiles relationships used by CIE 15:2004: a piecewise cubic
 * in 1/T for x (<= 7000 K / > 7000 K) and the quadratic y(x). F series
 * and typical LED spectra live in fluorescent.hpp / led.hpp.
 *
 * The D-series synthesizer follows CIE 15:2004 Appendix C: the daylight
 * basis functions S0/S1/S2 are combined as S = S0 + M1*S1 + M2*S2, with
 * M1/M2 rounded to three decimals as the standard recommends.
 *
 * @see CIE 15:2004, Section 7 (daylight locus) and Appendix C (D series)
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/spectral/spd.hpp>
#include <iterator>
#include <utility>
#include <vector>

namespace colorcpp::algorithms::spectral::illuminants {

/** @brief CIE standard illuminant A (300-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd a() {
  static constexpr float values[] = {
    0.930483f, 1.12821f, 1.35769f, 1.62219f, 1.92508f, 2.2698f, 2.65981f, 3.09861f,
    3.58968f, 4.13648f, 4.74238f, 5.4107f, 6.14462f, 6.9472f, 7.82135f, 8.7698f,
    9.7951f, 10.8996f, 12.0853f, 13.3543f, 14.708f, 16.148f, 17.6753f, 19.2907f,
    20.995f, 22.7883f, 24.6709f, 26.6425f, 28.7027f, 30.8508f, 33.0859f, 35.4068f,
    37.8121f, 40.3002f, 42.8693f, 45.5174f, 48.2423f, 51.0418f, 53.9132f, 56.8539f,
    59.8611f, 62.932f, 66.0635f, 69.2525f, 72.4959f, 75.7903f, 79.1326f, 82.5193f,
    85.947f, 89.4124f, 92.912f, 96.4423f, 100.0f, 103.582f, 107.184f, 110.803f,
    114.436f, 118.08f, 121.731f, 125.386f, 129.043f, 132.697f, 136.346f, 139.988f,
    143.618f, 147.235f, 150.836f, 154.418f, 157.979f, 161.516f, 165.028f, 168.51f,
    171.963f, 175.383f, 178.769f, 182.118f, 185.429f, 188.701f, 191.931f, 195.118f,
    198.261f, 201.359f, 204.409f, 207.411f, 210.365f, 213.268f, 216.12f, 218.92f,
    221.667f, 224.361f, 227.0f, 229.585f, 232.115f, 234.589f, 237.008f, 239.37f,
    241.675f,
  };
  return spd(300.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE D50 (300-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd d50() {
  static constexpr float values[] = {
    0.019f, 1.035f, 2.051f, 4.914f, 7.778f, 11.263f, 14.748f, 16.348f,
    17.948f, 19.479f, 21.01f, 22.476f, 23.942f, 25.451f, 26.961f, 25.724f,
    24.488f, 27.179f, 29.871f, 39.589f, 49.308f, 52.91f, 56.513f, 58.273f,
    60.034f, 58.926f, 57.818f, 66.321f, 74.825f, 81.036f, 87.247f, 88.93f,
    90.612f, 90.99f, 91.368f, 93.238f, 95.109f, 93.536f, 91.963f, 93.843f,
    95.724f, 96.169f, 96.613f, 96.871f, 97.129f, 99.614f, 102.099f, 101.427f,
    100.755f, 101.536f, 102.317f, 101.159f, 100.0f, 98.868f, 97.735f, 98.327f,
    98.918f, 96.208f, 93.499f, 95.593f, 97.688f, 98.478f, 99.269f, 99.155f,
    99.042f, 97.382f, 95.722f, 97.29f, 98.857f, 97.262f, 95.667f, 96.929f,
    98.19f, 100.597f, 103.003f, 101.068f, 99.133f, 93.257f, 87.381f, 89.492f,
    91.604f, 92.246f, 92.889f, 84.872f, 76.854f, 81.683f, 86.511f, 89.546f,
    92.58f, 85.405f, 78.23f, 67.961f, 57.692f, 70.307f, 82.923f, 80.599f,
    78.274f,
  };
  return spd(300.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE D55 (300-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd d55() {
  static constexpr float values[] = {
    0.024f, 1.048f, 2.072f, 6.648f, 11.224f, 15.936f, 20.647f, 22.266f,
    23.885f, 25.851f, 27.817f, 29.219f, 30.621f, 32.464f, 34.308f, 33.446f,
    32.584f, 35.335f, 38.087f, 49.518f, 60.949f, 64.751f, 68.554f, 70.065f,
    71.577f, 69.746f, 67.914f, 76.76f, 85.605f, 91.799f, 97.993f, 99.228f,
    100.463f, 100.188f, 99.913f, 101.326f, 102.739f, 100.409f, 98.078f, 99.379f,
    100.68f, 100.688f, 100.695f, 100.341f, 99.987f, 102.098f, 104.21f, 103.156f,
    102.102f, 102.535f, 102.968f, 101.484f, 100.0f, 98.608f, 97.216f, 97.482f,
    97.749f, 94.59f, 91.432f, 92.926f, 94.419f, 94.78f, 95.14f, 94.68f,
    94.22f, 92.334f, 90.448f, 91.389f, 92.33f, 90.592f, 88.854f, 89.586f,
    90.317f, 92.133f, 93.95f, 91.953f, 89.956f, 84.817f, 79.677f, 81.258f,
    82.84f, 83.842f, 84.844f, 77.539f, 70.235f, 74.768f, 79.301f, 82.147f,
    84.993f, 78.437f, 71.88f, 62.337f, 52.793f, 64.36f, 75.927f, 73.872f,
    71.818f,
  };
  return spd(300.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE D60 (300-830 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd d60() {
  static constexpr float values[] = {
    0.0293f, 1.2899f, 2.5505f, 9.04125f, 15.532f, 21.966749999999998f, 28.4015f, 29.958349999999996f,
    31.515199999999993f, 33.78424999999999f, 36.05329999999999f, 37.231f, 38.4087f, 40.6733f, 42.9379f, 42.08869999999999f,
    41.23949999999999f, 43.844649999999994f, 46.449799999999996f, 59.30195f, 72.15409999999999f, 76.22205f, 80.28999999999999f, 81.5341f,
    82.7782f, 80.17745f, 77.5767f, 86.5984f, 95.62010000000001f, 101.76225f, 107.9044f, 108.71109999999999f,
    109.5178f, 108.62295f, 107.7281f, 108.6911f, 109.6541f, 106.6692f, 103.6843f, 104.4494f,
    105.21449999999999f, 104.8149f, 104.4153f, 103.46975f, 102.52420000000001f, 104.29085f, 106.0575f, 104.68549999999999f,
    103.31349999999999f, 103.42665f, 103.53980000000001f, 101.76989999999999f, 100.0f, 98.3759f, 96.75179999999999f, 96.7319f,
    96.712f, 93.2952f, 89.8784f, 90.9071f, 91.9358f, 91.97585000000001f, 92.0159f, 91.2841f,
    90.5523f, 88.48885000000001f, 86.4254f, 86.9331f, 87.4408f, 85.631f, 83.8212f, 84.1803f,
    84.5394f, 85.9118f, 87.2842f, 85.273f, 83.2618f, 78.62875f, 73.9957f, 75.2048f,
    76.4139f, 77.64344999999999f, 78.873f, 72.09849999999999f, 65.324f, 69.6352f, 73.94640000000001f, 76.6527f,
    79.359f, 73.26265f, 67.16629999999999f, 58.16505f, 49.1638f, 59.9502f, 70.7366f, 68.8789f,
    67.02120000000001f, 67.52260000000001f, 68.024f, 65.43955f, 62.85509999999999f, 58.86505f, 54.875f, 57.7862f,
    60.6974f, 62.226949999999995f, 63.7565f,
  };
  return spd(300.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE D65 (300-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd d65() {
  static constexpr float values[] = {
    0.0341f, 1.6643f, 3.2945f, 11.7652f, 20.236f, 28.6447f, 37.0535f, 38.5011f,
    39.9488f, 42.4302f, 44.9117f, 45.775f, 46.6383f, 49.3637f, 52.0891f, 51.0323f,
    49.9755f, 52.3118f, 54.6482f, 68.7015f, 82.7549f, 87.1204f, 91.486f, 92.4589f,
    93.4318f, 90.057f, 86.6823f, 95.7736f, 104.865f, 110.936f, 117.008f, 117.41f,
    117.812f, 116.336f, 114.861f, 115.392f, 115.923f, 112.367f, 108.811f, 109.082f,
    109.354f, 108.578f, 107.802f, 106.296f, 104.79f, 106.239f, 107.689f, 106.047f,
    104.405f, 104.225f, 104.046f, 102.023f, 100.0f, 98.1671f, 96.3342f, 96.0611f,
    95.788f, 92.2368f, 88.6856f, 89.3459f, 90.0062f, 89.8026f, 89.5991f, 88.6489f,
    87.6987f, 85.4936f, 83.2886f, 83.4939f, 83.6992f, 81.863f, 80.0268f, 80.1207f,
    80.2146f, 81.2462f, 82.2778f, 80.281f, 78.2842f, 74.0027f, 69.7213f, 70.6652f,
    71.6091f, 72.979f, 74.349f, 67.9765f, 61.604f, 65.7448f, 69.8856f, 72.4863f,
    75.087f, 69.3398f, 63.5927f, 55.0054f, 46.4182f, 56.6118f, 66.8054f, 65.0941f,
    63.3828f,
  };
  return spd(300.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/** @brief CIE D75 (300-780 nm, 5 nm) (colour-science dataset, BSD-3-Clause). */
inline spd d75() {
  static constexpr float values[] = {
    0.043f, 2.588f, 5.133f, 17.47f, 29.808f, 42.369f, 54.93f, 56.095f,
    57.259f, 60.0f, 62.74f, 62.861f, 62.982f, 66.647f, 70.312f, 68.507f,
    66.703f, 68.333f, 69.963f, 85.946f, 101.929f, 106.911f, 111.894f, 112.346f,
    112.798f, 107.945f, 103.092f, 112.145f, 121.198f, 127.104f, 133.01f, 132.682f,
    132.355f, 129.838f, 127.322f, 127.061f, 126.8f, 122.291f, 117.783f, 117.186f,
    116.589f, 115.146f, 113.702f, 111.181f, 108.659f, 109.552f, 110.445f, 108.367f,
    106.289f, 105.596f, 104.904f, 102.452f, 100.0f, 97.808f, 95.616f, 94.914f,
    94.213f, 90.605f, 86.997f, 87.112f, 87.227f, 86.684f, 86.14f, 84.861f,
    83.581f, 81.164f, 78.747f, 78.587f, 78.428f, 76.614f, 74.801f, 74.562f,
    74.324f, 74.873f, 75.422f, 73.499f, 71.576f, 67.714f, 63.852f, 64.464f,
    65.076f, 66.573f, 68.07f, 62.256f, 56.443f, 60.343f, 64.242f, 66.697f,
    69.151f, 63.89f, 58.629f, 50.623f, 42.617f, 51.985f, 61.352f, 59.838f,
    58.324f,
  };
  return spd(300.0f, 5.0f, std::vector<float>(std::begin(values), std::end(values)));
}

/**
 * @brief CIE daylight locus chromaticity for a correlated colour temperature.
 *
 * x comes from the piecewise cubic in 1/T (CIE D series, Wyszecki &
 * Stiles), y from the quadratic locus relation y = -3x^2 + 2.87x - 0.275.
 * The physical domain is [4000, 25000] K; the input is clamped to it so
 * out-of-domain calls stay deterministic.
 */
inline std::pair<float, float> cie_d_xy(float cct) {
  const double T = std::clamp(static_cast<double>(cct), 4000.0, 25000.0);
  double x;
  if (T <= 7000.0) {
    x = -4.607e9 / (T * T * T) + 2.9678e6 / (T * T) + 0.09911e3 / T + 0.244063;
  } else {
    x = -2.0064e9 / (T * T * T) + 1.9018e6 / (T * T) + 0.24748e3 / T + 0.23704;
  }
  const double y = -3.0 * x * x + 2.870 * x - 0.275;
  return {static_cast<float>(x), static_cast<float>(y)};
}

namespace details {

/** @brief One sample of the CIE daylight basis functions S0/S1/S2. */
struct daylight_basis_sample {
  float nm;
  float s0;
  float s1;
  float s2;
};

/**
 * @brief CIE daylight basis functions S0/S1/S2, 300-830 nm at 5 nm.
 *
 * Transcribed from colour-science's
 * datasets/illuminants/sds_d_illuminant_series.py (BSD-3-Clause), which
 * reproduces CIE 15:2004 Table T.2. S0(560) = 100 and S1(560) = S2(560) = 0,
 * so every synthesized spectrum is normalized to S(560) = 100.
 */
inline constexpr daylight_basis_sample DAYLIGHT_BASIS[] = {
  {300.0f, 0.04f, 0.02f, 0.0f}, {305.0f, 3.02f, 2.26f, 1.0f}, {310.0f, 6.0f, 4.5f, 2.0f},
  {315.0f, 17.8f, 13.45f, 3.0f}, {320.0f, 29.6f, 22.4f, 4.0f}, {325.0f, 42.45f, 32.2f, 6.25f},
  {330.0f, 55.3f, 42.0f, 8.5f}, {335.0f, 56.3f, 41.3f, 8.15f}, {340.0f, 57.3f, 40.6f, 7.8f},
  {345.0f, 59.55f, 41.1f, 7.25f}, {350.0f, 61.8f, 41.6f, 6.7f}, {355.0f, 61.65f, 39.8f, 6.0f},
  {360.0f, 61.5f, 38.0f, 5.3f}, {365.0f, 65.15f, 40.2f, 5.7f}, {370.0f, 68.8f, 42.4f, 6.1f},
  {375.0f, 66.1f, 40.45f, 4.55f}, {380.0f, 63.4f, 38.5f, 3.0f}, {385.0f, 64.6f, 36.75f, 2.1f},
  {390.0f, 65.8f, 35.0f, 1.2f}, {395.0f, 80.3f, 39.2f, 0.05f}, {400.0f, 94.8f, 43.4f, -1.1f},
  {405.0f, 99.8f, 44.85f, -0.8f}, {410.0f, 104.8f, 46.3f, -0.5f}, {415.0f, 105.35f, 45.1f, -0.6f},
  {420.0f, 105.9f, 43.9f, -0.7f}, {425.0f, 101.35f, 40.5f, -0.95f}, {430.0f, 96.8f, 37.1f, -1.2f},
  {435.0f, 105.35f, 36.9f, -1.9f}, {440.0f, 113.9f, 36.7f, -2.6f}, {445.0f, 119.75f, 36.3f, -2.75f},
  {450.0f, 125.6f, 35.9f, -2.9f}, {455.0f, 125.55f, 34.25f, -2.85f}, {460.0f, 125.5f, 32.6f, -2.8f},
  {465.0f, 123.4f, 30.25f, -2.7f}, {470.0f, 121.3f, 27.9f, -2.6f}, {475.0f, 121.3f, 26.1f, -2.6f},
  {480.0f, 121.3f, 24.3f, -2.6f}, {485.0f, 117.4f, 22.2f, -2.2f}, {490.0f, 113.5f, 20.1f, -1.8f},
  {495.0f, 113.3f, 18.15f, -1.65f}, {500.0f, 113.1f, 16.2f, -1.5f}, {505.0f, 111.95f, 14.7f, -1.4f},
  {510.0f, 110.8f, 13.2f, -1.3f}, {515.0f, 108.65f, 10.9f, -1.25f}, {520.0f, 106.5f, 8.6f, -1.2f},
  {525.0f, 107.65f, 7.35f, -1.1f}, {530.0f, 108.8f, 6.1f, -1.0f}, {535.0f, 107.05f, 5.15f, -0.75f},
  {540.0f, 105.3f, 4.2f, -0.5f}, {545.0f, 104.85f, 3.05f, -0.4f}, {550.0f, 104.4f, 1.9f, -0.3f},
  {555.0f, 102.2f, 0.95f, -0.15f}, {560.0f, 100.0f, 0.0f, 0.0f}, {565.0f, 98.0f, -0.8f, 0.1f},
  {570.0f, 96.0f, -1.6f, 0.2f}, {575.0f, 95.55f, -2.55f, 0.35f}, {580.0f, 95.1f, -3.5f, 0.5f},
  {585.0f, 92.1f, -3.5f, 1.3f}, {590.0f, 89.1f, -3.5f, 2.1f}, {595.0f, 89.8f, -4.65f, 2.65f},
  {600.0f, 90.5f, -5.8f, 3.2f}, {605.0f, 90.4f, -6.5f, 3.65f}, {610.0f, 90.3f, -7.2f, 4.1f},
  {615.0f, 89.35f, -7.9f, 4.4f}, {620.0f, 88.4f, -8.6f, 4.7f}, {625.0f, 86.2f, -9.05f, 4.9f},
  {630.0f, 84.0f, -9.5f, 5.1f}, {635.0f, 84.55f, -10.2f, 5.9f}, {640.0f, 85.1f, -10.9f, 6.7f},
  {645.0f, 83.5f, -10.8f, 7.0f}, {650.0f, 81.9f, -10.7f, 7.3f}, {655.0f, 82.25f, -11.35f, 7.95f},
  {660.0f, 82.6f, -12.0f, 8.6f}, {665.0f, 83.75f, -13.0f, 9.2f}, {670.0f, 84.9f, -14.0f, 9.8f},
  {675.0f, 83.1f, -13.8f, 10.0f}, {680.0f, 81.3f, -13.6f, 10.2f}, {685.0f, 76.6f, -12.8f, 9.25f},
  {690.0f, 71.9f, -12.0f, 8.3f}, {695.0f, 73.1f, -12.65f, 8.95f}, {700.0f, 74.3f, -13.3f, 9.6f},
  {705.0f, 75.35f, -13.1f, 9.05f}, {710.0f, 76.4f, -12.9f, 8.5f}, {715.0f, 69.85f, -11.75f, 7.75f},
  {720.0f, 63.3f, -10.6f, 7.0f}, {725.0f, 67.5f, -11.1f, 7.3f}, {730.0f, 71.7f, -11.6f, 7.6f},
  {735.0f, 74.35f, -11.9f, 7.8f}, {740.0f, 77.0f, -12.2f, 8.0f}, {745.0f, 71.1f, -11.2f, 7.35f},
  {750.0f, 65.2f, -10.2f, 6.7f}, {755.0f, 56.45f, -9.0f, 5.95f}, {760.0f, 47.7f, -7.8f, 5.2f},
  {765.0f, 58.15f, -9.5f, 6.3f}, {770.0f, 68.6f, -11.2f, 7.4f}, {775.0f, 66.8f, -10.8f, 7.1f},
  {780.0f, 65.0f, -10.4f, 6.8f}, {785.0f, 65.5f, -10.5f, 6.9f}, {790.0f, 66.0f, -10.6f, 7.0f},
  {795.0f, 63.5f, -10.15f, 6.7f}, {800.0f, 61.0f, -9.7f, 6.4f}, {805.0f, 57.15f, -9.0f, 5.95f},
  {810.0f, 53.3f, -8.3f, 5.5f}, {815.0f, 56.1f, -8.8f, 5.8f}, {820.0f, 58.9f, -9.3f, 6.1f},
  {825.0f, 60.4f, -9.55f, 6.3f}, {830.0f, 61.9f, -9.8f, 6.5f},
};

/**
 * @brief CIE daylight coefficients M1/M2 for chromaticity (x, y).
 *
 * Rounded to three decimals, as CIE 15:2004 recommends for the
 * internationally agreed D-series spectra.
 */
inline std::pair<float, float> daylight_coefficients(float x, float y) {
  const double M = 0.0241 + 0.2562 * x - 0.7341 * y;
  const double m1 = (-1.3515 - 1.7703 * x + 5.9114 * y) / M;
  const double m2 = (0.0300 - 31.4424 * x + 30.0717 * y) / M;
  const auto round3 = [](double v) { return static_cast<float>(std::round(v * 1000.0) / 1000.0); };
  return {round3(m1), round3(m2)};
}

}  // namespace details

/**
 * @brief CIE D-series daylight SPD for chromaticity (x, y).
 *
 * S(λ) = S0(λ) + M1·S1(λ) + M2·S2(λ) on the 300-830 nm / 5 nm basis
 * grid. The result satisfies S(560) = 100; it is not scaled to a
 * particular luminance.
 */
inline spd d_series_xy(float x, float y) {
  const auto [m1, m2] = details::daylight_coefficients(x, y);
  const double d1 = m1;
  const double d2 = m2;
  std::vector<float> values;
  values.reserve(std::size(details::DAYLIGHT_BASIS));
  for (const auto& sample : details::DAYLIGHT_BASIS) {
    values.push_back(static_cast<float>(sample.s0 + d1 * sample.s1 + d2 * sample.s2));
  }
  return spd(300.0f, 5.0f, std::move(values));
}

/**
 * @brief CIE D-series daylight SPD for a correlated colour temperature.
 *
 * The chromaticity comes from cie_d_xy(cct), so the input is clamped to
 * [4000, 25000] K. Use d_series_xy() when the daylight chromaticity is
 * known directly instead of derived from a CCT.
 */
inline spd d_series(float cct) {
  const auto xy = cie_d_xy(cct);
  return d_series_xy(xy.first, xy.second);
}

}  // namespace colorcpp::algorithms::spectral::illuminants
