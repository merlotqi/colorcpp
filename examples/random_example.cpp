#include <colorcpp/colorcpp.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace colorcpp;
using namespace colorcpp::operations::random;

// ANSI color codes for terminal visualization
namespace ansi {
const char* reset = "\033[0m";
const char* bold = "\033[1m";

std::string color_block(float r, float g, float b) {
  int ri = static_cast<int>(r * 255);
  int gi = static_cast<int>(g * 255);
  int bi = static_cast<int>(b * 255);
  return "\033[48;2;" + std::to_string(ri) + ";" + std::to_string(gi) + ";" + std::to_string(bi) + "m  " + reset;
}

std::string color_block(const core::rgbf_t& c) { return color_block(c.r(), c.g(), c.b()); }

}  // namespace ansi

int main() {
  std::cout << ansi::bold << "=== ColorCPP Random Generation Examples ===" << ansi::reset << "\n\n";

  // 1. Basic random generation with seed reproducibility
  std::cout << ansi::bold << "1. Seed Reproducibility" << ansi::reset << "\n";
  rgb8_generator gen1(42u);
  rgb8_generator gen2(42u);
  auto c1 = gen1.next(), c2 = gen2.next();
  std::cout << "  Same seed produces same color: " << (c1.r() == c2.r() ? "OK" : "FAIL") << "\n";
  std::cout << "  Color: " << ansi::color_block(c1.r() / 255.0f, c1.g() / 255.0f, c1.b() / 255.0f) << "\n\n";

  // 2. Generic random color interface
  std::cout << ansi::bold << "2. Generic Random Color Interface" << ansi::reset << "\n";
  auto rgb_color = random_color<core::rgbf_t>(123);
  auto hsl_color = random_color<core::hsl_float_t>(456);
  std::cout << "  RGB:  " << ansi::color_block(rgb_color) << " R=" << rgb_color.r() << " G=" << rgb_color.g()
            << " B=" << rgb_color.b() << "\n";
  std::cout << "  HSL:  " << ansi::color_block(conversion::color_cast<core::rgbf_t>(hsl_color))
            << " H=" << hsl_color.h() << " S=" << hsl_color.s() << " L=" << hsl_color.l() << "\n\n";

  // 3. Batch generation
  std::cout << ansi::bold << "3. Batch Generation" << ansi::reset << "\n";
  auto batch = random_colors<core::rgbf_t>(5, 789);
  std::cout << "  Generated " << batch.size() << " colors: ";
  for (const auto& c : batch) {
    std::cout << ansi::color_block(c);
  }
  std::cout << "\n\n";

  // 4. Different color spaces
  std::cout << ansi::bold << "4. Multi-Color Space Support" << ansi::reset << "\n";
  {
    std::mt19937 rng(100);
    oklab_generator oklab_gen(rng);
    auto oklab_c = oklab_gen.next();
    std::cout << "  OkLab:  " << ansi::color_block(conversion::color_cast<core::rgbf_t>(oklab_c)) << "\n";

    oklch_generator oklch_gen(rng);
    auto oklch_c = oklch_gen.next();
    std::cout << "  OkLCH:  " << ansi::color_block(conversion::color_cast<core::rgbf_t>(oklch_c)) << "\n";

    hwb_generator hwb_gen(rng);
    auto hwb_c = hwb_gen.next();
    std::cout << "  HWB:    " << ansi::color_block(conversion::color_cast<core::rgbf_t>(hwb_c)) << "\n";
  }
  std::cout << "\n";

  // 5. All harmony modes
  std::cout << ansi::bold << "5. Harmony Modes" << ansi::reset << "\n";
  {
    std::mt19937 rng(200);
    harmony_gen hgen(rng);
    auto base = hgen.next();

    std::cout << "  Base color: " << ansi::color_block(conversion::color_cast<core::rgbf_t>(base)) << "\n";

    const char* mode_names[] = {"analogous", "complementary", "split_complementary", "triadic", "tetradic", "square"};
    harmony_mode modes[] = {harmony_mode::analogous, harmony_mode::complementary, harmony_mode::split_complementary,
                            harmony_mode::triadic,   harmony_mode::tetradic,      harmony_mode::square};

    for (int i = 0; i < 6; ++i) {
      auto palette = hgen.generate_palette(base, modes[i]);
      std::cout << "  " << mode_names[i] << ": ";
      for (std::size_t j = 0; j < palette.size(); ++j) {
        std::cout << ansi::color_block(conversion::color_cast<core::rgbf_t>(palette[j]));
      }
      std::cout << "\n";
    }
  }
  std::cout << "\n";

  // 6. Poisson disk sampling
  std::cout << ansi::bold << "6. Poisson Disk Sampling" << ansi::reset << "\n";
  {
    std::mt19937 rng(300);
    harmony_gen hgen(rng);
    std::vector<float> samples;
    std::cout << "  Generated hues (min 30° apart): ";
    for (int i = 0; i < 6; ++i) {
      auto c = hgen.next_poisson(samples, 30.0f);
      std::cout << ansi::color_block(conversion::color_cast<core::rgbf_t>(c)) << " ";
    }
    std::cout << "\n\n";
  }

  // 7. Golden angle sequence
  std::cout << ansi::bold << "7. Golden Angle Sequence" << ansi::reset << "\n";
  {
    std::mt19937 rng(400);
    golden_gen ggen(rng, 1.0f, 0.5f);
    auto golden_batch = ggen.generate_n(8);
    std::cout << "  8 colors with golden angle separation: ";
    for (const auto& c : golden_batch) {
      std::cout << ansi::color_block(conversion::color_cast<core::rgbf_t>(c));
    }
    std::cout << "\n\n";
  }

  // 8. Options validation
  std::cout << ansi::bold << "8. Options Validation" << ansi::reset << "\n";
  {
    std::mt19937 rng(500);
    try {
      hsl_generator bad(rng, {360.0f, 0.0f});  // h_min > h_max
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
  std::cout << "\n";

  // 9. Engine sharing
  std::cout << ansi::bold << "9. Engine Sharing" << ansi::reset << "\n";
  {
    std::mt19937 shared_rng(600);
    rgb8_generator gen_a(shared_rng);
    rgb8_generator gen_b(shared_rng);

    // Both generators use the same engine state
    auto a1 = gen_a.next();
    auto b1 = gen_b.next();
    auto a2 = gen_a.next();
    auto b2 = gen_b.next();

    std::cout << "  Generator A sequence: " << ansi::color_block(a1.r() / 255.0f, a1.g() / 255.0f, a1.b() / 255.0f)
              << " " << ansi::color_block(a2.r() / 255.0f, a2.g() / 255.0f, a2.b() / 255.0f) << "\n";
    std::cout << "  Generator B sequence: " << ansi::color_block(b1.r() / 255.0f, b1.g() / 255.0f, b1.b() / 255.0f)
              << " " << ansi::color_block(b2.r() / 255.0f, b2.g() / 255.0f, b2.b() / 255.0f) << "\n";
  }
  std::cout << "\n";

  std::cout << ansi::bold << "=== All examples completed successfully! ===" << ansi::reset << "\n";

  return 0;
}
