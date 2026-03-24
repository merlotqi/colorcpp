Color vision deficiency simulation
===================================

Simulations approximate how some viewers perceive colors under **protanopia**, **deuteranopia**, or **tritanopia** by working in **linear sRGB** and **LMS** space.

In colorcpp
------------

* Header: ``include/colorcpp/operations/vision.hpp``
* Entry points: ``simulate_protanopia``, ``simulate_deuteranopia``, ``simulate_tritanopia``, ``is_distinguishable``, etc.

Notes
-----

* Matrices cite **Viénot, Brettel, Mollon**-style formulations (see file header): linearization uses **IEC 61966-2-1** piecewise sRGB.
* Simulations are **educational / design aids**; they do not capture individual variation, acquired deficiencies, or display calibration.

References
----------

* Brettel, H., Viénot, F., & Mollon, J. D. (1997). *Computerized simulation of color appearance for dichromats.* `DOI 10.1002/(SICI)1520-6378(199708)22:4<333::AID-COL6>3.0.CO;2-3 <https://doi.org/10.1002/(SICI)1520-6378(199708)22:4%3C333::AID-COL6%3E3.0.CO;2-3>`__
* Viénot, F., Brettel, H., & Mollon, J. D. (1999). *Digital video colourmaps for checking the legibility of displays by dichromats.* `DOI 10.1006/cvju.1998.0458 <https://doi.org/10.1006/cvju.1998.0458>`__
