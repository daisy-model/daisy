// daisy_api.C -- Implementation of DaisyAPI non-BMI extension methods.

#include "programs/daisy_api.h"
#include <stdexcept>

auto DaisyAPI::perturbation_tick (double dh_cm, unsigned int col)
  -> std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
{
  if (col != 0u)
    throw std::invalid_argument (
      "perturbation_tick: col > 0 not yet supported");
  return ctrl ().perturbation_tick (dh_cm);
}
