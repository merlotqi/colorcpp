colorcpp documentation
======================

This directory holds **narrative reference** material (algorithms, standards, and links). API details for functions and types are generated separately with **Doxygen** from the headers.

.. toctree::
   :maxdepth: 2
   :caption: Contents

   reference/index

Building HTML (optional)
------------------------

Install Sphinx (see ``requirements-docs.txt``), then from the repository root::

   sphinx-build -b html docs docs/_build/html

Or configure CMake with ``-DCOLORCPP_BUILD_SPHINX=ON`` and build the ``sphinx`` target.

API reference (Doxygen)
-----------------------

Configure with ``-DCOLORCPP_BUILD_DOCS=ON``, build target ``doc``, and open the generated HTML under the build tree (see ``doxygen/Doxygen.in``).
