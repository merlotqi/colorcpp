#include <array>

#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

namespace colorcpp::algorithms::test {

using namespace core;
using namespace delta_e;

namespace {

float delta_e_ok_scalar_reference(const oklab_t& a, const oklab_t& b) {
  return delta_e::details::delta_e_ok_scalar(a, b);
}

}  // namespace

TEST(DeltaEOkSimdTest, FastPathAvailabilityMatchesCurrentBackend) {
  EXPECT_EQ(delta_e::details::has_delta_e_ok_fast_path(), colorcpp::detail::simd::has_sse2);
}

TEST(DeltaEOkSimdTest, OklabInputMatchesScalarReference) {
  constexpr std::array<oklab_t, 4> kA = {
      oklab_t{0.0f, 0.0f, 0.0f},
      oklab_t{0.62f, 0.11f, -0.09f},
      oklab_t{0.95f, -0.02f, 0.04f},
      oklab_t{0.37f, -0.21f, 0.19f},
  };
  constexpr std::array<oklab_t, 4> kB = {
      oklab_t{0.0f, 0.0f, 0.0f},
      oklab_t{0.41f, -0.08f, 0.15f},
      oklab_t{0.73f, 0.19f, -0.13f},
      oklab_t{0.18f, 0.22f, -0.17f},
  };

  for (std::size_t i = 0; i < kA.size(); ++i) {
    const float actual = delta_e_ok(kA[i], kB[i]);
    const float expected = delta_e_ok_scalar_reference(kA[i], kB[i]);
    EXPECT_NEAR(actual, expected, 1e-6f) << i;
  }
}

TEST(DeltaEOkSimdTest, InternalFastPathMatchesScalarReference) {
  const oklab_t a{0.78f, -0.14f, 0.18f};
  const oklab_t b{0.43f, 0.17f, -0.12f};

  float fast = 0.0f;
  if (delta_e::details::try_delta_e_ok_fast_path(fast, a, b)) {
    EXPECT_NEAR(fast, delta_e_ok_scalar_reference(a, b), 1e-6f);
  } else {
    SUCCEED();
  }
}

TEST(DeltaEOkSimdTest, MixedInputPathStillMatchesOklabReference) {
  const rgbaf_t a{0.82f, 0.23f, 0.41f, 1.0f};
  const rgbaf_t b{0.15f, 0.54f, 0.79f, 1.0f};

  const auto ok_a = operations::conversion::color_cast<oklab_t>(a);
  const auto ok_b = operations::conversion::color_cast<oklab_t>(b);

  EXPECT_NEAR(delta_e_ok(a, b), delta_e_ok_scalar_reference(ok_a, ok_b), 1e-5f);
}

}  // namespace colorcpp::algorithms::test
