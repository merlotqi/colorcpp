// compare::equal on non-color arguments must be ill-formed. The channel
// policies guard entry with a static_assert (clear message) instead of dying
// deep inside get_index/channel dispatch.
#include <colorcpp/operations/compare.hpp>

using namespace colorcpp::operations::compare;

auto bad = equal(42, 42);

int main() {}
