#include <gtest/gtest.h>

#include <colorcpp/colorcpp.hpp>

using namespace colorcpp;

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
      {0.0f, rgbf_t{1.0f, 0.0f, 0.0f}},
      {0.25f, rgbf_t{0.0f, 1.0f, 0.0f}},
      {0.50f, rgbf_t{0.0f, 0.0f, 1.0f}},
      {0.75f, rgbf_t{1.0f, 1.0f, 0.0f}},
      {1.0f, rgbf_t{1.0f, 0.0f, 0.0f}},
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
