#include <colorcpp/colorcpp.hpp>
#include <iostream>
#include <vector>

using namespace colorcpp::core;
using namespace colorcpp::operations;
using namespace colorcpp::operations::random;
using namespace colorcpp::operations::conversion;
using namespace colorcpp::io::ansi;

static void section(const char* title) { std::cout << '\n' << bold() << "=== " << title << " ===" << reset() << '\n'; }

template <typename Color>
void print_rgbf_row(const std::vector<Color>& colors) {
  for (const auto& c : colors) print_swatch(std::cout, color_cast<rgba8_t>(c), 2);
  std::cout << '\n';
}

int main() {
  std::cout << bold() << "ColorCPP random generation" << reset() << "\n";

  section("Seed reproducibility");
  rgb8_generator gen1(42u);
  rgb8_generator gen2(42u);
  auto c1 = gen1.next(), c2 = gen2.next();
  std::cout << "  Same seed produces same color: " << (c1.r() == c2.r() ? "OK" : "FAIL") << "\n  ";
  print_swatch(std::cout, color_cast<rgba8_t>(c1), 2);
  std::cout << '\n';

  section("Generic random_color");
  auto rgb_color = random_color<rgbf_t>(123);
  auto hsl_color = random_color<hsl_float_t>(456);
  std::cout << "  RGB:  ";
  print_swatch(std::cout, color_cast<rgba8_t>(rgb_color), 2);
  std::cout << " R=" << rgb_color.r() << " G=" << rgb_color.g() << " B=" << rgb_color.b() << "\n";
  std::cout << "  HSL:  ";
  print_swatch(std::cout, color_cast<rgba8_t>(hsl_color), 2);
  std::cout << " H=" << hsl_color.h() << " S=" << hsl_color.s() << " L=" << hsl_color.l() << "\n";

  section("Batch generation");
  auto batch = random_colors<rgbf_t>(5, 789);
  std::cout << "  " << batch.size() << " colors: ";
  print_rgbf_row(batch);

  section("Multi-color space generators");
  {
    std::mt19937 rng(100);
    oklab_generator oklab_gen(rng);
    std::cout << "  OkLab:  ";
    print_swatch(std::cout, color_cast<rgba8_t>(oklab_gen.next()), 2);
    std::cout << '\n';

    oklch_generator oklch_gen(rng);
    std::cout << "  OkLCH:  ";
    print_swatch(std::cout, color_cast<rgba8_t>(oklch_gen.next()), 2);
    std::cout << '\n';

    hwb_generator hwb_gen(rng);
    std::cout << "  HWB:    ";
    print_swatch(std::cout, color_cast<rgba8_t>(hwb_gen.next()), 2);
    std::cout << '\n';
  }

  section("Harmony modes");
  {
    std::mt19937 rng(200);
    harmony_gen hgen(rng);
    auto base = hgen.next();

    std::cout << "  Base: ";
    print_swatch(std::cout, color_cast<rgba8_t>(base), 2);
    std::cout << '\n';

    const char* mode_names[] = {"analogous", "complementary", "split_complementary", "triadic", "tetradic", "square"};
    harmony_mode modes[] = {harmony_mode::analogous, harmony_mode::complementary, harmony_mode::split_complementary,
                            harmony_mode::triadic,   harmony_mode::tetradic,      harmony_mode::square};

    for (int i = 0; i < 6; ++i) {
      auto palette = hgen.generate_palette(base, modes[i]);
      std::cout << "  " << mode_names[i] << ": ";
      for (std::size_t j = 0; j < palette.size(); ++j) print_swatch(std::cout, color_cast<rgba8_t>(palette[j]), 2);
      std::cout << '\n';
    }
  }

  section("Poisson disk sampling (hue spacing)");
  {
    std::mt19937 rng(300);
    harmony_gen hgen(rng);
    std::vector<float> samples;
    std::cout << "  ";
    for (int i = 0; i < 6; ++i) {
      auto c = hgen.next_poisson(samples, 30.0f);
      print_swatch(std::cout, color_cast<rgba8_t>(c), 2);
    }
    std::cout << '\n';
  }

  section("Golden angle sequence");
  {
    std::mt19937 rng(400);
    golden_gen ggen(rng, 1.0f, 0.5f);
    auto golden_batch = ggen.generate_n(8);
    std::cout << "  ";
    print_rgbf_row(golden_batch);
  }

  section("Options validation");
  {
    std::mt19937 rng(500);
    try {
      hsl_generator bad(rng, {360.0f, 0.0f});
      std::cout << "  Invalid options: FAIL (should have thrown)\n";
    } catch (const std::invalid_argument&) {
      std::cout << "  Invalid options correctly rejected: OK\n";
    }

    hsl_generator::options opts{0.0f, 360.0f, 0.6f, 0.8f, 0.4f, 0.6f};
    hsl_generator good(rng, opts);
    auto c = good.next();
    bool s_ok = c.s() >= 0.6f && c.s() <= 0.8f;
    bool l_ok = c.l() >= 0.4f && c.l() <= 0.6f;
    std::cout << "  Custom options S in [0.6,0.8]: " << (s_ok ? "OK" : "FAIL") << "\n";
    std::cout << "  Custom options L in [0.4,0.6]: " << (l_ok ? "OK" : "FAIL") << "\n";
  }

  section("Engine sharing");
  {
    std::mt19937 shared_rng(600);
    rgb8_generator gen_a(shared_rng);
    rgb8_generator gen_b(shared_rng);

    auto a1 = gen_a.next();
    auto b1 = gen_b.next();
    auto a2 = gen_a.next();
    auto b2 = gen_b.next();

    std::cout << "  Generator A: ";
    print_swatch(std::cout, color_cast<rgba8_t>(a1), 2);
    print_swatch(std::cout, color_cast<rgba8_t>(a2), 2);
    std::cout << "\n  Generator B: ";
    print_swatch(std::cout, color_cast<rgba8_t>(b1), 2);
    print_swatch(std::cout, color_cast<rgba8_t>(b2), 2);
    std::cout << '\n';
  }

  std::cout << '\n' << bold() << "Done." << reset() << '\n';
  return 0;
}
