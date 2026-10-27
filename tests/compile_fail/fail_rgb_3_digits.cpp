// 0xFFF has 3 hex digits (CSS shorthand style) — must be ill-formed for _rgb,
// which requires exactly 6. The ULL overload used to decode it as {0, 15, 255}.
#include <colorcpp/io/literals/rgb.hpp>

using namespace colorcpp::io::literals;

auto shorthand = 0xFFF_rgb;

int main() {}
