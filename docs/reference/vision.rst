Color vision deficiency simulation
===================================

Simulations approximate how some viewers perceive colors under **protanopia**, **deuteranopia**, or **tritanopia** by working in **linear sRGB** and **LMS** space.

In colorcpp
------------

* Header: ``include/colorcpp/algorithms/vision.hpp``

Available simulations:

  * **Dichromacy** (full color deficiency):
    * ``simulate_protanopia()`` - L-cone absence (red blindness)
    * ``simulate_deuteranopia()`` - M-cone absence (green blindness)
    * ``simulate_tritanopia()`` - S-cone absence (blue blindness)
    * ``simulate_achromatopsia()`` - Total color blindness

  * **Machado 2009 Model** (variable severity):
    * ``simulate_protanopia_machado()`` with severity parameter [0.0, 1.0]
    * ``simulate_deuteranopia_machado()``
    * ``simulate_tritanopia_machado()``
    * Supports anomalous trichromacy to full dichromacy continuum

  * **Utility functions**:
    * ``is_distinguishable_for_cvd()``
    * ``color_difference_for_cvd()``


Notes
-----

* Brettel / Viénot model operates in LMS cone response space
* Machado model uses optimized RGB matrices for GPU-friendly implementation
* Severity 0.0 = normal vision, 1.0 = full dichromacy
* Simulations are design aids, not medical accurate representation of individual vision

References
----------

* Brettel, H., Viénot, F., & Mollon, J. D. (1997). *Computerized simulation of color appearance for dichromats.*
* Viénot, F., Brettel, H., & Mollon, J. D. (1999). *Digital video colourmaps for checking the legibility of displays by dichromats.*
* Machado, G. M., Oliveira, M. M., & Fernandes, L. A. (2009). *A Physiologically-Based Model for Simulation of Color Vision Deficiency.*

References
----------

* Brettel, H., Viénot, F., & Mollon, J. D. (1997). *Computerized simulation of color appearance for dichromats.* `DOI 10.1002/(SICI)1520-6378(199708)22:4<333::AID-COL6>3.0.CO;2-3 <https://doi.org/10.1002/(SICI)1520-6378(199708)22:4%3C333::AID-COL6%3E3.0.CO;2-3>`__
* Viénot, F., Brettel, H., & Mollon, J. D. (1999). *Digital video colourmaps for checking the legibility of displays by dichromats.* `DOI 10.1006/cvju.1998.0458 <https://doi.org/10.1006/cvju.1998.0458>`__
