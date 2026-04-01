Color comparison
================

The compare module provides policy-based color comparison strategies.

Overview
--------

The compare module offers multiple comparison strategies:

* **exact**: Exact channel-by-channel equality (default)
* **epsilon**: Comparison within fixed epsilon tolerance
* **relative**: Comparison within relative tolerance
* **perceptual**: Comparison using ΔE2000 perceptual distance

Key features:

* **Policy-based design**: Use different strategies via template policies
* **No implicit color space conversion**: Compares colors in their native space
* **Constexpr support**: exact, epsilon, and relative are fully constexpr
* **Header-only**: No compilation required

Basic usage
-----------

.. code-block:: cpp

    #include <colorcpp/colorcpp.hpp>

    using namespace colorcpp;

    rgb8_t a{128, 64, 32};
    rgb8_t b{128, 64, 32};

    // Using operators (exact comparison)
    bool same = (a == b);
    bool different = (a != b);

    // Using compare functions
    bool same2 = compare::equal(a, b);
    bool same3 = compare::exact_equal(a, b);

Comparison policies
-------------------

Exact policy
~~~~~~~~~~~~

Exact channel-by-channel equality. This is the default strategy.

.. code-block:: cpp

    // Default comparison uses exact
    bool same = compare::equal(a, b);

    // Explicit exact comparison
    bool same2 = compare::exact_equal(a, b);

    // Or use policy directly
    bool same3 = compare::equal<compare::exact_policy>(a, b);

Epsilon policy
~~~~~~~~~~~~~~

Compares colors within a fixed epsilon tolerance.

.. code-block:: cpp

    rgbf_t a{0.5f, 0.3f, 0.7f};
    rgbf_t b{0.5001f, 0.3f, 0.7f};

    // Using convenience function
    bool close = compare::epsilon_equal(a, b, 0.001f);

    // Using policy
    bool close2 = compare::equal<compare::epsilon_policy>(a, b, 0.001f);

Relative policy
~~~~~~~~~~~~~~~

Compares colors within a relative tolerance. The tolerance is relative to the
larger of the two values being compared.

.. code-block:: cpp

    rgbf_t a{100.0f, 50.0f, 200.0f};
    rgbf_t b{100.1f, 50.0f, 200.0f};

    // Using convenience function
    bool similar = compare::relative_equal(a, b, 0.01f);

    // Using policy
    bool similar2 = compare::equal<compare::relative_policy>(a, b, 0.01f);

Perceptual policy
~~~~~~~~~~~~~~~~~

Compares colors using ΔE2000 perceptual distance. This is **not constexpr**
due to the complexity of the ΔE calculation.

.. code-block:: cpp

    rgb8_t a{128, 64, 32};
    rgb8_t b{130, 65, 33};

    // Using convenience function
    bool similar = compare::perceptual_equal(a, b, 5.0f);

    // Using policy
    bool similar2 = compare::equal<compare::perceptual_policy>(a, b, 5.0f);

Creating custom policies
------------------------

You can create custom comparison policies by defining a struct with a static
``compare`` method:

.. code-block:: cpp

    struct my_policy {
        template <typename Color>
        static constexpr bool compare(const Color& a, const Color& b) {
            // Custom comparison logic
            return true;
        }
    };

    // Use with equal
    bool result = compare::equal<my_policy>(a, b);

API reference
-------------

Functions
~~~~~~~~~

* ``equal(a, b)`` — Default exact comparison
* ``equal<Policy>(a, b, args...)`` — Comparison with specified policy
* ``exact_equal(a, b)`` — Exact comparison
* ``epsilon_equal(a, b, eps)`` — Epsilon comparison
* ``relative_equal(a, b, rel)`` — Relative comparison
* ``perceptual_equal(a, b, threshold)`` — Perceptual comparison

Policies
~~~~~~~~

* ``exact_policy`` — Exact equality
* ``epsilon_policy`` — Epsilon tolerance
* ``relative_policy`` — Relative tolerance
* ``perceptual_policy`` — ΔE2000 perceptual

See also
--------

* :doc:`delta_e` — Color difference metrics used by perceptual comparison
