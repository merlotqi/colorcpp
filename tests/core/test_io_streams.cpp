/**
 * @file test_io_streams.cpp
 * @brief Stream extraction contract for core color types (operator>>).
 */

#include <gtest/gtest.h>

#include <colorcpp/core/cielab.hpp>
#include <colorcpp/core/cmyk.hpp>
#include <colorcpp/core/rgb.hpp>
#include <sstream>

namespace colorcpp::core::test {

TEST(IoStreamParseTest, OmittedAlphaDefaultsToOpaque) {
  {
    std::istringstream is("10 20 30");
    rgba8_t c;
    is >> c;
    EXPECT_FALSE(is.fail());
    EXPECT_EQ(c.r(), 10);
    EXPECT_EQ(c.g(), 20);
    EXPECT_EQ(c.b(), 30);
    EXPECT_EQ(c.a(), 255);
  }
  {
    std::istringstream is("0.1 0.2 0.3");
    rgbaf_t c;
    is >> c;
    EXPECT_FALSE(is.fail());
    EXPECT_FLOAT_EQ(c.a(), 1.0f);
  }
}

TEST(IoStreamParseTest, OmittedNonAlphaLastChannelFails) {
  {
    std::istringstream is("10 20");
    rgb8_t c;
    is >> c;
    EXPECT_TRUE(is.fail());
  }
  {
    std::istringstream is("lab(50, 10)");
    cielab_t c;
    is >> c;
    EXPECT_TRUE(is.fail());
  }
  {
    std::istringstream is("10 20 30");
    cmyk_float_t c;
    is >> c;
    EXPECT_TRUE(is.fail());
  }
}

TEST(IoStreamParseTest, FullInputParsesAllChannels) {
  std::istringstream is("1 2 3 4");
  rgba8_t c;
  is >> c;
  EXPECT_FALSE(is.fail());
  EXPECT_EQ(c.r(), 1);
  EXPECT_EQ(c.g(), 2);
  EXPECT_EQ(c.b(), 3);
  EXPECT_EQ(c.a(), 4);
}

TEST(IoStreamParseTest, OutOfRangeInputSetsFailbitWithoutThrowing) {
  {
    // Float model: 300.0 is far outside rgbaf's [0, 1].
    std::istringstream is("300 0 0");
    rgbaf_t c;
    EXPECT_NO_THROW(is >> c);
    EXPECT_TRUE(is.fail());
  }
  {
    std::istringstream is("1.5 0.5 0.5");
    rgbaf_t c;
    EXPECT_NO_THROW(is >> c);
    EXPECT_TRUE(is.fail());
  }
  {
    // Integral model: 300 exceeds rgb8's [0, 255] and must not wrap to 44.
    std::istringstream is("300 0 0");
    rgb8_t c;
    EXPECT_NO_THROW(is >> c);
    EXPECT_TRUE(is.fail());
  }
}

TEST(IoStreamFormatTest, HexOutputRoundsToNearest) {
  std::ostringstream os;
  os << std::hex;
  rgbaf_t c(0.5f, 0.1f, 0.2f, 1.0f);
  os << c;
  // 0.5→128 (0x80), 0.1→25.5→26 (0x1a), 0.2→51 (0x33), 1.0→255 (0xff); truncation would give 7f/19/33.
  EXPECT_EQ(os.str(), "#801a33ff");
}

TEST(IoStreamFormatTest, HexOutputOnlyForRgbFamilies) {
  {
    std::ostringstream os;
    os << std::hex;
    cielab_t c(50, 10, 20);
    os << c;
    EXPECT_EQ(os.str(), "lab(50, 10, 20)");
  }
  {
    // Positive control: RGB family keeps hex output.
    std::ostringstream os;
    os << std::hex;
    rgba8_t c(100, 150, 200, 250);
    os << c;
    EXPECT_EQ(os.str(), "#6496c8fa");
  }
}

}  // namespace colorcpp::core::test
