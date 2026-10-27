// 0xFF0000 is 6 digits where _rgba requires 8 (RRGGBBAA) — must be ill-formed.
// The ULL overload used to decode this as {0, 255, 0, 0}: fully transparent
// green instead of opaque red.
#include <colorcpp/io/literals/rgb.hpp>

using namespace colorcpp::io::literals;

auto missing_alpha = 0xFF0000_rgba;

int main() {}
