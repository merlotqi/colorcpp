/**
 * @file sequence.hpp
 * @brief Sequence gradient generation.
 *
 * Sequence gradients combine multiple gradients at different positions along [0, 1].
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/algorithms/gradient/details.hpp>
#include <colorcpp/algorithms/gradient/easing.hpp>
#include <colorcpp/operations/interpolate.hpp>
#include <stdexcept>
#include <vector>

namespace colorcpp::algorithms::gradient {

/**
 * @brief A segment in a sequence gradient.
 * @tparam Gradient The gradient type.
 */
template <typename Gradient>
struct sequence_segment {
  using color_type = typename Gradient::color_type;

  Gradient gradient;  // The gradient for this segment
  float start;        // Start position in [0, 1]
  float end;          // End position in [0, 1]

  /**
   * @brief Construct a sequence segment.
   * @param grad The gradient.
   * @param s Start position.
   * @param e End position.
   */
  sequence_segment(Gradient grad, float s, float e) : gradient(std::move(grad)), start(s), end(e) {
    if (s < 0.0f || s > 1.0f || e < 0.0f || e > 1.0f) {
      throw std::invalid_argument("colorcpp: segment positions must be in [0, 1]");
    }
    if (s >= e) {
      throw std::invalid_argument("colorcpp: segment start must be less than end");
    }
  }
};

/**
 * @brief Sequence gradient generator.
 * @tparam Gradient The gradient type.
 */
template <typename Gradient>
class sequence_gradient {
 public:
  using color_type = typename Gradient::color_type;
  using segment_type = sequence_segment<Gradient>;
  using easing_type = easing::easing_function<float>;

  /**
   * @brief Construct a sequence gradient from segments.
   * @param segments The sequence segments.
   * @param easing Easing function (default: linear).
   */
  explicit sequence_gradient(
      std::vector<segment_type> segments, easing_type easing = [](float t) { return easing::linear(t); })
      : segments_(std::move(segments)), easing_(std::move(easing)) {
    std::sort(segments_.begin(), segments_.end(),
              [](const segment_type& a, const segment_type& b) { return a.start < b.start; });
    validate_segments();
  }

  /**
   * @brief Sample the gradient at a given position.
   * @param t Position in [0, 1].
   * @return The interpolated color.
   */
  color_type sample(float t) const {
    t = std::clamp(t, 0.0f, 1.0f);

    if (t <= segments_.front().start) {
      return segments_.front().gradient.sample(0.0f);
    }

    for (std::size_t i = 0; i < segments_.size(); ++i) {
      const auto& segment = segments_[i];
      if (t >= segment.start && t <= segment.end) {
        float local_t = (t - segment.start) / (segment.end - segment.start);
        local_t = details::clamp_01(easing_(local_t));
        return segment.gradient.sample(local_t);
      }

      if (i + 1 < segments_.size()) {
        const auto& next = segments_[i + 1];
        if (t > segment.end && t < next.start) {
          float gap_t = (t - segment.end) / (next.start - segment.end);
          return operations::interpolate::lerp(segment.gradient.sample(1.0f), next.gradient.sample(0.0f), gap_t);
        }
      }
    }

    return segments_.back().gradient.sample(1.0f);
  }

  /**
   * @brief Generate a palette of colors.
   * @param count Number of colors to generate.
   * @return Vector of colors.
   */
  std::vector<color_type> palette(std::size_t count) const {
    if (count == 0) {
      return {};
    }

    std::vector<color_type> palette;
    palette.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
      auto t = static_cast<float>(i) / static_cast<float>(count - 1);
      palette.push_back(sample(t));
    }

    return palette;
  }

  /**
   * @brief Get the segments.
   * @return Reference to the segments.
   */
  const std::vector<segment_type>& segments() const noexcept { return segments_; }

  /**
   * @brief Get the easing function.
   * @return Reference to the easing function.
   */
  const easing_type& easing() const noexcept { return easing_; }

  /**
   * @brief Set the easing function.
   * @param easing New easing function.
   */
  void set_easing(easing_type easing) { easing_ = std::move(easing); }

  /**
   * @brief Add a segment to the sequence.
   * @param gradient The gradient.
   * @param start Start position.
   * @param end End position.
   */
  void add_segment(Gradient gradient, float start, float end) {
    segments_.emplace_back(std::move(gradient), start, end);
    std::sort(segments_.begin(), segments_.end(),
              [](const segment_type& a, const segment_type& b) { return a.start < b.start; });
    validate_segments();
  }

  /**
   * @brief Clear all segments.
   */
  void clear() noexcept { segments_.clear(); }

 private:
  std::vector<segment_type> segments_;
  easing_type easing_;

  void validate_segments() const {
    if (segments_.empty()) {
      throw std::invalid_argument("colorcpp: sequence gradient must have at least one segment");
    }

    // Check for overlapping segments after sorting.
    for (std::size_t i = 0; i < segments_.size() - 1; ++i) {
      if (segments_[i].end > segments_[i + 1].start) {
        throw std::invalid_argument("colorcpp: sequence segments must not overlap");
      }
    }
  }
};

/**
 * @brief Create a sequence gradient from segments.
 * @tparam Gradient Gradient type.
 * @param segments The sequence segments.
 * @param easing Easing function (default: linear).
 * @return Sequence gradient object.
 */
template <typename Gradient>
sequence_gradient<Gradient> sequence(
    std::vector<sequence_segment<Gradient>> segments,
    easing::easing_function<float> easing = [](float t) { return easing::linear(t); }) {
  return sequence_gradient<Gradient>(std::move(segments), std::move(easing));
}

}  // namespace colorcpp::algorithms::gradient
