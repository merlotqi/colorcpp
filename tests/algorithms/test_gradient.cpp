#include <gtest/gtest.h>

#include <cmath>
#include <colorcpp/colorcpp.hpp>
#include <limits>

using namespace colorcpp;

namespace {

// blend/concat/quantize bake already-eased samples into new stop lists, so the
// resulting gradient must sample with linear easing. A second easing pass
// distorts everything between the baked stops (probe: blend(g,g,0) != g).
template <typename G>
void ExpectNoDoubleEasing(const G& g, const G& h, const char* label) {
  SCOPED_TRACE(label);

  // blend(g, h, 0) is documented as "this gradient" — identity must survive.
  const auto id = g.blend(h, 0.0f);
  for (float t : {0.123f, 0.5f, 0.877f}) {
    EXPECT_NEAR(id.sample(t).r(), g.sample(t).r(), 6e-3f);
    EXPECT_NEAR(id.sample(t).b(), g.sample(t).b(), 6e-3f);
  }

  // concat first half must reproduce the source (with its easing) at 2*u.
  const auto cat = g.concat(h);
  EXPECT_NEAR(cat.sample(0.23f).r(), g.sample(0.46f).r(), 0.01f);

  // quantize: between baked stops the result must move linearly between the
  // baked colors — no second easing pass on top of the baked samples.
  const auto q = g.quantize(3);
  const float local = 0.1f;  // sample(0.55) sits 10% between stops at 0.5 and 1.0
  const float linear_r = q.stops()[1].color.r() + local * (q.stops()[2].color.r() - q.stops()[1].color.r());
  EXPECT_NEAR(q.sample(0.55f).r(), linear_r, 1e-3f);
}

gradient::color_stops<rgbf_t> RedToBlueStops() {
  return {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
}

}  // namespace

TEST(GradientTest, ColorStopBasic) {
  gradient::color_stop<rgbf_t> stop(0.5f, rgbf_t{1.0f, 0.0f, 0.0f});
  EXPECT_FLOAT_EQ(stop.position, 0.5f);
  EXPECT_FLOAT_EQ(stop.color.r(), 1.0f);
}

TEST(GradientTest, ColorStopsSorting) {
  gradient::color_stops<rgbf_t> stops = {
      {0.8f, rgbf_t{0.0f, 0.0f, 1.0f}}, {0.2f, rgbf_t{1.0f, 0.0f, 0.0f}}, {0.5f, rgbf_t{0.0f, 1.0f, 0.0f}}};

  EXPECT_FLOAT_EQ(stops[0].position, 0.2f);
  EXPECT_FLOAT_EQ(stops[1].position, 0.5f);
  EXPECT_FLOAT_EQ(stops[2].position, 0.8f);
}

TEST(GradientTest, LinearGradientBasic) {
  using namespace colorcpp::algorithms;

  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};

  auto grad = gradient::linear(stops);

  auto start = grad.sample(0.0f);
  EXPECT_NEAR(start.r(), 1.0f, 0.001f);
  EXPECT_NEAR(start.b(), 0.0f, 0.001f);

  auto mid = grad.sample(0.5f);
  EXPECT_NEAR(mid.r(), 0.5f, 0.001f);
  EXPECT_NEAR(mid.b(), 0.5f, 0.001f);

  auto end = grad.sample(1.0f);
  EXPECT_NEAR(end.r(), 0.0f, 0.001f);
  EXPECT_NEAR(end.b(), 1.0f, 0.001f);
}

TEST(GradientTest, LinearGradientPalette) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};

  auto grad = gradient::linear(stops);
  auto palette = grad.palette(3);

  EXPECT_EQ(palette.size(), 3);
  EXPECT_NEAR(palette[0].r(), 1.0f, 0.001f);
  EXPECT_NEAR(palette[1].r(), 0.5f, 0.001f);
  EXPECT_NEAR(palette[2].r(), 0.0f, 0.001f);
}

TEST(GradientTest, RadialGradientBasic) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 1.0f, 1.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 0.0f}}};

  auto grad = gradient::radial(stops);

  auto center = grad.sample(0.0f);
  EXPECT_NEAR(center.r(), 1.0f, 0.001f);

  auto edge = grad.sample(1.0f);
  EXPECT_NEAR(edge.r(), 0.0f, 0.001f);
}

TEST(GradientTest, DiamondGradientBasic) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 1.0f, 1.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 0.0f}}};

  auto grad = gradient::diamond(stops);

  auto center = grad.sample_at(0.5f, 0.5f);
  EXPECT_NEAR(center.r(), 1.0f, 0.001f);

  auto mid = grad.sample_at(0.5f, 0.25f);
  EXPECT_NEAR(mid.r(), 0.5f, 0.001f);

  auto edge = grad.sample_at(0.5f, 0.0f);
  EXPECT_NEAR(edge.r(), 0.0f, 0.001f);
}

TEST(GradientTest, BoxGradientBasic) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 1.0f, 1.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 0.0f}}};

  auto grad = gradient::box(stops);

  auto center = grad.sample_at(0.5f, 0.5f);
  EXPECT_NEAR(center.r(), 1.0f, 0.001f);

  auto quarter = grad.sample_at(0.25f, 0.25f);
  EXPECT_NEAR(quarter.r(), 0.5f, 0.001f);

  auto side = grad.sample_at(0.5f, 0.0f);
  EXPECT_NEAR(side.r(), 0.0f, 0.001f);
}

TEST(GradientTest, AngularGradientBasic) {
  gradient::color_stops<rgbf_t> stops = {
      {0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {0.5f, rgbf_t{0.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 0.0f, 0.0f}}};

  auto grad = gradient::angular(stops);

  auto start = grad.sample(0.0f);
  EXPECT_NEAR(start.r(), 1.0f, 0.001f);

  auto mid = grad.sample(0.5f);
  EXPECT_NEAR(mid.g(), 1.0f, 0.001f);
}

TEST(GradientTest, EasingFunctions) {
  using namespace gradient::easing;

  EXPECT_FLOAT_EQ(linear(0.5f), 0.5f);
  EXPECT_FLOAT_EQ(ease_in(0.5f), 0.25f);
  EXPECT_FLOAT_EQ(ease_out(0.5f), 0.75f);

  auto bezier_linear = cubic_bezier<float>(0.0f, 0.0f, 1.0f, 1.0f);
  EXPECT_NEAR(bezier_linear(0.25f), 0.25f, 0.001f);
  EXPECT_NEAR(bezier_linear(0.50f), 0.50f, 0.001f);
  EXPECT_NEAR(bezier_linear(0.75f), 0.75f, 0.001f);
}

TEST(GradientTest, PresetGradientsSupportCanonicalAndResampledStops) {
  auto viridis_default = gradient::preset::viridis<rgba8_t>();
  EXPECT_EQ(viridis_default.stops().size(), 5);

  auto viridis_resampled = gradient::preset::viridis<rgba8_t>(7);
  EXPECT_EQ(viridis_resampled.stops().size(), 7);
  EXPECT_EQ(viridis_resampled.sample(0.0f), viridis_default.sample(0.0f));
  EXPECT_EQ(viridis_resampled.sample(1.0f), viridis_default.sample(1.0f));
}

TEST(GradientTest, PresetNamespaceAliasAndSpectralPresetExist) {
  using namespace colorcpp::io::literals;
  auto sunset_alias = gradient::preset::sunset<rgba8_t>();
  EXPECT_EQ(sunset_alias.stops().size(), 5);

  auto spectral = gradient::preset::spectral<rgba8_t>(7);
  EXPECT_EQ(spectral.stops().size(), 7);
  EXPECT_EQ(spectral.sample(0.0f), 0x9e0142_rgb);
  EXPECT_EQ(spectral.sample(1.0f), 0x5e4fa2_rgb);
}

TEST(GradientTest, MultiStopGradient) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}},
                                         {0.3f, rgbf_t{1.0f, 1.0f, 0.0f}},
                                         {0.7f, rgbf_t{0.0f, 1.0f, 0.0f}},
                                         {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};

  auto grad = gradient::linear(stops);

  auto quarter = grad.sample(0.25f);
  EXPECT_NEAR(quarter.r(), 1.0f, 0.001f);
  EXPECT_NEAR(quarter.g(), 0.833f, 0.01f);
}

TEST(GradientTest, SequenceGradientBasic) {
  // Create two linear gradients
  gradient::color_stops<rgbf_t> stops1 = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad1 = gradient::linear(stops1);

  gradient::color_stops<rgbf_t> stops2 = {{0.0f, rgbf_t{0.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 1.0f, 0.0f}}};
  auto grad2 = gradient::linear(stops2);

  // Create sequence: 0-0.5 uses grad1, 0.5-1.0 uses grad2
  std::vector<gradient::sequence_segment<gradient::linear_gradient<rgbf_t>>> segments = {{grad1, 0.0f, 0.5f},
                                                                                         {grad2, 0.5f, 1.0f}};
  auto seq = gradient::sequence(segments);

  // Test at 0.25 (should be grad1 at 0.5)
  auto color1 = seq.sample(0.25f);
  EXPECT_NEAR(color1.r(), 0.5f, 0.001f);
  EXPECT_NEAR(color1.b(), 0.5f, 0.001f);

  // Test at 0.75 (should be grad2 at 0.5)
  auto color2 = seq.sample(0.75f);
  EXPECT_NEAR(color2.g(), 1.0f, 0.001f);
  EXPECT_NEAR(color2.r(), 0.5f, 0.001f);
}

TEST(GradientTest, SequenceGradientPalette) {
  gradient::color_stops<rgbf_t> stops1 = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad1 = gradient::linear(stops1);

  gradient::color_stops<rgbf_t> stops2 = {{0.0f, rgbf_t{0.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 1.0f, 0.0f}}};
  auto grad2 = gradient::linear(stops2);

  std::vector<gradient::sequence_segment<gradient::linear_gradient<rgbf_t>>> segments = {{grad1, 0.0f, 0.5f},
                                                                                         {grad2, 0.5f, 1.0f}};
  auto seq = gradient::sequence(segments);

  auto palette = seq.palette(5);
  EXPECT_EQ(palette.size(), 5);

  // First color should be red
  EXPECT_NEAR(palette[0].r(), 1.0f, 0.001f);
  EXPECT_NEAR(palette[0].g(), 0.0f, 0.001f);

  // Last color should be yellow
  EXPECT_NEAR(palette[4].r(), 1.0f, 0.001f);
  EXPECT_NEAR(palette[4].g(), 1.0f, 0.001f);
}

TEST(GradientTest, ScaleOffsetQuantize) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad = gradient::linear(stops);

  // Test scale
  auto scaled = grad.scale(2.0f);
  auto color_scaled = scaled.sample(0.25f);
  EXPECT_NEAR(color_scaled.r(), 0.5f, 0.001f);

  // offset(delta) shifts the sample parameter: sample(t) == original.sample(clamp_01(t + delta))
  auto shifted = grad.offset(0.5f);
  EXPECT_NEAR(shifted.sample(0.0f).r(), grad.sample(0.5f).r(), 0.001f);
  EXPECT_NEAR(shifted.sample(0.0f).r(), 0.5f, 0.001f);

  // Test quantize
  auto quantized = grad.quantize(3);
  auto palette = quantized.palette(3);
  EXPECT_EQ(palette.size(), 3);
}

TEST(GradientTest, ScaleDetailed) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad = gradient::linear(stops);

  // Scale by 0.5 (stretch)
  auto stretched = grad.scale(0.5f);
  EXPECT_NEAR(stretched.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(stretched.sample(0.5f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(stretched.sample(1.0f).r(), 0.0f, 0.001f);

  // Scale by 2.0 (compress)
  auto compressed = grad.scale(2.0f);
  EXPECT_NEAR(compressed.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(compressed.sample(0.25f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(compressed.sample(0.5f).r(), 0.0f, 0.001f);

  // Scale by 4.0 (strongly compress)
  auto strongly_compressed = grad.scale(4.0f);
  EXPECT_NEAR(strongly_compressed.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(strongly_compressed.sample(0.125f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(strongly_compressed.sample(0.25f).r(), 0.0f, 0.001f);
}

TEST(GradientTest, OffsetDetailed) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad = gradient::linear(stops);

  // Positive offset
  auto shifted_right = grad.offset(0.3f);
  EXPECT_NEAR(shifted_right.sample(0.0f).r(), grad.sample(0.3f).r(), 0.001f);
  EXPECT_NEAR(shifted_right.sample(0.7f).r(), grad.sample(1.0f).r(), 0.001f);

  // Negative offset
  auto shifted_left = grad.offset(-0.3f);
  EXPECT_NEAR(shifted_left.sample(0.3f).r(), grad.sample(0.0f).r(), 0.001f);
  EXPECT_NEAR(shifted_left.sample(1.0f).r(), grad.sample(0.7f).r(), 0.001f);

  // Full offset
  auto full_offset = grad.offset(1.0f);
  EXPECT_NEAR(full_offset.sample(0.0f).r(), grad.sample(1.0f).r(), 0.001f);
  EXPECT_NEAR(full_offset.sample(0.5f).r(), grad.sample(1.0f).r(), 0.001f);
}

TEST(GradientTest, QuantizeDetailed) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad = gradient::linear(stops);

  // Quantize to 2 levels
  auto q2 = grad.quantize(2);
  EXPECT_EQ(q2.stops().size(), 2);
  EXPECT_NEAR(q2.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(q2.sample(1.0f).r(), 0.0f, 0.001f);

  // Quantize to 5 levels
  auto q5 = grad.quantize(5);
  EXPECT_EQ(q5.stops().size(), 5);
  EXPECT_NEAR(q5.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(q5.sample(0.25f).r(), 0.75f, 0.001f);
  EXPECT_NEAR(q5.sample(0.5f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(q5.sample(0.75f).r(), 0.25f, 0.001f);
  EXPECT_NEAR(q5.sample(1.0f).r(), 0.0f, 0.001f);

  // Quantize to 1 level (single color)
  auto q1 = grad.quantize(1);
  EXPECT_EQ(q1.stops().size(), 1);
  EXPECT_NEAR(q1.sample(0.0f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(q1.sample(0.5f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(q1.sample(1.0f).r(), 0.5f, 0.001f);
}

TEST(GradientTest, BoundaryConditions) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad = gradient::linear(stops);

  // Test boundary positions
  EXPECT_NEAR(grad.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(grad.sample(1.0f).r(), 0.0f, 0.001f);

  // Test out of bounds (should clamp)
  EXPECT_NEAR(grad.sample(-0.5f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(grad.sample(1.5f).r(), 0.0f, 0.001f);

  // Test very close to boundaries
  EXPECT_NEAR(grad.sample(0.001f).r(), 0.999f, 0.001f);
  EXPECT_NEAR(grad.sample(0.999f).r(), 0.001f, 0.001f);
}

TEST(GradientTest, ReverseGradient) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad = gradient::linear(stops);

  auto reversed = grad.reverse();

  // Reversed gradient should have opposite colors at positions
  EXPECT_NEAR(reversed.sample(0.0f).r(), 0.0f, 0.001f);
  EXPECT_NEAR(reversed.sample(0.0f).b(), 1.0f, 0.001f);
  EXPECT_NEAR(reversed.sample(1.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(reversed.sample(1.0f).b(), 0.0f, 0.001f);
  EXPECT_NEAR(reversed.sample(0.5f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(reversed.sample(0.5f).b(), 0.5f, 0.001f);
}

TEST(GradientTest, BlendGradients) {
  gradient::color_stops<rgbf_t> stops1 = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  gradient::color_stops<rgbf_t> stops2 = {{0.0f, rgbf_t{0.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 1.0f, 0.0f}}};

  auto grad1 = gradient::linear(stops1);
  auto grad2 = gradient::linear(stops2);

  // Blend at 0.5
  auto blended = grad1.blend(grad2, 0.5f);

  // Result should be a mix of both gradients
  EXPECT_NEAR(blended.sample(0.0f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(blended.sample(0.0f).g(), 0.5f, 0.001f);
  EXPECT_NEAR(blended.sample(1.0f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(blended.sample(1.0f).b(), 0.5f, 0.001f);
}

TEST(GradientTest, ConcatGradients) {
  gradient::color_stops<rgbf_t> stops1 = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  gradient::color_stops<rgbf_t> stops2 = {{0.0f, rgbf_t{0.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 1.0f, 0.0f}}};

  auto grad1 = gradient::linear(stops1);
  auto grad2 = gradient::linear(stops2);

  auto concatenated = grad1.concat(grad2);

  // First half should be grad1
  EXPECT_NEAR(concatenated.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(concatenated.sample(0.25f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(concatenated.sample(0.5f).r(), 0.0f, 0.001f);

  // The seam is blended to avoid a single-sample hard edge.
  EXPECT_NEAR(concatenated.sample(0.5f).g(), 0.5f, 0.001f);
  EXPECT_NEAR(concatenated.sample(0.5f).b(), 0.5f, 0.001f);

  // Second half should be grad2
  EXPECT_NEAR(concatenated.sample(0.75f).g(), 1.0f, 0.001f);
  EXPECT_NEAR(concatenated.sample(1.0f).r(), 1.0f, 0.001f);
}

TEST(GradientTest, MoreEasingFunctions) {
  using namespace gradient::easing;

  // Test smooth_step
  EXPECT_NEAR(smooth_step(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(smooth_step(0.5f), 0.5f, 0.001f);
  EXPECT_NEAR(smooth_step(1.0f), 1.0f, 0.001f);

  // Test smoother_step
  EXPECT_NEAR(smoother_step(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(smoother_step(0.5f), 0.5f, 0.001f);
  EXPECT_NEAR(smoother_step(1.0f), 1.0f, 0.001f);

  // Test ease_in_sine
  EXPECT_NEAR(ease_in_sine(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(ease_in_sine(1.0f), 1.0f, 0.001f);

  // Test ease_out_sine
  EXPECT_NEAR(ease_out_sine(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(ease_out_sine(1.0f), 1.0f, 0.001f);

  // Test ease_in_out_sine
  EXPECT_NEAR(ease_in_out_sine(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(ease_in_out_sine(0.5f), 0.5f, 0.001f);
  EXPECT_NEAR(ease_in_out_sine(1.0f), 1.0f, 0.001f);

  // Test ease_in_cubic
  EXPECT_NEAR(ease_in_cubic(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(ease_in_cubic(0.5f), 0.125f, 0.001f);
  EXPECT_NEAR(ease_in_cubic(1.0f), 1.0f, 0.001f);

  // Test ease_out_cubic
  EXPECT_NEAR(ease_out_cubic(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(ease_out_cubic(0.5f), 0.875f, 0.001f);
  EXPECT_NEAR(ease_out_cubic(1.0f), 1.0f, 0.001f);

  // Test ease_in_out_cubic
  EXPECT_NEAR(ease_in_out_cubic(0.0f), 0.0f, 0.001f);
  EXPECT_NEAR(ease_in_out_cubic(0.5f), 0.5f, 0.001f);
  EXPECT_NEAR(ease_in_out_cubic(1.0f), 1.0f, 0.001f);
}

TEST(GradientTest, EasingWithGradient) {
  using namespace gradient;

  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};

  // Linear easing
  auto linear_grad = gradient::linear(stops, easing::linear<float>);
  EXPECT_NEAR(linear_grad.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(linear_grad.sample(0.5f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(linear_grad.sample(1.0f).r(), 0.0f, 0.001f);

  // Ease in
  auto ease_in_grad = gradient::linear(stops, easing::ease_in<float>);
  EXPECT_NEAR(ease_in_grad.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(ease_in_grad.sample(0.5f).r(), 0.75f, 0.001f);
  EXPECT_NEAR(ease_in_grad.sample(1.0f).r(), 0.0f, 0.001f);

  // Ease out
  auto ease_out_grad = gradient::linear(stops, easing::ease_out<float>);
  EXPECT_NEAR(ease_out_grad.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(ease_out_grad.sample(0.5f).r(), 0.25f, 0.001f);
  EXPECT_NEAR(ease_out_grad.sample(1.0f).r(), 0.0f, 0.001f);

  // Ease in-out
  auto ease_in_out_grad = gradient::linear(stops, easing::ease_in_out<float>);
  EXPECT_NEAR(ease_in_out_grad.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(ease_in_out_grad.sample(0.25f).r(), 0.875f, 0.001f);
  EXPECT_NEAR(ease_in_out_grad.sample(0.5f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(ease_in_out_grad.sample(0.75f).r(), 0.125f, 0.001f);
  EXPECT_NEAR(ease_in_out_grad.sample(1.0f).r(), 0.0f, 0.001f);
}

TEST(GradientTest, Rgb8GradientUsesNormalizedFloatPosition) {
  using namespace gradient;

  gradient::color_stops<rgb8_t> stops = {{0.0f, rgb8_t{255, 0, 0}}, {1.0f, rgb8_t{0, 0, 255}}};

  auto linear_grad = gradient::linear(stops, easing::ease_in<float>);
  auto mid = linear_grad.sample(0.5f);

  EXPECT_NEAR(static_cast<float>(mid.r()), 191.0f, 1.0f);
  EXPECT_NEAR(static_cast<float>(mid.b()), 64.0f, 1.0f);
}

TEST(GradientTest, SequenceWithDifferentGradients) {
  // Create different types of gradients for sequence
  gradient::color_stops<rgbf_t> linear_stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto linear_grad = gradient::linear(linear_stops);

  gradient::color_stops<rgbf_t> radial_stops = {{0.0f, rgbf_t{0.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 1.0f, 0.0f}}};
  auto radial_grad = gradient::radial(radial_stops);

  // Create sequence with linear and radial
  std::vector<gradient::sequence_segment<gradient::linear_gradient<rgbf_t>>> segments = {{linear_grad, 0.0f, 0.5f},
                                                                                         {linear_grad, 0.5f, 1.0f}};
  auto seq = gradient::sequence(segments);

  // Test sequence
  EXPECT_NEAR(seq.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(seq.sample(0.25f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(seq.sample(0.5f).r(), 0.0f, 0.001f);
  EXPECT_NEAR(seq.sample(0.75f).r(), 0.5f, 0.001f);
  EXPECT_NEAR(seq.sample(1.0f).r(), 0.0f, 0.001f);
}

TEST(GradientTest, SequenceInterpolatesAcrossGaps) {
  auto left = gradient::linear<rgbf_t>({{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 0.0f, 0.0f}}});
  auto right = gradient::linear<rgbf_t>({{0.0f, rgbf_t{0.0f, 0.0f, 1.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}});

  std::vector<gradient::sequence_segment<gradient::linear_gradient<rgbf_t>>> segments = {{left, 0.0f, 0.4f},
                                                                                         {right, 0.6f, 1.0f}};
  auto seq = gradient::sequence(std::move(segments));
  auto mid_gap = seq.sample(0.5f);

  EXPECT_NEAR(mid_gap.r(), 0.5f, 0.001f);
  EXPECT_NEAR(mid_gap.g(), 0.0f, 0.001f);
  EXPECT_NEAR(mid_gap.b(), 0.5f, 0.001f);
}

TEST(GradientTest, SequencePaletteOfOneReturnsMidpoint) {
  auto g = gradient::linear(RedToBlueStops());
  std::vector<gradient::sequence_segment<gradient::linear_gradient<rgbf_t>>> segments = {{g, 0.0f, 1.0f}};
  auto seq = gradient::sequence(std::move(segments));

  auto p1 = seq.palette(1);
  ASSERT_EQ(p1.size(), 1u);
  // Every other gradient class returns sample(0.5) for count == 1. Sequence must
  // agree: without a guard, i/(count-1) is 0/0 = NaN and the loop falls through
  // to back().sample(1.0f) — the terminal blue, not the midpoint.
  EXPECT_NEAR(p1[0].r(), 0.5f, 1e-3f);
  EXPECT_NEAR(p1[0].g(), 0.0f, 1e-3f);
  EXPECT_NEAR(p1[0].b(), 0.5f, 1e-3f);
}

TEST(GradientTest, GradientCtorRejectsNonFiniteColorInAnyChannel) {
  using namespace colorcpp::algorithms;
  const float nan = std::numeric_limits<float>::quiet_NaN();
  const float inf = std::numeric_limits<float>::infinity();

  // get_index<I> requires a compile-time index, so dispatch per channel.
  auto expect_reject = [](auto channel_tag, float bad, const char* label) {
    SCOPED_TRACE(label);
    constexpr std::size_t ch = decltype(channel_tag)::value;
    rgbf_t c{0.5f, 0.5f, 0.5f};
    c.get_index<ch>() = bad;
    gradient::color_stops<rgbf_t> s = {{0.0f, c}, {1.0f, rgbf_t{1.0f, 1.0f, 1.0f}}};
    EXPECT_THROW((void)gradient::linear(s), std::invalid_argument);
  };

  for (float bad : {nan, inf, -inf}) {
    expect_reject(std::integral_constant<std::size_t, 0>{}, bad, "channel 0 (r)");
    expect_reject(std::integral_constant<std::size_t, 1>{}, bad, "channel 1 (g)");
    expect_reject(std::integral_constant<std::size_t, 2>{}, bad, "channel 2 (b)");
  }
}

TEST(GradientTest, ColorStopsSortIsStableForEqualPositions) {
  // std::sort is unstable: with enough equal-position elements (above the
  // introsort small-range cutoff) their relative order is unspecified. Equal
  // positions carry distinct colors, so insertion order must be preserved.
  gradient::color_stops<rgbf_t> stops{{0.5f, rgbf_t{0.0f, 0.0f, 0.0f}}};
  constexpr int n = 24;  // above libstdc++'s ~16-element insertion-sort cutoff
  for (int i = 1; i < n; ++i) {
    stops.add(0.5f, rgbf_t{static_cast<float>(i), 0.0f, 0.0f});
  }
  ASSERT_EQ(stops.size(), static_cast<std::size_t>(n));
  for (int i = 0; i < n; ++i) {
    EXPECT_FLOAT_EQ(stops[static_cast<std::size_t>(i)].color.r(), static_cast<float>(i))
        << "equal-position stop " << i << " lost its insertion order";
  }
}

TEST(GradientTest, CubicBezierClampsControlPointXToUnitInterval) {
  using namespace colorcpp::algorithms;
  // W3C css-easing-1: x1/x2 outside [0, 1] are invalid and must be clamped at
  // parse time; y1/y2 may exceed. Out-of-range x makes x(t) non-monotonic and
  // the root find ambiguous.
  const auto clamped = gradient::easing::cubic_bezier<float>(1.0f, 0.5f, 0.0f, 0.5f);
  const auto raw = gradient::easing::cubic_bezier<float>(1.5f, 0.5f, -0.5f, 0.5f);
  for (float t : {0.0f, 0.1f, 0.25f, 0.5f, 0.75f, 0.9f, 1.0f}) {
    EXPECT_NEAR(raw(t), clamped(t), 1e-5f) << "at t=" << t;
  }
}

TEST(GradientTest, SteppedGradientQuantizesSamplePositions) {
  auto base = gradient::linear<rgbf_t>({{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}});
  auto stepped_grad = gradient::stepped(base, 3);

  auto low = stepped_grad.sample(0.10f);
  EXPECT_NEAR(low.r(), 1.0f, 0.001f);
  EXPECT_NEAR(low.b(), 0.0f, 0.001f);

  auto mid = stepped_grad.sample(0.60f);
  EXPECT_NEAR(mid.r(), 0.5f, 0.001f);
  EXPECT_NEAR(mid.b(), 0.5f, 0.001f);

  auto high = stepped_grad.sample(1.00f);
  EXPECT_NEAR(high.r(), 0.0f, 0.001f);
  EXPECT_NEAR(high.b(), 1.0f, 0.001f);
}

TEST(GradientTest, SteppedSpatialGradientUsesPositionAt) {
  auto base = gradient::radial<rgbf_t>({{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}});
  auto stepped_grad = gradient::stepped(base, 3);

  auto center = stepped_grad.sample_at(0.5f, 0.5f);
  EXPECT_NEAR(center.r(), 1.0f, 0.001f);
  EXPECT_NEAR(center.b(), 0.0f, 0.001f);

  auto mid = stepped_grad.sample_at(0.5f, 0.25f);
  EXPECT_NEAR(mid.r(), 0.5f, 0.001f);
  EXPECT_NEAR(mid.b(), 0.5f, 0.001f);

  auto edge = stepped_grad.sample_at(0.5f, 0.0f);
  EXPECT_NEAR(edge.r(), 0.0f, 0.001f);
  EXPECT_NEAR(edge.b(), 1.0f, 0.001f);
}

TEST(GradientTest, RadialGradientOperations) {
  gradient::color_stops<rgbf_t> stops = {{0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {1.0f, rgbf_t{0.0f, 0.0f, 1.0f}}};
  auto grad = gradient::radial(stops);

  // Test scale on radial gradient
  auto scaled = grad.scale(2.0f);
  EXPECT_NEAR(scaled.sample(0.25f).r(), 0.5f, 0.001f);

  // Test offset on radial gradient
  auto offset = grad.offset(0.5f);
  EXPECT_NEAR(offset.sample(0.0f).r(), grad.sample(0.5f).r(), 0.001f);

  // Test quantize on radial gradient
  auto quantized = grad.quantize(3);
  EXPECT_EQ(quantized.stops().size(), 3);

  // Test reverse on radial gradient
  auto reversed = grad.reverse();
  EXPECT_NEAR(reversed.sample(0.0f).r(), 0.0f, 0.001f);
  EXPECT_NEAR(reversed.sample(1.0f).r(), 1.0f, 0.001f);
}

TEST(GradientTest, AngularGradientOperations) {
  gradient::color_stops<rgbf_t> stops = {
      {0.0f, rgbf_t{1.0f, 0.0f, 0.0f}}, {0.5f, rgbf_t{0.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 0.0f, 0.0f}}};
  auto grad = gradient::angular(stops);

  // Test scale on angular gradient
  auto scaled = grad.scale(2.0f);
  EXPECT_NEAR(scaled.sample(0.25f).g(), 1.0f, 0.001f);
  EXPECT_NEAR(scaled.sample(0.75f).r(), 1.0f, 0.001f);

  // Test offset on angular gradient
  auto offset = grad.offset(0.25f);
  EXPECT_NEAR(offset.sample(0.0f).g(), 0.5f, 0.001f);

  // Test quantize on angular gradient
  auto quantized = grad.quantize(3);
  EXPECT_EQ(quantized.stops().size(), 3);

  // Test reverse on angular gradient
  auto reversed = grad.reverse();
  EXPECT_NEAR(reversed.sample(0.0f).r(), 1.0f, 0.001f);
  EXPECT_NEAR(reversed.sample(0.5f).g(), 1.0f, 0.001f);
}

TEST(GradientTest, AngularSampleAtUsesCssOrientation) {
  gradient::color_stops<rgbf_t> stops = {
      {0.0f, rgbf_t{1.0f, 0.0f, 0.0f}},  {0.25f, rgbf_t{0.0f, 1.0f, 0.0f}}, {0.50f, rgbf_t{0.0f, 0.0f, 1.0f}},
      {0.75f, rgbf_t{1.0f, 1.0f, 0.0f}}, {1.0f, rgbf_t{1.0f, 0.0f, 0.0f}},
  };

  auto grad = gradient::angular(stops);

  auto north = grad.sample_at(0.5f, 0.0f);
  EXPECT_NEAR(north.r(), 1.0f, 0.001f);
  EXPECT_NEAR(north.g(), 0.0f, 0.001f);
  EXPECT_NEAR(north.b(), 0.0f, 0.001f);

  auto east = grad.sample_at(1.0f, 0.5f);
  EXPECT_NEAR(east.r(), 0.0f, 0.001f);
  EXPECT_NEAR(east.g(), 1.0f, 0.001f);
  EXPECT_NEAR(east.b(), 0.0f, 0.001f);

  auto south = grad.sample_at(0.5f, 1.0f);
  EXPECT_NEAR(south.r(), 0.0f, 0.001f);
  EXPECT_NEAR(south.g(), 0.0f, 0.001f);
  EXPECT_NEAR(south.b(), 1.0f, 0.001f);

  auto west = grad.sample_at(0.0f, 0.5f);
  EXPECT_NEAR(west.r(), 1.0f, 0.001f);
  EXPECT_NEAR(west.g(), 1.0f, 0.001f);
  EXPECT_NEAR(west.b(), 0.0f, 0.001f);
}

TEST(GradientTest, BlendConcatQuantizeDoNotReapplyEasing) {
  using gradient::easing::ease_in;

  {
    auto g = gradient::linear(RedToBlueStops(), ease_in<float>);
    auto h = gradient::linear(RedToBlueStops(), ease_in<float>);
    ExpectNoDoubleEasing(g, h, "linear");
  }
  {
    auto g = gradient::radial(RedToBlueStops(), ease_in<float>);
    auto h = gradient::radial(RedToBlueStops(), ease_in<float>);
    ExpectNoDoubleEasing(g, h, "radial");
  }
  {
    auto g = gradient::angular(RedToBlueStops(), ease_in<float>);
    auto h = gradient::angular(RedToBlueStops(), ease_in<float>);
    ExpectNoDoubleEasing(g, h, "angular");
  }
  {
    auto g = gradient::box(RedToBlueStops(), ease_in<float>);
    auto h = gradient::box(RedToBlueStops(), ease_in<float>);
    ExpectNoDoubleEasing(g, h, "box");
  }
  {
    auto g = gradient::diamond(RedToBlueStops(), ease_in<float>);
    auto h = gradient::diamond(RedToBlueStops(), ease_in<float>);
    ExpectNoDoubleEasing(g, h, "diamond");
  }
}
