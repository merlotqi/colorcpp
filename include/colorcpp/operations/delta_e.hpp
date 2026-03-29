/**
 * @file delta_e.hpp
 * @brief Color difference metrics (ΔE) after conversion to CIELAB D65.
 *
 * This is an aggregate header that includes all delta_e components.
 * Individual components can be included separately for faster compilation.
 *
 * @note Rule-of-thumb for ΔE2000: <1 imperceptible, 1–2 close inspection, 2–10 obvious, >50 very different.
 */

#pragma once

#include <colorcpp/operations/delta_e/de2000.hpp>
#include <colorcpp/operations/delta_e/de76.hpp>
#include <colorcpp/operations/delta_e/de94.hpp>
#include <colorcpp/operations/delta_e/helpers.hpp>
