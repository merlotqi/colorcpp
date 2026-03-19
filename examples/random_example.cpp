#include <colorcpp/colorcpp.hpp>
#include <iostream>

using namespace colorcpp::operations::random;

int main() {
  // 1. seed
  rgb8_generator gen1(42u);
  rgb8_generator gen2(42u);
  auto c1 = gen1.next(), c2 = gen2.next();
  std::cout << "seed reproducibility: " << (c1.r() == c2.r() ? "OK" : "FAIL") << '\n';

  // 2. generate_n
  std::mt19937 rng(7);
  rgb8_generator gen3(rng);
  auto batch = gen3.generate_n(3);
  std::cout << "generate_n count=" << batch.size() << '\n';

  // 3. options
  std::mt19937 rng2(1);
  try {
    hsl_generator bad(rng2, {360.0f, 0.0f});  // h_min > h_max
    std::cout << "options validation: FAIL\n";
  } catch (const std::invalid_argument&) {
    std::cout << "options validation: OK\n";
  }

  // 4. generate_palette
  std::mt19937 rng3(99);
  harmony_gen hgen(rng3);
  auto base = hgen.next();
  auto pal = hgen.generate_palette(base, harmony_mode::triadic);
  std::cout << "triadic palette size=" << pal.size() << " (expected 3)\n";

  // 5. golden_gen with options
  hsl_generator::options o{0.0f, 360.0f, 60.0f, 80.0f, 40.0f, 60.0f};
  golden_gen ggen(42u, o);
  auto gc = ggen.next();
  std::cout << "golden S=" << gc.s() << " (in [60,80]=" << (gc.s() >= 60 && gc.s() <= 80 ? "OK" : "FAIL") << ")\n";

  // 6. hsv_generator alias
  std::mt19937 rng4(5);
  hsv_generator hsvgen(rng4);
  auto hsvc = hsvgen.next();
  std::cout << "hsv_generator H=" << hsvc.h() << '\n';

  // 7. golden generate_n
  auto golden_batch = ggen.generate_n(4);
  std::cout << "golden generate_n count=" << golden_batch.size() << '\n';

  return 0;
}
