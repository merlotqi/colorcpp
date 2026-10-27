// A decimal number spelled without 0x must be ill-formed for _rgb: the digits
// are parsed as hex, yielding 8 digits instead of 6. The ULL overload used to
// accept 16711680_rgb and silently produce red.
#include <colorcpp/io/literals/rgb.hpp>

using namespace colorcpp::io::literals;

auto decimal_spelled = 16711680_rgb;

int main() {}
