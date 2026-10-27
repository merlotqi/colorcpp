// 0x1000000 has 7 hex digits — must be ill-formed.
// The removed operator""_rgb(unsigned long long) used to accept this and throw
// std::out_of_range at runtime (tests used to assert exactly that).
#include <colorcpp/io/literals/rgb.hpp>

using namespace colorcpp::io::literals;

// Runtime context on purpose: a constexpr context would fail today for the
// wrong reason (the ULL overload throws during constant evaluation).
auto too_many = 0x1000000_rgb;

int main() {}
