Colour appearance models (CAM16, CAM16-UCS, HCT)
=================================================

CAM16 (Li et al. 2017) is a colour appearance model: it predicts how a colour
looks under a specific viewing condition — lightness, chroma, hue, brightness,
colourfulness, saturation and hue quadrature — rather than just its
tristimulus values. colorcpp implements the CAM16 forward and inverse
transforms with precomputed viewing conditions, the CAM16-UCS uniform space
(J'a'b' coordinates and delta E), and HCT, the Material You hue/chroma/tone
space built on CAM16.

In colorcpp
-----------

* Headers: ``include/colorcpp/algorithms/appearance/cam16.hpp``,
  ``cam16_ucs.hpp`` and ``hct.hpp``, aggregated by
  ``include/colorcpp/algorithms/appearance.hpp``
* Namespace: ``colorcpp::algorithms::appearance``

Scale convention
~~~~~~~~~~~~~~~~

This library's XYZ is relative with Y = 1 at the white, while the published
CAM16 equations use the Y = 100 scale. The module scales XYZ and Y_b by 100 at
its boundary; L_A is absolute (cd/m^2) and passes through unchanged. All public
inputs and outputs use this library's relative XYZ scale, results are returned
as single-precision floats with double-precision internal math, and viewing
conditions are built once per scene and reused across colours.

CAM16 — viewing conditions, spec, forward and inverse
-----------------------------------------------------

Surround
~~~~~~~~

``enum class surround { average, dim, dark }`` — CIE 159:2004 induction
category selecting the induction factors F, c and N_c (average: 1.0/0.69/1.0,
dim: 0.9/0.59/0.9, dark: 0.8/0.525/0.8).

Viewing conditions
~~~~~~~~~~~~~~~~~~

``struct viewing_conditions`` holds every precomputed term of the model
(F, c, N_c, D, F_L, n, z, N_bb, N_cb, A_w and the adapted cone responses of the
white). Build once per scene via ``create`` and reuse across colours::

    viewing_conditions vc = viewing_conditions::create(white, La, Yb, surround::average);

* ``white`` — reference white, library Y = 1 scale (e.g. D65)
* ``La`` — adapted luminance in cd/m^2 (absolute; colour-science uses 318.31)
* ``Yb`` — background luminance factor, 0-1 (colour-science uses 0.2)
* ``s`` — surround category, default ``surround::average``
Members:

+-------------+-------------------------------------------------------------+
| Member      | Meaning                                                     |
+=============+=============================================================+
| F           | Induction factor                                            |
+-------------+-------------------------------------------------------------+
| c           | Exponent                                                    |
+-------------+-------------------------------------------------------------+
| N_c         | Chromatic induction                                         |
+-------------+-------------------------------------------------------------+
| D           | Degree of adaptation, clamped to [0, 1]                     |
+-------------+-------------------------------------------------------------+
| F_L         | Luminance-level adaptation factor                           |
+-------------+-------------------------------------------------------------+
| n           | Background-to-white luminance ratio (Y = 100 scale)         |
+-------------+-------------------------------------------------------------+
| z           | 1.48 + sqrt(n)                                              |
+-------------+-------------------------------------------------------------+
| N_bb        | Brightness induction                                        |
+-------------+-------------------------------------------------------------+
| N_cb        | Chromatic induction of the background                       |
+-------------+-------------------------------------------------------------+
| Y_w         | White luminance on the Y = 100 scale                        |
+-------------+-------------------------------------------------------------+
| RGB_w       | M_CAT16 * XYZ_w (Y = 100 scale)                             |
+-------------+-------------------------------------------------------------+
| D_RGB       | Per-cone factor D * Y_w / RGB_w + 1 - D                     |
+-------------+-------------------------------------------------------------+
| A_w         | Achromatic response of the adapted white                    |
+-------------+-------------------------------------------------------------+

CAM16 specification
~~~~~~~~~~~~~~~~~~~

``struct cam16_spec`` — the full set of appearance correlates:

+-----------+--------------------------------------+
| Member    | Meaning                              |
+===========+======================================+
| J         | Lightness (0-100)                    |
+-----------+--------------------------------------+
| C         | Chroma                               |
+-----------+--------------------------------------+
| h         | Hue angle, degrees 0-360             |
+-----------+--------------------------------------+
| s         | Saturation                           |
+-----------+--------------------------------------+
| Q         | Brightness                           |
+-----------+--------------------------------------+
| M         | Colourfulness                        |
+-----------+--------------------------------------+
| H         | Hue quadrature, 0-400                |
+-----------+--------------------------------------+

``float cam16_hue_quadrature(float h)``
    CAM16 hue quadrature H(h) on the 0-400 scale, including the wrap region
    (H(0) = H(360-) ≈ 385.9, H(90) = 100, H(164.25) = 200,
    H(237.53) = 300).

``cam16_spec xyz_to_cam16(const core::xyz_t& xyz, const viewing_conditions& vc)``
    Forward CAM16 transform: relative XYZ to appearance correlates. The hue
    result is normalized to [0, 360).

``core::xyz_t cam16_to_xyz(float J, float C, float h, const viewing_conditions& vc)``
    Inverse CAM16 transform: appearance correlates back to relative XYZ. With
    t = 0 the result collapses to the achromatic axis; the |sin h| vs |cos h|
    branch keeps the reciprocal away from zero at cardinal hues
    (h = 0/90/180/270).
CAM16-UCS — uniform J'a'b' coordinates and delta E
--------------------------------------------------

CAM16-UCS (Luo 2006 coefficients as adopted by Li et al. 2017) maps
``(J, M, h)`` — colourfulness, not chroma — to a nearly uniform space::

    J' = (1 + 100 c1) J / (1 + c1 J)          c1 = 0.007
    M' = ln(1 + c2 M) / c2                    c2 = 0.0228
    a' = M' cos h,  b' = M' sin h

delta E is the plain Euclidean distance in J'a'b'.

``struct cam16_ucs { float Jp, ap, bp; }``
    CAM16-UCS coordinates.
``struct cam16_jmh { float J, M, h; }``
    CAM16 correlates feeding the UCS transform.

``cam16_ucs cam16_to_ucs(const cam16_spec& spec)``
    Forward transform. Uses spec.M (colourfulness); chroma C is not the input.

``cam16_jmh ucs_to_cam16_jmh(const cam16_ucs& c)``
    Inverse transform. The achromatic origin maps to M = 0 with h = 0
    (atan2(0, 0) = 0).

``float delta_e_cam16_ucs(const cam16_ucs& a, const cam16_ucs& b)``
    Euclidean colour difference in CAM16-UCS. Its JND scale is not the
    ΔE2000 scale (≈ 1.0); do not reuse ΔE2000 thresholds.

HCT — Material You hue/chroma/tone
----------------------------------

HCT separates what the eye separates: hue and chroma come from CAM16 under
Material's canonical viewing conditions, tone is the CIE L* lightness of the
luminance. The inverse is an independent Newton iteration on J (the Y(J)
relation is locally quadratic), with a chroma bisection when the requested
chroma leaves the sRGB gamut at that hue and tone — hue and tone are preserved
and chroma is maximized, matching Material's contract.

``struct hct { float hue, chroma, tone; }``
    HCT coordinate triple: hue in degrees, CAM16 chroma, CIE L* tone.

``float lstar_from_y(float y)`` / ``float y_from_lstar(float lstar)``
    CIE L* from relative luminance Y (0-1) and back, exact k = 24389/27
    breakpoint at L* = 8.

``const viewing_conditions& hct_viewing_conditions()``
    Material's canonical conditions: D65 white (0.95047, 1, 1.08883),
    L_A = 11.725676537 cd/m^2, background L* = 50 (Y_b = 0.184186503),
    average surround.

``hct xyz_to_hct(const core::xyz_t& xyz)``
    Forward: CAM16 hue and chroma, CIE L* tone of the luminance.

``core::xyz_t hct_to_xyz(const hct& c)``
    Inverse, limited to sRGB. Tone 0 is black and tone 100 is white; chroma
    below 1e-4 is the exact achromatic axis (shortcut to the sRGB gray);
    otherwise Newton solves J for the target luminance with chroma bisection
    for out-of-gamut requests.

``template <typename Color> core::palette_set<Color> hct_tonal_palette(float hue, float chroma, std::size_t count)``
    Material tonal palette: count tones evenly spaced over L* 0..100; tone 0 is
    black, tone 100 is white, count == 1 gives tone 50.

Notes
-----

* Viewing conditions must be created per scene (white, L_A, Y_b, surround) and
  passed identically to the forward and inverse transforms; round-trip
  accuracy depends on matching conditions.
* The inverse transform's singularity guards matter at cardinal hues — the
  |sin h| vs |cos h| branch is selected by whichever component is larger in
  magnitude.
* HCT clamps chroma to the sRGB gamut at fixed hue and tone rather than
  drifting hue.
* CAM16-UCS delta E and CIEDE2000 live on different scales and are not
  interchangeable; never reuse ΔE2000 thresholds for CAM16-UCS.

References
----------

* Li, Li, Wang, Zu, Luo, Melgosa, Brill, Pointer (2017). *Comprehensive
  colour solutions: CAM16, CAT16 and CAM16-UCS*. Color Research & Application
  42(6).
* Luo, M. R. (2006). CAM02-UCS coefficients, adopted by CAM16-UCS (Li et al.
  2017).
* CIE 159:2004 — induction factors shared by CIECAM02 and CAM16.
* `colour-science <https://github.com/colour-science/colour>`__ (BSD-3-Clause)
  — reference vectors the unit tests pin against.
* Google `material-color-utilities
  <https://github.com/material-material/material-color-utilities>`__
  (Apache-2.0) — HCT viewing-condition constants (numeric values only).
