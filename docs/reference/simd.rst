SIMD support
============

colorcpp currently treats SIMD as an **optional internal acceleration layer**, not as a separate public API family. Existing scalar APIs stay canonical; accelerated code paths are selected internally when enabled at build time and when the target ISA is supported.

In colorcpp
-----------

* Build option: ``COLORCPP_ENABLE_SIMD=ON``
* Internal capability probe: ``include/colorcpp/detail/simd.hpp``
* Current accelerated paths:
  ``include/colorcpp/operations/blend/simd.hpp`` and ``include/colorcpp/algorithms/delta_e/simd.hpp``

Current scope
-------------

Current SIMD-backed paths target ``colorcpp::operations::blend::blend()`` and ``colorcpp::algorithms::delta_e::delta_e_ok()``.

``blend()`` currently accelerates the following **separable** modes:

* ``normal``
* ``multiply``
* ``screen``
* ``overlay``
* ``darken``
* ``lighten``
* ``addition``
* ``subtraction``
* ``difference``
* ``exclusion``
* ``hard_light``

All other blend modes, including ``soft_light``, ``color_dodge``, ``color_burn``, ``divide``, and all non-separable modes, continue to use the scalar implementation.

``delta_e_ok()`` currently has an experimental SIMD-backed implementation for the Oklab-distance step after inputs have
been converted to ``oklab_t``. On compact three-float AoS storage, its benefit is workload- and compiler-dependent.

Notes
-----

* SIMD is **opt-in** and defaults to off so the current release behavior remains conservative.
* The present fast path is implemented for targets exposing **SSE2**. Unsupported targets keep the exact scalar code path.
* The library's core color storage is an array-of-structs layout, so SIMD work is most promising where four-channel float math already clusters naturally.
* The benchmark candidate target in ``benchmarks/benchmark_simd_candidates.cpp`` is intended to guide future expansion into batch-oriented conversion and selected vision/gamut helpers.

References
----------

* Intel, `Intrinsics Guide <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html>`__
* Arm, `NEON Programmer's Guide <https://developer.arm.com/Architectures/Neon>`__
* Agner Fog, `Optimizing software in C++ <https://www.agner.org/optimize/>`__
