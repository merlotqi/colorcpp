# Error System Design

## Summary

Replace all raw `throw std::out_of_range` / `std::invalid_argument` calls with structured error handling based on `std::error_category`. Introduce a `color_errc` enum and `color_error_category`, then wrap each throw site with a `report_error()` helper that produces `std::system_error`.

**Approach:** Keep exceptions, upgrade to `std::system_error` + custom error codes (option 1).

## Error codes (11 codes, medium granularity)

| Code | Exception base | Trigger |
|------|---------------|---------|
| `invalid_channel_value` | `out_of_range` | Channel value outside model range (`color_base`) |
| `value_out_of_range` | `out_of_range` | Palette index out of bounds, hex literal exceeds 24/32 bits |
| `invalid_hex_format` | `invalid_argument` | Hex string contains non-hex digit or wrong length (not 3/4/6/8) |
| `unknown_named_color` | `invalid_argument` | CSS named color lookup failure |
| `insufficient_stops` | `invalid_argument` | Gradient/interpolation has < minimum stops (empty or <2) |
| `invalid_stop_position` | `out_of_range` | Stop/segment position not in [0, 1] |
| `invalid_segment` | `invalid_argument` | Segment start >= end, or segments overlap |
| `invalid_scale_factor` | `invalid_argument` | Gradient scale factor not positive |
| `invalid_quantize_levels` | `invalid_argument` | Quantize/stepped gradient levels < 1 |
| `invalid_option_range` | `invalid_argument` | Random generator min > max |
| `non_finite_value` | `invalid_argument` | Color value contains NaN or Inf |

## Module structure

```
include/colorcpp/error/
├── error.hpp           # Umbrella header
├── color_error.hpp     # color_errc enum + make_error_code + report_error helper
└── category.hpp        # color_error_category (std::error_category subclass)
```

- `color_error.hpp` — `color_errc` enum, `make_error_code(color_errc)`, `report_error(color_errc, const char* msg)` helper (throws `std::system_error` internally).
- `category.hpp` — `color_error_category` singleton via `color_error_category::instance()`. Implements `name()` and `message()`.
- `error.hpp` — umbrella, includes both.
- Zero dependency on other colorcpp modules. Only `<system_error>` and `<string>`.

## Migration mapping

Every `throw std::<exception>("colorcpp: <msg>")` becomes `report_error(<errc>, "colorcpp: <msg>")`.

| File | Count | Old | New code |
|------|-------|-----|----------|
| `core/color_base.hpp` | 2 | `std::out_of_range` | `invalid_channel_value` |
| `core/palette_set.hpp` | 1 | `std::out_of_range` | `value_out_of_range` |
| `io/literals/rgb.hpp` | 3 | `std::out_of_range` | `value_out_of_range` |
| `io/literals/details.hpp` | 2 | `std::invalid_argument` | `invalid_hex_format` |
| `io/css/named_colors.hpp` | 1 | `std::invalid_argument` | `unknown_named_color` |
| `algorithms/gradient/details.hpp` | 5 | mixed | `insufficient_stops`, `invalid_stop_position`, `non_finite_value` |
| `algorithms/gradient/stops.hpp` | 6 | mixed | `invalid_stop_position`, `value_out_of_range` |
| `algorithms/gradient/angular.hpp` | 2 | `std::invalid_argument` | `invalid_scale_factor`, `invalid_quantize_levels` |
| `algorithms/gradient/box.hpp` | 2 | `std::invalid_argument` | `invalid_scale_factor`, `invalid_quantize_levels` |
| `algorithms/gradient/diamond.hpp` | 2 | `std::invalid_argument` | `invalid_scale_factor`, `invalid_quantize_levels` |
| `algorithms/gradient/linear.hpp` | 2 | `std::invalid_argument` | `invalid_scale_factor`, `invalid_quantize_levels` |
| `algorithms/gradient/radial.hpp` | 2 | `std::invalid_argument` | `invalid_scale_factor`, `invalid_quantize_levels` |
| `algorithms/gradient/sequence.hpp` | 4 | `std::invalid_argument` | `invalid_stop_position`, `invalid_segment`, `insufficient_stops` |
| `algorithms/gradient/stepped.hpp` | 2 | `std::invalid_argument` | `invalid_quantize_levels` |
| `operations/interpolate/color_lerp.hpp` | 8 | `std::invalid_argument` | `insufficient_stops` |
| `operations/random/hsl.hpp` | 2 | `std::invalid_argument` | `invalid_option_range` |
| `operations/random/luminance.hpp` | 2 | `std::invalid_argument` | `invalid_option_range` |

## Backward compatibility

- `what()` strings unchanged.
- `catch (std::exception&)` still works.
- `catch (const std::system_error& e)` gives structured `e.code()` access.
- Callers can compare: `e.code() == color_errc::insufficient_stops`.

## Test plan

- Unit test per error code (11 tests) — verify `make_error_code()` and `message()`.
- Verify migration completeness via `grep -rn "throw std::" include/` returns zero results.
- Existing tests continue to pass — `EXPECT_THROW` with `std::system_error` or keep `std::exception` base.
