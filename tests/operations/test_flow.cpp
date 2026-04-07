/**
 * @file test_flow.cpp
 * @brief Tests for the colorflow module (pipeline, theme, scheme, export).
 */

#include <gtest/gtest.h>
#include <colorcpp/operations/flow.hpp>
#include <string>

using namespace colorcpp;
using namespace colorcpp::operations::flow;

TEST(FlowPipeline, MakeFromHex) {
    auto color = make(0xff6347ff);
    EXPECT_EQ(color.color().r(), 0xff);
    EXPECT_EQ(color.color().g(), 0x63);
    EXPECT_EQ(color.color().b(), 0x47);
}

TEST(FlowPipeline, MakeFromRgba8) {
    core::rgba8_t input{100, 150, 200, 255};
    auto color = make(input);
    EXPECT_EQ(color.color().r(), 100);
    EXPECT_EQ(color.color().g(), 150);
}

TEST(FlowPipeline, MakeRgb) {
    auto color = make_rgb(255, 128, 64);
    EXPECT_EQ(color.color().r(), 255);
    EXPECT_EQ(color.color().g(), 128);
    EXPECT_EQ(color.color().b(), 64);
}

TEST(FlowPipeline, Lighten) {
    auto original = make(0x808080ff);
    auto lightened = original.lighten(50);
    EXPECT_GE(static_cast<int>(lightened.color().r()), static_cast<int>(original.color().r()));
}

TEST(FlowPipeline, Darken) {
    auto original = make(0x808080ff);
    auto darkened = original.darken(50);
    EXPECT_LE(static_cast<int>(darkened.color().r()), static_cast<int>(original.color().r()));
}

TEST(FlowPipeline, RotateHue) {
    auto red = make_rgb(255, 0, 0);
    auto rotated = red.rotate_hue(180);
    EXPECT_TRUE(rotated.color().r() != 255 || rotated.color().b() != 0);
}

TEST(FlowPipeline, ToHex) {
    auto color = make_rgb(255, 99, 71);
    EXPECT_EQ(color.to_hex(), "#ff6347");
}

TEST(FlowPipeline, ToPalette) {
    auto color = make_rgb(255, 100, 100);
    auto palette = color.to_palette(4);
    EXPECT_EQ(palette.size(), 4u);
    EXPECT_EQ(palette[0].r(), 255);
}

TEST(FlowPipeline, Blend) {
    auto red = make_rgb(255, 0, 0);
    auto blue = make_rgb(0, 0, 255);
    auto blended = red.blend(blue, 0.5f);
    EXPECT_GT(blended.color().r(), 0);
    EXPECT_GT(blended.color().b(), 0);
}

TEST(FlowPipeline, MethodChaining) {
    auto result = make_rgb(200, 100, 50).lighten(20).rotate_hue(30).fade(80).to_rgba8();
    EXPECT_GT(result.a(), 0);
    EXPECT_LT(result.a(), 255);
}

TEST(FlowTheme, FromSeedHex) {
    auto theme = flow_theme::from_seed(0x6750A7ff);
    EXPECT_EQ(theme.seed().r(), 0x67);
}

TEST(FlowTheme, LightModePalette) {
    auto theme = flow_theme::from_seed(0x6750A7ff);
    auto light = theme.light_mode();
    EXPECT_EQ(light.primary.r(), 0x67);
    EXPECT_GT(light.surface.r(), 200);
}

TEST(FlowTheme, DarkModePalette) {
    auto theme = flow_theme::from_seed(0x6750A7ff);
    auto dark = theme.dark_mode();
    EXPECT_LT(dark.surface.r(), 50);
    EXPECT_GT(dark.on_surface.r(), 200);
}

TEST(FlowScheme, Complementary) {
    auto base = make_rgb(255, 0, 0).color();
    auto scheme = generate_scheme(base, scheme_type::complementary);
    EXPECT_EQ(scheme.size(), 2u);
}

TEST(FlowScheme, Triadic) {
    auto base = make_rgb(255, 0, 0).color();
    auto scheme = generate_scheme(base, scheme_type::triadic);
    EXPECT_EQ(scheme.size(), 3u);
}

TEST(FlowScheme, Analogous) {
    auto base = make_rgb(255, 0, 0).color();
    auto scheme = generate_scheme(base, scheme_type::analogous, 5);
    EXPECT_EQ(scheme.size(), 5u);
}

TEST(FlowExport, ToCssVariables) {
    auto theme = flow_theme::from_seed(0x6750A7ff);
    auto light = theme.light_mode();
    auto css = export_::to_css_variables(light);
    EXPECT_NE(css.find(":root"), std::string::npos);
    EXPECT_NE(css.find("#6750a7"), std::string::npos);
}

TEST(FlowExport, ToScssVariables) {
    auto theme = flow_theme::from_seed(0x6750A7ff);
    auto light = theme.light_mode();
    auto scss = export_::to_scss_variables(light);
    EXPECT_NE(scss.find("$primary:"), std::string::npos);
}

TEST(FlowExport, ToFlutterCode) {
    auto theme = flow_theme::from_seed(0x6750A7ff);
    auto dark = theme.dark_mode();
    auto flutter = export_::to_flutter_code(dark);
    EXPECT_NE(flutter.find("class AppColors"), std::string::npos);
}
