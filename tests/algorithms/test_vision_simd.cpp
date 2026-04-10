#include <gtest/gtest.h>

#include <array>
#include <colorcpp/colorcpp.hpp>

namespace colorcpp::algorithms::vision::test {

using namespace core;
using namespace vision;

namespace {

void expect_rgba_near(const rgbaf_t& actual, const rgbaf_t& expected, float tol) {
  EXPECT_NEAR(actual.r(), expected.r(), tol);
  EXPECT_NEAR(actual.g(), expected.g(), tol);
  EXPECT_NEAR(actual.b(), expected.b(), tol);
  EXPECT_NEAR(actual.a(), expected.a(), tol);
}

}  // namespace

TEST(MachadoSimdTest, FastPathAvailabilityMatchesCurrentBackend) {
  EXPECT_EQ(details::has_machado_fast_path(),
            details::machado_fast_path_requested() && colorcpp::detail::simd::has_sse2);
}

TEST(MachadoSimdTest, InternalFastPathMatchesScalarReference) {
  constexpr std::array<rgbaf_t, 3> kSamples = {
      rgbaf_t{0.91f, 0.24f, 0.16f, 0.8f},
      rgbaf_t{0.11f, 0.77f, 0.52f, 0.6f},
      rgbaf_t{0.33f, 0.41f, 0.95f, 1.0f},
  };
  constexpr std::array<float, 3> kSeverities = {0.15f, 0.5f, 1.0f};

  for (const auto& sample : kSamples) {
    for (float severity : kSeverities) {
      rgbaf_t fast{};
      if (details::try_simulate_machado_rgbaf_fast_path(fast, sample, severity, machado_detail::kProtanopia)) {
        const auto expected = details::simulate_machado_rgbaf_scalar(sample, severity, machado_detail::kProtanopia);
        expect_rgba_near(fast, expected, 1e-6f);
      } else {
        SUCCEED();
      }
    }
  }
}

TEST(MachadoSimdTest, PublicApiMatchesScalarReferenceAcrossDeficiencies) {
  constexpr rgbaf_t kSample{0.37f, 0.64f, 0.18f, 0.45f};
  constexpr float kSeverity = 0.6f;

  const auto protan = simulate_protan_machado(kSample, kSeverity);
  const auto protan_expected = details::simulate_machado_rgbaf_scalar(kSample, kSeverity, machado_detail::kProtanopia);
  expect_rgba_near(protan, protan_expected, 1e-6f);

  const auto deutan = simulate_deutan_machado(kSample, kSeverity);
  const auto deutan_expected =
      details::simulate_machado_rgbaf_scalar(kSample, kSeverity, machado_detail::kDeuteranopia);
  expect_rgba_near(deutan, deutan_expected, 1e-6f);

  const auto tritan = simulate_tritan_machado(kSample, kSeverity);
  const auto tritan_expected = details::simulate_machado_rgbaf_scalar(kSample, kSeverity, machado_detail::kTritanopia);
  expect_rgba_near(tritan, tritan_expected, 1e-6f);
}

TEST(MachadoSimdTest, SeverityZeroPreservesExactInput) {
  const rgb8_t input{12, 34, 56};
  EXPECT_EQ(simulate_protan_machado(input, 0.0f), input);
  EXPECT_EQ(simulate_deutan_machado(input, 0.0f), input);
  EXPECT_EQ(simulate_tritan_machado(input, 0.0f), input);
}

}  // namespace colorcpp::algorithms::vision::test
