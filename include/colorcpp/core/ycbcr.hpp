/**
 * @file ycbcr.hpp
 * @brief YCbCr signal encoding: BT.601 / BT.709 / BT.2020 luma matrices with
 *        studio-swing (limited) or full-range 8/10-bit quantization.
 *
 * Float channels are normalized: Y in [0, 1], Cb/Cr signed in [-0.5, 0.5]
 * (neutral = 0). Range and bit depth do not live in the type — they are
 * quantization parameters (a limited-range code value is a storage format,
 * not a colour space), so @ref quantize / @ref dequantize take them
 * explicitly. Alpha is not part of YCbCr; carry it alongside the codes.
 *
 * Quantization follows the ITU-R studio swing:
 *   full 8    : round(Y * 255),        round(C * 255 + 128)
 *   limited 8 : 16 + round(Y * 219),   128 + round(C * 224)
 *   full 10   : round(Y * 1023),       round(C * 1023 + 512)
 *   limited 10: 64 + round(Y * 876),   512 + round(C * 896)
 * with half-away-from-zero rounding and clamping to the code range.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <colorcpp/core/color_base.hpp>
#include <colorcpp/core/io.hpp>

namespace colorcpp::core::ycbcr {

namespace channel {

struct y_tag {};
struct cb_tag {};
struct cr_tag {};

using f32_y = traits::basic_channel<y_tag, float, 0, 1>;  // luma [0, 1]
// Signed chroma in [-0.5, 0.5] (neutral 0), expressed as <-50, 50, 100>
// like Oklab's chroma channels.
using f32_cb = traits::basic_channel<cb_tag, float, -50, 50, 100>;
using f32_cr = traits::basic_channel<cr_tag, float, -50, 50, 100>;

}  // namespace channel

namespace model {

struct ycbcr601 {};   // BT.601 (SD video, Kr = 0.299)
struct ycbcr709 {};   // BT.709 (HD video, Kr = 0.2126)
struct ycbcr2020 {};  // BT.2020 (UHD video, Kr = 0.2627)

}  // namespace model

/** @brief Component code range: full swing (0..max) or limited/studio swing. */
enum class range {
  full,
  limited
};

/** @brief Quantization depth of the component codes. */
enum class bit_depth {
  depth8,
  depth10
};

/** @brief Quantized component codes (Y, Cb, Cr) at the requested range/depth. */
struct codes {
  int y;
  int cb;
  int cr;
};

}  // namespace colorcpp::core::ycbcr

namespace colorcpp::traits {

template <>
struct model_traits<core::ycbcr::model::ycbcr601> {
  using channels_type =
      std::tuple<core::ycbcr::channel::f32_y, core::ycbcr::channel::f32_cb, core::ycbcr::channel::f32_cr>;
  static constexpr std::string_view prefix = "ycbcr601";
};

template <>
struct model_traits<core::ycbcr::model::ycbcr709> {
  using channels_type =
      std::tuple<core::ycbcr::channel::f32_y, core::ycbcr::channel::f32_cb, core::ycbcr::channel::f32_cr>;
  static constexpr std::string_view prefix = "ycbcr709";
};

template <>
struct model_traits<core::ycbcr::model::ycbcr2020> {
  using channels_type =
      std::tuple<core::ycbcr::channel::f32_y, core::ycbcr::channel::f32_cb, core::ycbcr::channel::f32_cr>;
  static constexpr std::string_view prefix = "ycbcr2020";
};

}  // namespace colorcpp::traits

namespace colorcpp::core {

/**
 * @brief YCbCr with @c y(), @c cb(), and @c cr() accessors.
 * @tparam Model ycbcr::model::ycbcr601, ycbcr709, or ycbcr2020.
 */
template <typename Model>
struct basic_ycbcr : basic_color<Model> {
  using base = basic_color<Model>;

  using base::base;
  using base::data;

 public:
  COLORCPP_CHANNEL_ACCESSOR(ycbcr::channel::y_tag, y)

  COLORCPP_CHANNEL_ACCESSOR(ycbcr::channel::cb_tag, cb)

  COLORCPP_CHANNEL_ACCESSOR(ycbcr::channel::cr_tag, cr)
};

/** @brief YCbCr under the BT.601 luma matrix. */
using ycbcr601_t = basic_ycbcr<ycbcr::model::ycbcr601>;
/** @brief YCbCr under the BT.709 luma matrix. */
using ycbcr709_t = basic_ycbcr<ycbcr::model::ycbcr709>;
/** @brief YCbCr under the BT.2020 luma matrix. */
using ycbcr2020_t = basic_ycbcr<ycbcr::model::ycbcr2020>;

COLORCPP_STREAM_OPERATORS(colorcpp::core::basic_ycbcr)

}  // namespace colorcpp::core

COLORCPP_ENABLE_STRUCTURED_BINDINGS(colorcpp::core::basic_ycbcr)
COLORCPP_ENABLE_HASH(colorcpp::core::basic_ycbcr)

namespace colorcpp::core::ycbcr {

namespace details {

/** @brief Half-away-from-zero rounding (std::lround semantics). */
inline int round_half_away(double v) { return static_cast<int>(std::lround(v)); }

inline int clamp_code(int v, int max_code) { return std::clamp(v, 0, max_code); }

}  // namespace details

/**
 * @brief Quantize normalized YCbCr to integer component codes.
 * @param c Normalized YCbCr (Y in [0, 1], Cb/Cr in [-0.5, 0.5]; values
 *          outside the nominal range are clamped into the code space).
 * @param r Full or limited (studio swing) range.
 * @param d 8- or 10-bit depth.
 * @return Component codes in the requested range/depth.
 */
template <typename Model>
inline codes quantize(const basic_ycbcr<Model>& c, range r, bit_depth d) {
  const double y = static_cast<double>(c.y());
  const double cb = static_cast<double>(c.cb());
  const double cr = static_cast<double>(c.cr());

  if (d == bit_depth::depth8) {
    if (r == range::full) {
      return codes{details::clamp_code(details::round_half_away(y * 255.0), 255),
                   details::clamp_code(details::round_half_away(cb * 255.0 + 128.0), 255),
                   details::clamp_code(details::round_half_away(cr * 255.0 + 128.0), 255)};
    }
    return codes{details::clamp_code(16 + details::round_half_away(y * 219.0), 255),
                 details::clamp_code(128 + details::round_half_away(cb * 224.0), 255),
                 details::clamp_code(128 + details::round_half_away(cr * 224.0), 255)};
  }
  if (r == range::full) {
    return codes{details::clamp_code(details::round_half_away(y * 1023.0), 1023),
                 details::clamp_code(details::round_half_away(cb * 1023.0 + 512.0), 1023),
                 details::clamp_code(details::round_half_away(cr * 1023.0 + 512.0), 1023)};
  }
  return codes{details::clamp_code(64 + details::round_half_away(y * 876.0), 1023),
               details::clamp_code(512 + details::round_half_away(cb * 896.0), 1023),
               details::clamp_code(512 + details::round_half_away(cr * 896.0), 1023)};
}

/**
 * @brief Dequantize integer component codes back to normalized YCbCr.
 * @tparam Color Target YCbCr type, e.g. ycbcr709_t (codes carry no matrix
 *         information, so the type only names the result).
 */
template <typename Color>
inline Color dequantize(const codes& q, range r, bit_depth d) {
  double y;
  double cb;
  double cr;
  if (d == bit_depth::depth8) {
    if (r == range::full) {
      y = static_cast<double>(q.y) / 255.0;
      cb = (static_cast<double>(q.cb) - 128.0) / 255.0;
      cr = (static_cast<double>(q.cr) - 128.0) / 255.0;
    } else {
      y = (static_cast<double>(q.y) - 16.0) / 219.0;
      cb = (static_cast<double>(q.cb) - 128.0) / 224.0;
      cr = (static_cast<double>(q.cr) - 128.0) / 224.0;
    }
  } else if (r == range::full) {
    y = static_cast<double>(q.y) / 1023.0;
    cb = (static_cast<double>(q.cb) - 512.0) / 1023.0;
    cr = (static_cast<double>(q.cr) - 512.0) / 1023.0;
  } else {
    y = (static_cast<double>(q.y) - 64.0) / 876.0;
    cb = (static_cast<double>(q.cb) - 512.0) / 896.0;
    cr = (static_cast<double>(q.cr) - 512.0) / 896.0;
  }
  return Color{static_cast<float>(y), static_cast<float>(cb), static_cast<float>(cr)};
}

}  // namespace colorcpp::core::ycbcr
