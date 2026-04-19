Color harmony analysis
======================

Color harmony analysis evaluates how well colors work together in a palette, and can suggest improvements or corrections to achieve specific harmony schemes.

In colorcpp
-----------

* Header: ``include/colorcpp/algorithms/harmony.hpp``
* Namespace: ``colorcpp::algorithms::harmony``

Main entry points:

* ``assess(palette)`` — evaluate how harmonious a palette is
* ``assess_weighted(palette, weights)`` — weighted harmony assessment with configurable channel weights
* ``correct(palette, target_scheme)`` — adjust colors and report whether correction was applied
* ``correct(palette, target_scheme, tolerance)`` — correction with tolerance threshold
* ``correct_minimal(palette, target_scheme)`` — minimal correction (only adjust largest deviation)
* ``correct_hue_only(palette, target_scheme)`` — hue only correction (preserve saturation/lightness)
* ``suggest(palette)`` — provide suggestions for improving harmony

Harmony schemes
---------------

The ``harmony_scheme`` enum defines supported color harmony patterns:

**2-color schemes:**

* ``complementary`` — 180° separation
* ``golden`` — 137.5° separation (golden angle)

**3-color schemes:**

* ``analogous`` — 30°, 30° separations
* ``triadic`` — 120°, 120° separations
* ``split_complementary`` — 150°, 60° separations
* ``split_analogous`` — 60°, 30° separations

**4-color schemes:**

* ``tetradic`` — 60°, 120°, 60° separations
* ``square`` — 90°, 90°, 90° separations
* ``analogous_wide`` — 20°, 20°, 20° separations
* ``compound`` — 30°, 150°, 30° separations
* ``double_complementary`` — 60°, 120°, 60° separations

**Multi-color palettes:**

The harmony module supports palettes of any size. For palettes with more than 5 colors, the algorithm automatically detects natural gaps and splits the palette into multiple groups for independent analysis.

Assessment
----------

``assess()`` analyzes a palette and returns an ``assessment_result``:

.. code-block:: cpp

  struct assessment_result {
    float score;            // 0-100, higher is better
    harmony_scheme scheme;  // Detected harmony scheme
    float deviation;        // Average deviation from ideal angles
  };

The score is calculated based on how closely the palette's hue differences match the ideal angles for the detected scheme.

Weighted Assessment
-------------------

``assess_weighted()`` provides advanced harmony evaluation with configurable channel weights:

.. code-block:: cpp

  struct harmony_weights {
    float angle_weight = 1.0f;       // Weight for angle/hue harmony (0.0-1.0)
    float saturation_weight = 0.5f;  // Weight for saturation uniformity (0.0-1.0)
    float lightness_weight = 0.3f;   // Weight for lightness uniformity (0.0-1.0)

    // Preset factory methods
    static harmony_weights angle_only();
    static harmony_weights balanced();
    static harmony_weights default_weights();
  };

  struct weighted_assessment_result {
    float score = 0.0f;             // Weighted harmony score (0-100)
    float angle_score = 0.0f;       // Angle/hue harmony component score
    float saturation_score = 0.0f;  // Saturation uniformity component score
    float lightness_score = 0.0f;   // Lightness uniformity component score
    harmony_scheme scheme = harmony_scheme::custom;
  };

Usage example:

.. code-block:: cpp

  // Use balanced equal weights for all channels
  auto result = harmony::assess_weighted(palette, harmony_weights::balanced());

  std::cout << "Total score: " << result.score << "\n";
  std::cout << "Hue score: " << result.angle_score << "\n";
  std::cout << "Saturation uniformity: " << result.saturation_score << "\n";
  std::cout << "Lightness uniformity: " << result.lightness_score << "\n";

Correction
----------

``correct()`` adjusts colors to match a target harmony scheme and returns a ``correction_result``:

.. code-block:: cpp

  // Auto-detect and correct
  auto corrected = harmony::correct(palette);
  
  // Correct to specific scheme
  auto corrected = harmony::correct(palette, harmony_scheme::triadic);

  // Correct only when deviation exceeds tolerance threshold (in degrees)
  auto corrected = harmony::correct(palette, harmony_scheme::triadic, 10.0f);

  // Minimal correction: only adjust the single color with largest deviation
  auto corrected = harmony::correct_minimal(palette, harmony_scheme::triadic);

  // Hue only correction: adjust only hue channel, preserve saturation and lightness
  auto corrected = harmony::correct_hue_only(palette, harmony_scheme::triadic);

  if (corrected.applied()) {
    auto harmony_score = harmony::assess(corrected.palette);
  }

.. code-block:: cpp

  enum class correction_status {
    corrected,
    unchanged,
    insufficient_colors,
    unsupported_scheme,
    tolerance_exceeded,
  };

  template <typename Color>
  struct correction_result {
    core::palette_set<Color> palette;
    harmony_scheme detected_scheme;
    harmony_scheme target_scheme;
    correction_status status;

    bool applied() const;
  };

The function preserves the first color's hue and adjusts subsequent colors to match the ideal angles for the target scheme.
If the input palette is too small or the resolved target scheme has no correction rule, the original palette is returned alongside a non-corrected status.

When using tolerance threshold mode:
- Deviations below the tolerance leave the palette unchanged
- Deviations above the tolerance are corrected fully
- Returns ``tolerance_exceeded`` status when correction was applied

Suggestions
-----------

``suggest()`` provides actionable suggestions for improving harmony:

.. code-block:: cpp

  auto suggestions = harmony::suggest(palette);
  for (const auto& s : suggestions) {
    // s.color_index — which color to adjust
    // s.action — description of the adjustment
    // s.hue_adjustment — recommended hue shift in degrees
  }

Notes
-----

* Hue analysis is performed in HSL space
* The ``golden`` scheme (137.5°) is based on the golden angle, which provides visually pleasing spacing
* Detection uses a 15° tolerance when matching against ideal angles
* For palettes with no recognized scheme, a base penalty is applied to the deviation
* All correction functions preserve saturation and lightness by default
* Multi-group detection automatically handles palettes with 6 or more colors

References
----------

* Color harmony theory: https://en.wikipedia.org/wiki/Color_harmony
* Golden angle: https://en.wikipedia.org/wiki/Golden_angle
* Color wheel relationships: https://www.sessions.edu/color-calculator/
