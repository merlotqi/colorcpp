#pragma once

// Include all color conversion headers
#include <color/conversion/conversion.hpp>

// Include all color core headers
#include <color/core/cmyk.hpp>
#include <color/core/color.hpp>
#include <color/core/hsl.hpp>
#include <color/core/hsv.hpp>
#include <color/core/rgb.hpp>

// Include all color operations headers
#include <color/operations/adjust.hpp>
#include <color/operations/blend.hpp>
#include <color/operations/io.hpp>

// Include all color palettes headers
#include <color/palettes/web.hpp>

// Include all color traits headers
#include <color/traits/concepts.hpp>

// Include all color utils headers
#include <color/utils/maths.hpp>

// Include literals headers
#include <color/literals/literals.hpp>

namespace color {

using namespace core;
using namespace conversion;
using namespace operations;
using namespace palettes;
using namespace traits;
using namespace literals;

}  // namespace color
