/**
 * @file equal.hpp
 * @brief Main color comparison dispatch.
 */

#pragma once

#include <colorcpp/operations/compare/epsilon.hpp>
#include <colorcpp/operations/compare/exact.hpp>
#include <colorcpp/operations/compare/perceptual.hpp>
#include <colorcpp/operations/compare/relative.hpp>

namespace colorcpp::operations::compare {

/**
 * @brief Compare two colors using the default (exact) strategy.
 * @tparam Color Color type.
 * @param a First color.
 * @param b Second color.
 * @return True if colors are exactly equal.
 */
template <typename Color>
constexpr bool equal(const Color& a, const Color& b) noexcept {
  return exact_equal(a, b);
}

/**
 * @brief Compare two colors using a specific policy.
 * @tparam Policy Comparison policy type.
 * @tparam Color Color type.
 * @tparam Args Additional arguments for the policy.
 * @param a First color.
 * @param b Second color.
 * @param args Additional arguments for the policy.
 * @return True if colors are equal according to the policy.
 */
template <typename Policy, typename Color, typename... Args>
constexpr auto equal(const Color& a, const Color& b, Args&&... args) noexcept(noexcept(Policy::compare(a, b, args...)))
    -> decltype(Policy::compare(a, b, args...)) {
  return Policy::compare(a, b, args...);
}

}  // namespace colorcpp::operations::compare
