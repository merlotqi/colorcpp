/**
 * @file test_binary_io.cpp
 * @brief Unit tests for binary_io: LUT data structures and .cube format.
 */

#include <gtest/gtest.h>

#include <colorcpp/io/binary_io.hpp>
#include <sstream>
#include <string>

using namespace colorcpp::io::binary_io;

// ===== LUT 3D Tests =====

TEST(Lut3d, DefaultConstruction) {
  lut3d lut;
  EXPECT_EQ(lut.size, 0u);
  EXPECT_FALSE(lut.valid());
}

TEST(Lut3d, ValidConstruction) {
  lut3d lut;
  lut.size = 2;
  lut.data.resize(8, {0.0f, 0.0f, 0.0f});
  EXPECT_TRUE(lut.valid());
}

TEST(Lut3d, IdentityLut) {
  // Build a 2x2x2 identity LUT
  lut3d lut;
  lut.size = 2;
  lut.data.resize(8);
  for (std::size_t r = 0; r < 2; ++r) {
    for (std::size_t g = 0; g < 2; ++g) {
      for (std::size_t b = 0; b < 2; ++b) {
        lut.at(r, g, b) = {static_cast<float>(r), static_cast<float>(g), static_cast<float>(b)};
      }
    }
  }

  // Applying identity LUT should return same values
  auto result = apply(lut, 0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(result[0], 0.0f);
  EXPECT_FLOAT_EQ(result[1], 0.0f);
  EXPECT_FLOAT_EQ(result[2], 0.0f);

  result = apply(lut, 1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(result[0], 1.0f);
  EXPECT_FLOAT_EQ(result[1], 1.0f);
  EXPECT_FLOAT_EQ(result[2], 1.0f);

  // Midpoint should interpolate
  result = apply(lut, 0.5f, 0.5f, 0.5f);
  EXPECT_NEAR(result[0], 0.5f, 1e-5f);
  EXPECT_NEAR(result[1], 0.5f, 1e-5f);
  EXPECT_NEAR(result[2], 0.5f, 1e-5f);
}

TEST(Lut3d, InvalidLutPassthrough) {
  lut3d lut;  // invalid, size = 0
  auto result = apply(lut, 0.5f, 0.3f, 0.1f);
  EXPECT_FLOAT_EQ(result[0], 0.5f);
  EXPECT_FLOAT_EQ(result[1], 0.3f);
  EXPECT_FLOAT_EQ(result[2], 0.1f);
}

// ===== LUT 1D Tests =====

TEST(Lut1d, DefaultConstruction) {
  lut1d lut;
  EXPECT_EQ(lut.size, 0u);
  EXPECT_FALSE(lut.valid());
}

TEST(Lut1d, IdentityLut) {
  lut1d lut;
  lut.size = 3;
  lut.r = {0.0f, 0.5f, 1.0f};
  lut.g = {0.0f, 0.5f, 1.0f};
  lut.b = {0.0f, 0.5f, 1.0f};
  EXPECT_TRUE(lut.valid());

  auto result = apply(lut, 0.5f, 0.5f, 0.5f);
  EXPECT_NEAR(result[0], 0.5f, 1e-5f);
  EXPECT_NEAR(result[1], 0.5f, 1e-5f);
  EXPECT_NEAR(result[2], 0.5f, 1e-5f);
}

// ===== .cube Format Tests =====

TEST(Cube, Read3dCube) {
  const char* cube_data = R"(TITLE "Test 3D LUT"
LUT_3D_SIZE 2
0.0 0.0 0.0
0.0 0.0 1.0
0.0 1.0 0.0
0.0 1.0 1.0
1.0 0.0 0.0
1.0 0.0 1.0
1.0 1.0 0.0
1.0 1.0 1.0
)";

  std::istringstream ss(cube_data);
  auto result = cube::read_3d(ss);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->size, 2u);
  EXPECT_EQ(result->title, "Test 3D LUT");
  EXPECT_TRUE(result->valid());

  // Check corner values
  EXPECT_FLOAT_EQ(result->at(0, 0, 0)[0], 0.0f);
  EXPECT_FLOAT_EQ(result->at(1, 1, 1)[0], 1.0f);
}

TEST(Cube, Read1dCube) {
  const char* cube_data = R"(TITLE "Test 1D LUT"
LUT_1D_SIZE 3
0.0 0.0 0.0
0.5 0.5 0.5
1.0 1.0 1.0
)";

  std::istringstream ss(cube_data);
  auto result = cube::read_1d(ss);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->size, 3u);
  EXPECT_EQ(result->title, "Test 1D LUT");
  EXPECT_TRUE(result->valid());
}

TEST(Cube, ReadAutoDetect3d) {
  const char* cube_data = R"(LUT_3D_SIZE 2
0.0 0.0 0.0
0.0 0.0 1.0
0.0 1.0 0.0
0.0 1.0 1.0
1.0 0.0 0.0
1.0 0.0 1.0
1.0 1.0 0.0
1.0 1.0 1.0
)";

  std::istringstream ss(cube_data);
  auto result = cube::read(ss);
  ASSERT_TRUE(std::holds_alternative<lut3d>(result));
  auto& lut = std::get<lut3d>(result);
  EXPECT_EQ(lut.size, 2u);
}

TEST(Cube, ReadWithDomain) {
  const char* cube_data = R"(LUT_1D_SIZE 2
DOMAIN_MIN 0.0 0.0 0.0
DOMAIN_MAX 2.0 2.0 2.0
0.0 0.0 0.0
2.0 2.0 2.0
)";

  std::istringstream ss(cube_data);
  auto result = cube::read_1d(ss);
  ASSERT_TRUE(result.has_value());
  EXPECT_FLOAT_EQ(result->domain_max[0], 2.0f);
}

TEST(Cube, ReadWithComments) {
  const char* cube_data = R"(# This is a comment
TITLE "Commented LUT"
# Another comment
LUT_3D_SIZE 2
0.0 0.0 0.0
0.0 0.0 1.0
0.0 1.0 0.0
0.0 1.0 1.0
1.0 0.0 0.0
1.0 0.0 1.0
1.0 1.0 0.0
1.0 1.0 1.0
)";

  std::istringstream ss(cube_data);
  auto result = cube::read_3d(ss);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->title, "Commented LUT");
}

TEST(Cube, Write3dRoundTrip) {
  // Build a 2x2x2 LUT
  lut3d lut;
  lut.size = 2;
  lut.title = "RoundTrip";
  lut.data.resize(8);
  for (std::size_t r = 0; r < 2; ++r) {
    for (std::size_t g = 0; g < 2; ++g) {
      for (std::size_t b = 0; b < 2; ++b) {
        lut.at(r, g, b) = {static_cast<float>(r) * 0.5f, static_cast<float>(g) * 0.5f, static_cast<float>(b) * 0.5f};
      }
    }
  }

  // Write
  std::ostringstream out;
  ASSERT_TRUE(cube::write(out, lut));

  // Read back
  std::istringstream in(out.str());
  auto result = cube::read_3d(in);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->size, 2u);
  EXPECT_EQ(result->title, "RoundTrip");

  // Verify data matches
  for (std::size_t r = 0; r < 2; ++r) {
    for (std::size_t g = 0; g < 2; ++g) {
      for (std::size_t b = 0; b < 2; ++b) {
        const auto& orig = lut.at(r, g, b);
        const auto& read = result->at(r, g, b);
        EXPECT_FLOAT_EQ(read[0], orig[0]);
        EXPECT_FLOAT_EQ(read[1], orig[1]);
        EXPECT_FLOAT_EQ(read[2], orig[2]);
      }
    }
  }
}

TEST(Cube, Write1dRoundTrip) {
  lut1d lut;
  lut.size = 4;
  lut.title = "1D RoundTrip";
  lut.r = {0.0f, 0.33f, 0.66f, 1.0f};
  lut.g = {0.0f, 0.25f, 0.75f, 1.0f};
  lut.b = {0.0f, 0.5f, 0.5f, 1.0f};

  std::ostringstream out;
  ASSERT_TRUE(cube::write(out, lut));

  std::istringstream in(out.str());
  auto result = cube::read_1d(in);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->size, 4u);
  EXPECT_EQ(result->title, "1D RoundTrip");
}

TEST(Cube, InvalidLutWriteFails) {
  lut3d lut;  // invalid
  std::ostringstream out;
  EXPECT_FALSE(cube::write(out, lut));
}
