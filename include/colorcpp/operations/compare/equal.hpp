/**
 * @file equal.hpp
 * @brief Main color comparison dispatch.
 */

#pragma once

#include <utility>

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
 *
 * Mixed-type support depends on the selected policy. For example,
 * perceptual_policy accepts mixed-type inputs because it converts both
 * colors before comparison, while same-type channel policies compare the
 * provided channel layouts directly.
 *
 * @tparam Policy Comparison policy type.
 * @tparam ColorA First color type.
 * @tparam ColorB Second color type.
 * @tparam Args Additional arguments for the policy.
 * @param a First color.
 * @param b Second color.
 * @param args Additional arguments for the policy.
 * @return True if colors are equal according to the policy.
 */
template <typename Policy, typename ColorA, typename ColorB, typename... Args>
constexpr auto equal(const ColorA& a, const ColorB& b, Args&&... args)
    noexcept(noexcept(Policy::compare(a, b, std::forward<Args>(args)...)))
        -> decltype(Policy::compare(a, b, std::forward<Args>(args)...)) {
  return Policy::compare(a, b, std::forward<Args>(args)...);
}

}  // namespace colorcpp::operations::compare
