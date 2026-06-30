// daisy_api.h -- Daisy-specific API extensions on top of the BMI layer.
//
// DaisyAPI inherits BMI (pure BMI 2.0) and adds methods that are
// specific to Daisy coupling but are not part of the BMI standard.
// daisy_api.h -- Daisy-specific extensions on top of the BMI interface.indings.cpp.
//
// Adding a new non-BMI method:
//   1. Declare it here.
//   2. Implement it in daisy_api.C (calling ctrl()).
//   3. Add a .def() in api_bindings.cpp.

#ifndef DAISY_API_H
#define DAISY_API_H

#include <tuple>
#include <vector>
#include "programs/bmi.h"

class DaisyAPI : public BMI
{
public:
  // ===== PERTURBATION / Sy ESTIMATION =====

  /**
   * Re-run Richards with the groundwater table raised by dh_cm and return
   * the perturbed state.  Daisy is always restored to the real post-tick
   * result via a RAII guard — even if Richards throws.
   *
   * The caller computes Sy externally:
   *   theta_B = bmi.get_value_array("soil_water__content")  # normal post-tick
   *   theta_C, flux_C, h_C = api.perturbation_tick(dh_cm)
   *   Sy = sum((theta_C[i] - theta_B[i]) * dz[i] for i) / dh_cm
   *
   * @param dh_cm  GW table perturbation in cm (default 1 cm, upward = positive)
   * @param col    Column index (default 0; multi-column not yet supported)
   * @return       tuple(theta_C [-], flux_mm_d [mm/day], h_C [cm])
   *               — all arrays have length == number of soil layers
   */
  std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
    perturbation_tick(double dh_cm = 1.0, unsigned int col = 0u);
};

#endif // DAISY_API_H
