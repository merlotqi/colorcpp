#pragma once

// Include all color conversion headers
#include "color/conversion/cmyk_to_hsl.hpp"
#include "color/conversion/cmyk_to_hsv.hpp"
#include "color/conversion/cmyk_to_rgb.hpp"
#include "color/conversion/hsl_to_cmyk.hpp"
#include "color/conversion/hsl_to_hsv.hpp"
#include "color/conversion/hsl_to_rgb.hpp"
#include "color/conversion/hsv_to_cmyk.hpp"
#include "color/conversion/hsv_to_hsl.hpp"
#include "color/conversion/hsv_to_rgb.hpp"
#include "color/conversion/rgb_to_cmyk.hpp"
#include "color/conversion/rgb_to_hsl.hpp"
#include "color/conversion/rgb_to_hsv.hpp"

// Include all color core headers
#include "color/core/cmyk.hpp"
#include "color/core/color.hpp"
#include "color/core/hsl.hpp"
#include "color/core/hsv.hpp"
#include "color/core/rgb.hpp"

// Include all color operations headers
#include "color/operations/adjust.hpp"
#include "color/operations/blend.hpp"
#include "color/operations/filter.hpp"

// Include all color palettes headers
#include "color/palettes/web.hpp"

// Include all color traits headers
#include "color/traits/concepts.hpp"

// Include all color utils headers
#include "color/utils/literals.hpp"

namespace color {

using namespace core;
using namespace conversion;
using namespace operations;
using namespace palettes;
using namespace traits;
using namespace literals;

namespace colors = core::colors;

}  // namespace color
