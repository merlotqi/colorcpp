#pragma once

#include "color/core/cmyk.hpp"
#include "color/core/color.hpp"
#include "color/core/hsl.hpp"
#include "color/core/hsv.hpp"
#include "color/core/rgb.hpp"

#include "color/conversion/hsv_to_rgb.hpp"
#include "color/conversion/rgb_to_hsv.hpp"

#include "color/operations/adjust.hpp"
#include "color/operations/blend.hpp"
#include "color/operations/filter.hpp"

#include "color/palettes/web.hpp"

#include "color/traits/concepts.hpp"

#include "color/utils/literals.hpp"

namespace color {

using namespace core;
using namespace conversion;
using namespace operations;
using namespace palettes;
using namespace traits;
using namespace literals;

namespace colors = core::colors;

namespace color_constants = core::color_constants;

} // namespace color
