/**
 * @file planckian.hpp
 * @brief Planckian (blackbody) locus by spectral integration.
 *
 * Integrates Planck's law against the embedded CIE 1931 2° standard observer
 * colour matching functions (5 nm steps, 360–830 nm) with the trapezoidal
 * rule, then converts the resulting tristimulus values to CIE 1960 UCS uv
 * chromaticity. The 5 nm sampling agrees with 1 nm integration to ~3e-7 in
 * uv (validated against colour-science).
 *
 * @see CIE 15:2004, Appendix E (Planck's equation for standard air).
 */

#pragma once

#include <cmath>
#include <colorcpp/algorithms/color_temperature/common.hpp>
#include <cstddef>
#include <utility>
#include <vector>

namespace colorcpp::algorithms::color_temperature {

namespace details {

// CIE 1931 2° standard observer colour matching functions, 5 nm, 360–830 nm.
// Source: colour-science CIE 1931 2 Degree Standard Observer (1 nm dataset
// subsampled at 5 nm; verified to reproduce 1 nm chromaticities to ~3e-7).
struct cmf_sample {
  float nm;
  float x_bar;
  float y_bar;
  float z_bar;
};

inline constexpr cmf_sample CMF_1931_2DEG[] = {
    {360.0f, 0.0001299000f, 0.0000039170f, 0.0006061000f}, {365.0f, 0.0002321000f, 0.0000069650f, 0.0010860000f},
    {370.0f, 0.0004149000f, 0.0000123900f, 0.0019460000f}, {375.0f, 0.0007416000f, 0.0000220200f, 0.0034860000f},
    {380.0f, 0.0013680000f, 0.0000390000f, 0.0064500010f}, {385.0f, 0.0022360000f, 0.0000640000f, 0.0105499900f},
    {390.0f, 0.0042430000f, 0.0001200000f, 0.0200500100f}, {395.0f, 0.0076500000f, 0.0002170000f, 0.0362100000f},
    {400.0f, 0.0143100000f, 0.0003960000f, 0.0678500100f}, {405.0f, 0.0231900000f, 0.0006400000f, 0.1102000000f},
    {410.0f, 0.0435100000f, 0.0012100000f, 0.2074000000f}, {415.0f, 0.0776300000f, 0.0021800000f, 0.3713000000f},
    {420.0f, 0.1343800000f, 0.0040000000f, 0.6456000000f}, {425.0f, 0.2147700000f, 0.0073000000f, 1.0390501000f},
    {430.0f, 0.2839000000f, 0.0116000000f, 1.3856000000f}, {435.0f, 0.3285000000f, 0.0168400000f, 1.6229600000f},
    {440.0f, 0.3482800000f, 0.0230000000f, 1.7470600000f}, {445.0f, 0.3480600000f, 0.0298000000f, 1.7826000000f},
    {450.0f, 0.3362000000f, 0.0380000000f, 1.7721100000f}, {455.0f, 0.3187000000f, 0.0480000000f, 1.7441000000f},
    {460.0f, 0.2908000000f, 0.0600000000f, 1.6692000000f}, {465.0f, 0.2511000000f, 0.0739000000f, 1.5281000000f},
    {470.0f, 0.1953600000f, 0.0909800000f, 1.2876400000f}, {475.0f, 0.1421000000f, 0.1126000000f, 1.0419000000f},
    {480.0f, 0.0956400000f, 0.1390200000f, 0.8129501000f}, {485.0f, 0.0579500100f, 0.1693000000f, 0.6162000000f},
    {490.0f, 0.0320100000f, 0.2080200000f, 0.4651800000f}, {495.0f, 0.0147000000f, 0.2586000000f, 0.3533000000f},
    {500.0f, 0.0049000000f, 0.3230000000f, 0.2720000000f}, {505.0f, 0.0024000000f, 0.4073000000f, 0.2123000000f},
    {510.0f, 0.0093000000f, 0.5030000000f, 0.1582000000f}, {515.0f, 0.0291000000f, 0.6082000000f, 0.1117000000f},
    {520.0f, 0.0632700000f, 0.7100000000f, 0.0782499900f}, {525.0f, 0.1096000000f, 0.7932000000f, 0.0572500100f},
    {530.0f, 0.1655000000f, 0.8620000000f, 0.0421600000f}, {535.0f, 0.2257499000f, 0.9148501000f, 0.0298400000f},
    {540.0f, 0.2904000000f, 0.9540000000f, 0.0203000000f}, {545.0f, 0.3597000000f, 0.9803000000f, 0.0134000000f},
    {550.0f, 0.4334499000f, 0.9949501000f, 0.0087499990f}, {555.0f, 0.5120501000f, 1.0000000000f, 0.0057499990f},
    {560.0f, 0.5945000000f, 0.9950000000f, 0.0039000000f}, {565.0f, 0.6784000000f, 0.9786000000f, 0.0027499990f},
    {570.0f, 0.7621000000f, 0.9520000000f, 0.0021000000f}, {575.0f, 0.8425000000f, 0.9154000000f, 0.0018000000f},
    {580.0f, 0.9163000000f, 0.8700000000f, 0.0016500010f}, {585.0f, 0.9786000000f, 0.8163000000f, 0.0014000000f},
    {590.0f, 1.0263000000f, 0.7570000000f, 0.0011000000f}, {595.0f, 1.0567000000f, 0.6949000000f, 0.0010000000f},
    {600.0f, 1.0622000000f, 0.6310000000f, 0.0008000000f}, {605.0f, 1.0456000000f, 0.5668000000f, 0.0006000000f},
    {610.0f, 1.0026000000f, 0.5030000000f, 0.0003400000f}, {615.0f, 0.9384000000f, 0.4412000000f, 0.0002400000f},
    {620.0f, 0.8544499000f, 0.3810000000f, 0.0001900000f}, {625.0f, 0.7514000000f, 0.3210000000f, 0.0001000000f},
    {630.0f, 0.6424000000f, 0.2650000000f, 0.0000500000f}, {635.0f, 0.5419000000f, 0.2170000000f, 0.0000300000f},
    {640.0f, 0.4479000000f, 0.1750000000f, 0.0000200000f}, {645.0f, 0.3608000000f, 0.1382000000f, 0.0000100000f},
    {650.0f, 0.2835000000f, 0.1070000000f, 0.0000000000f}, {655.0f, 0.2187000000f, 0.0816000000f, 0.0000000000f},
    {660.0f, 0.1649000000f, 0.0610000000f, 0.0000000000f}, {665.0f, 0.1212000000f, 0.0445800000f, 0.0000000000f},
    {670.0f, 0.0874000000f, 0.0320000000f, 0.0000000000f}, {675.0f, 0.0636000000f, 0.0232000000f, 0.0000000000f},
    {680.0f, 0.0467700000f, 0.0170000000f, 0.0000000000f}, {685.0f, 0.0329000000f, 0.0119200000f, 0.0000000000f},
    {690.0f, 0.0227000000f, 0.0082100000f, 0.0000000000f}, {695.0f, 0.0158400000f, 0.0057230000f, 0.0000000000f},
    {700.0f, 0.0113591600f, 0.0041020000f, 0.0000000000f}, {705.0f, 0.0081109160f, 0.0029290000f, 0.0000000000f},
    {710.0f, 0.0057903460f, 0.0020910000f, 0.0000000000f}, {715.0f, 0.0041094570f, 0.0014840000f, 0.0000000000f},
    {720.0f, 0.0028993270f, 0.0010470000f, 0.0000000000f}, {725.0f, 0.0020491900f, 0.0007400000f, 0.0000000000f},
    {730.0f, 0.0014399710f, 0.0005200000f, 0.0000000000f}, {735.0f, 0.0009999493f, 0.0003611000f, 0.0000000000f},
    {740.0f, 0.0006900786f, 0.0002492000f, 0.0000000000f}, {745.0f, 0.0004760213f, 0.0001719000f, 0.0000000000f},
    {750.0f, 0.0003323011f, 0.0001200000f, 0.0000000000f}, {755.0f, 0.0002348261f, 0.0000848000f, 0.0000000000f},
    {760.0f, 0.0001661505f, 0.0000600000f, 0.0000000000f}, {765.0f, 0.0001174130f, 0.0000424000f, 0.0000000000f},
    {770.0f, 0.0000830753f, 0.0000300000f, 0.0000000000f}, {775.0f, 0.0000587065f, 0.0000212000f, 0.0000000000f},
    {780.0f, 0.0000415099f, 0.0000149900f, 0.0000000000f}, {785.0f, 0.0000293533f, 0.0000106000f, 0.0000000000f},
    {790.0f, 0.0000206738f, 0.0000074657f, 0.0000000000f}, {795.0f, 0.0000145598f, 0.0000052578f, 0.0000000000f},
    {800.0f, 0.0000102540f, 0.0000037029f, 0.0000000000f}, {805.0f, 0.0000072215f, 0.0000026078f, 0.0000000000f},
    {810.0f, 0.0000050859f, 0.0000018366f, 0.0000000000f}, {815.0f, 0.0000035817f, 0.0000012934f, 0.0000000000f},
    {820.0f, 0.0000025225f, 0.0000009109f, 0.0000000000f}, {825.0f, 0.0000017765f, 0.0000006415f, 0.0000000000f},
    {830.0f, 0.0000012511f, 0.0000004518f, 0.0000000000f},
};

// Planck's law spectral radiance (CIE 15:2004 constants, n = 1);
// absolute scale cancels in chromaticity so only the shape matters.
inline constexpr float PLANCK_C1 = 3.741771e-16f;
inline constexpr float PLANCK_C2 = 1.4388e-2f;

}  // namespace details

/**
 * @brief Planckian locus chromaticity at a thermodynamic temperature.
 *
 * Spectral radiance S(λ) ∝ λ^-5 / (exp(c2 / (λ T)) - 1) integrated against
 * the CIE 1931 2° CMFs (trapezoidal rule) yields XYZ; the result is returned
 * as CIE 1960 UCS (u, v).
 *
 * @param cct Temperature in kelvin (> 0).
 * @return CIE 1960 UCS uv chromaticity of the blackbody at @p cct.
 */
inline std::pair<float, float> planck_uv(float cct) {
  using namespace details;

  constexpr std::size_t n = sizeof(CMF_1931_2DEG) / sizeof(CMF_1931_2DEG[0]);

  float s_prev = 0.0f;
  float lambda_prev = 0.0f;
  float X = 0.0f, Y = 0.0f, Z = 0.0f;
  for (std::size_t i = 0; i < n; ++i) {
    const float lambda = CMF_1931_2DEG[i].nm * 1e-9f;
    const float s = PLANCK_C1 * std::pow(lambda, -5.0f) / std::expm1(PLANCK_C2 / (lambda * cct));
    if (i > 0) {
      const float dl = lambda - lambda_prev;
      X += 0.5f * (s * CMF_1931_2DEG[i].x_bar + s_prev * CMF_1931_2DEG[i - 1].x_bar) * dl;
      Y += 0.5f * (s * CMF_1931_2DEG[i].y_bar + s_prev * CMF_1931_2DEG[i - 1].y_bar) * dl;
      Z += 0.5f * (s * CMF_1931_2DEG[i].z_bar + s_prev * CMF_1931_2DEG[i - 1].z_bar) * dl;
    }
    s_prev = s;
    lambda_prev = lambda;
  }
  return xyz_to_uv1960(core::xyz_t{X, Y, Z});
}

/**
 * @brief Planckian locus table for Ohno (2013) CCT lookup.
 *
 * Geometrically spaced temperatures from @p start to @p end (Ohno 2014,
 * section "Planckian table"): after each row the step multiplier decays
 * linearly from @p spacing toward 1 + (spacing - 1) / 10 at high CCT, so
 * density increases where the locus curves most. Rows @c end - 1 and
 * @c end are always appended.
 */
class planckian_table {
 public:
  /**
   * @brief Build the table.
   * @param start Lowest temperature in kelvin (inclusive).
   * @param end Highest temperature in kelvin (inclusive).
   * @param spacing Geometric step multiplier (> 1; smaller = more precise).
   */
  planckian_table(float start = 1000.0f, float end = 25000.0f, float spacing = 1.005f) {
    t_.reserve(1024);
    u_.reserve(1024);
    v_.reserve(1024);

    auto push = [this](float t) {
      const auto uv = planck_uv(t);
      t_.push_back(t);
      u_.push_back(uv.first);
      v_.push_back(uv.second);
    };

    push(start);
    push(start + 1.0f);

    float next_t = start + 1.0f;
    float next_spacing = spacing;
    while (true) {
      next_t *= next_spacing;
      if (next_t >= end) break;
      push(next_t);
      // Slightly decrease step-size for higher CCT (Ohno 2014).
      float D = (next_t - 1000.0f) / 99000.0f;
      D = D < 0.0f ? 0.0f : (D > 1.0f ? 1.0f : D);
      next_spacing = spacing * (1.0f - D) + (1.0f + (spacing - 1.0f) / 10.0f) * D;
    }
    push(end - 1.0f);
    push(end);
  }

  std::size_t size() const { return t_.size(); }
  float temperature(std::size_t i) const { return t_[i]; }
  float u(std::size_t i) const { return u_[i]; }
  float v(std::size_t i) const { return v_[i]; }

 private:
  std::vector<float> t_, u_, v_;
};

}  // namespace colorcpp::algorithms::color_temperature
