/**
 * @file gradient_example.cpp
 * @brief Example: practical coverage of the full gradient module.
 *
 * Covers:
 * - color_stop / color_stops management
 * - easing functions (built-in + cubic bezier)
 * - linear / radial / angular gradients
 * - sample(), sample_at(), palette()
 * - reverse(), blend(), concat(), scale(), offset(), quantize()
 * - sequence gradients and segment editing
 */

#include <colorcpp/colorcpp.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace colorcpp;
using namespace colorcpp::algorithms::gradient;
using namespace colorcpp::io::ansi;
using namespace colorcpp::io::literals;
using namespace colorcpp::operations::conversion;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

template <typename Color>
static rgba8_t as_rgba8(const Color& c) {
  return color_cast<rgba8_t>(c);
}

template <typename Color>
static void print_hex_chip(const Color& c, int width = 2) {
  auto rgba = as_rgba8(c);
  print_swatch(std::cout, rgba, width);
  std::cout << " " << std::hex << rgba << std::dec;
}

template <typename Color>
static void print_stops(const color_stops<Color>& stops, const char* label) {
  std::cout << "  " << label << " (" << stops.size() << " stops)\n";
  for (const auto& stop : stops) {
    std::cout << "    t=" << std::fixed << std::setprecision(2) << stop.position << "  ";
    print_hex_chip(stop.color);
    std::cout << '\n';
  }
}

template <typename Fn>
static void print_easing_line(const char* label, Fn&& fn) {
  std::cout << "  " << std::left << std::setw(20) << label << std::right << std::fixed << std::setprecision(3)
            << "0.25=" << std::setw(6) << fn(0.25f) << "  0.50=" << std::setw(6) << fn(0.50f)
            << "  0.75=" << std::setw(6) << fn(0.75f) << '\n';
}

template <typename Color>
static void print_palette_row(const char* label, const std::vector<Color>& palette, int swatch_width = 2) {
  std::cout << "  " << std::left << std::setw(24) << label;
  for (const auto& c : palette) {
    print_swatch(std::cout, as_rgba8(c), swatch_width);
  }
  std::cout << '\n';
}

template <typename Gradient>
static void print_sample_row(const Gradient& grad, const char* label, std::initializer_list<float> positions) {
  std::cout << "  " << label << '\n';
  for (float t : positions) {
    std::cout << "    t=" << std::fixed << std::setprecision(2) << std::setw(4) << t << "  ";
    print_hex_chip(grad.sample(t));
    std::cout << '\n';
  }
}

template <typename Gradient>
static void print_sample_at(const Gradient& grad, const char* label, float x, float y) {
  std::cout << "    " << std::left << std::setw(14) << label << std::right << " (" << std::fixed << std::setprecision(2)
            << x << ", " << y << ")  ";
  print_hex_chip(grad.sample_at(x, y));
  std::cout << '\n';
}

int main() {
  std::cout << std::fixed << std::setprecision(2);

  // ========================================================================
  section("1. color_stop and color_stops");

  color_stops<rgba8_t> managed_stops = {
      {1.00f, 0x0F172A_rgb},
      {0.00f, 0xF8FAFC_rgb},
      {0.55f, 0x38BDF8_rgb},
  };
  print_stops(managed_stops, "sorted on construction");

  managed_stops.add(color_stop<rgba8_t>{0.25f, 0xBAE6FD_rgb});
  managed_stops.add(color_stop<rgba8_t>{0.80f, 0xA855F7_rgb});
  managed_stops.update_position(1, 0.20f);
  managed_stops.update_color(2, 0x22D3EE_rgb);
  std::size_t removed_by_pos = managed_stops.remove_at(0.80f);
  managed_stops.remove(managed_stops.size() - 1);

  print_stops(managed_stops, "after add/update/remove");
  std::cout << "  empty() = " << std::boolalpha << managed_stops.empty() << '\n';
  std::cout << "  remove_at(0.80) removed " << removed_by_pos << " stop(s)\n";
  std::cout << "  front/back: ";
  print_hex_chip(managed_stops.front().color);
  std::cout << "  ";
  print_hex_chip(managed_stops.back().color);
  std::cout << '\n';

  // ========================================================================
  section("2. easing namespace");

  std::cout << "  Sample values at t = 0.25 / 0.50 / 0.75\n\n";

  print_easing_line("linear", easing::linear<float>);
  print_easing_line("ease_in", easing::ease_in<float>);
  print_easing_line("ease_out", easing::ease_out<float>);
  print_easing_line("ease_in_out", easing::ease_in_out<float>);
  print_easing_line("smooth_step", easing::smooth_step<float>);
  print_easing_line("smoother_step", easing::smoother_step<float>);
  print_easing_line("ease_in_sine", easing::ease_in_sine<float>);
  print_easing_line("ease_out_sine", easing::ease_out_sine<float>);
  print_easing_line("ease_in_out_sine", easing::ease_in_out_sine<float>);
  print_easing_line("ease_in_cubic", easing::ease_in_cubic<float>);
  print_easing_line("ease_out_cubic", easing::ease_out_cubic<float>);
  print_easing_line("ease_in_out_cubic", easing::ease_in_out_cubic<float>);
  print_easing_line("ease_in_quad", easing::ease_in_quad<float>);
  print_easing_line("ease_out_quad", easing::ease_out_quad<float>);
  print_easing_line("ease_in_out_quad", easing::ease_in_out_quad<float>);
  print_easing_line("ease_in_expo", easing::ease_in_expo<float>);
  print_easing_line("ease_out_expo", easing::ease_out_expo<float>);
  print_easing_line("ease_in_out_expo", easing::ease_in_out_expo<float>);
  print_easing_line("ease_in_circ", easing::ease_in_circ<float>);
  print_easing_line("ease_out_circ", easing::ease_out_circ<float>);
  print_easing_line("ease_in_out_circ", easing::ease_in_out_circ<float>);
  auto css_like_bezier = easing::cubic_bezier<float>(0.25f, 0.10f, 0.25f, 1.00f);
  print_easing_line("cubic_bezier(...)", css_like_bezier);

  // ========================================================================
  section("3. linear gradients");

  auto sky = 0x0EA5E9_rgb;
  auto orange = 0xF97316_rgb;

  auto rgb_linear = linear<rgba8_t>({
      {0.00f, sky},
      {1.00f, orange},
  });
  auto oklab_linear = linear<oklab_t>({
      {0.00f, color_cast<oklab_t>(sky)},
      {1.00f, color_cast<oklab_t>(orange)},
  });

  print_sample_row(rgb_linear, "sample(t)", {0.00f, 0.25f, 0.50f, 0.75f, 1.00f});
  print_palette_row("palette(7) in rgba8_t", rgb_linear.palette(7));
  print_palette_row("palette(7) in oklab_t", oklab_linear.palette(7));

  auto multi_stop = linear<rgba8_t>(
      {
          {0.00f, 0x020617_rgb},
          {0.30f, 0x0EA5E9_rgb},
          {0.70f, 0x22C55E_rgb},
          {1.00f, 0xFACC15_rgb},
      },
      easing::smooth_step<float>);

  print_stops(multi_stop.stops(), "multi-stop + smooth_step");
  print_palette_row("palette(10)", multi_stop.palette(10));

  auto retimed = multi_stop;
  retimed.set_easing(easing::ease_in_out_cubic<float>);
  print_palette_row("after set_easing()", retimed.palette(10));

  auto warm = linear<rgba8_t>({
      {0.00f, 0xEF4444_rgb},
      {1.00f, 0x3B82F6_rgb},
  });
  auto cool = linear<rgba8_t>({
      {0.00f, 0x10B981_rgb},
      {1.00f, 0xF59E0B_rgb},
  });

  print_palette_row("reverse()", warm.reverse().palette(7));
  print_palette_row("scale(2.0)", warm.scale(2.0f).palette(7));
  print_palette_row("offset(0.25)", warm.offset(0.25f).palette(7));
  print_palette_row("quantize(5)", warm.quantize(5).palette(11));
  print_palette_row("blend(cool, 0.50)", warm.blend(cool, 0.50f).palette(7));
  print_palette_row("concat(cool)", warm.concat(cool).palette(11));

  // ========================================================================
  section("4. radial gradients");

  auto spotlight = radial<rgba8_t>(
      {
          {0.00f, 0xFFFFFF_rgb},
          {0.45f, 0xFDE68A_rgb},
          {1.00f, 0x7C2D12_rgb},
      },
      easing::ease_out_sine<float>);

  auto mist = radial<rgba8_t>({
      {0.00f, 0xDBEAFE_rgb},
      {1.00f, 0x1D4ED8_rgb},
  });

  print_sample_row(spotlight, "sample(radius)", {0.00f, 0.25f, 0.50f, 0.75f, 1.00f});
  print_palette_row("palette(8)", spotlight.palette(8));
  std::cout << "  sample_at(x, y) around center (0.50, 0.50)\n";
  print_sample_at(spotlight, "center", 0.50f, 0.50f);
  print_sample_at(spotlight, "top", 0.50f, 0.10f);
  print_sample_at(spotlight, "left", 0.10f, 0.50f);
  print_sample_at(spotlight, "corner", 0.00f, 0.00f);
  print_palette_row("reverse().offset(0.20)", spotlight.reverse().offset(0.20f).palette(8));
  print_palette_row("scale(1.5)", spotlight.scale(1.50f).palette(8));
  print_palette_row("quantize(4)", spotlight.quantize(4).palette(8));
  print_palette_row("blend(mist, 0.50)", spotlight.blend(mist, 0.50f).palette(8));
  print_palette_row("concat(mist)", spotlight.concat(mist).palette(10));

  // ========================================================================
  section("5. angular gradients");

  auto wheel = angular<rgba8_t>({
      {0.00f, 0xFF0000_rgb},
      {0.33f, 0x00FF00_rgb},
      {0.66f, 0x0000FF_rgb},
      {1.00f, 0xFF0000_rgb},
  });
  auto dusk = angular<rgba8_t>(
      {
          {0.00f, 0x111827_rgb},
          {0.50f, 0xF472B6_rgb},
          {1.00f, 0x111827_rgb},
      },
      easing::ease_in_out_sine<float>);

  print_sample_row(wheel, "sample(angle)", {0.00f, 0.25f, 0.50f, 0.75f, 1.00f});
  print_palette_row("palette(12)", wheel.palette(12));
  std::cout << "  sample_at(x, y) around center (0.50, 0.50)\n";
  print_sample_at(wheel, "east", 1.00f, 0.50f);
  print_sample_at(wheel, "south", 0.50f, 1.00f);
  print_sample_at(wheel, "west", 0.00f, 0.50f);
  print_sample_at(wheel, "north", 0.50f, 0.00f);
  print_palette_row("offset(0.125)", wheel.offset(0.125f).palette(12));
  print_palette_row("quantize(6)", wheel.quantize(6).palette(12));
  print_palette_row("reverse()", wheel.reverse().palette(12));
  print_palette_row("scale(2.0)", wheel.scale(2.0f).palette(12));
  print_palette_row("blend(dusk, 0.50)", wheel.blend(dusk, 0.50f).palette(12));
  print_palette_row("concat(dusk)", wheel.concat(dusk).palette(12));
  std::cout << "  angular scale() currently validates the factor but preserves sampling on the circle.\n";

  // ========================================================================
  section("6. sequence gradients");

  using linear_rgba = linear_gradient<rgba8_t>;

  auto seq_a = linear<rgba8_t>({
      {0.00f, 0x0F172A_rgb},
      {1.00f, 0x38BDF8_rgb},
  });
  auto seq_b = linear<rgba8_t>({
      {0.00f, 0x38BDF8_rgb},
      {1.00f, 0xF0ABFC_rgb},
  });
  auto seq_c = linear<rgba8_t>({
      {0.00f, 0xF0ABFC_rgb},
      {1.00f, 0xF97316_rgb},
  });

  std::vector<sequence_segment<linear_rgba>> segments = {
      {seq_a, 0.00f, 0.35f},
      {seq_b, 0.35f, 0.70f},
      {seq_c, 0.70f, 1.00f},
  };

  auto seq = sequence(std::move(segments), easing::smooth_step<float>);
  print_sample_row(seq, "sample(t)", {0.10f, 0.35f, 0.50f, 0.85f, 1.00f});
  print_palette_row("palette(15)", seq.palette(15));
  std::cout << "  segments().size() = " << seq.segments().size() << '\n';

  std::vector<sequence_segment<linear_rgba>> builder_segments = {
      {linear<rgba8_t>({{0.00f, 0x111827_rgb}, {1.00f, 0x6366F1_rgb}}), 0.00f, 0.50f},
  };
  sequence_gradient<linear_rgba> builder(builder_segments);
  builder.add_segment(linear<rgba8_t>({{0.00f, 0x6366F1_rgb}, {1.00f, 0xEC4899_rgb}}), 0.50f, 1.00f);
  print_palette_row("after add_segment()", builder.palette(9));
  builder.set_easing(easing::ease_in_out_sine<float>);
  print_palette_row("after set_easing()", builder.palette(9));
  builder.clear();
  builder.add_segment(linear<rgba8_t>({{0.00f, 0xEC4899_rgb}, {1.00f, 0xF59E0B_rgb}}), 0.00f, 1.00f);
  print_palette_row("after clear() + add", builder.palette(7));
  std::cout << "  rebuilt segments().size() = " << builder.segments().size() << '\n';

  std::cout << "\n";
  return 0;
}
