// adsorption_air.C -- Air "adsorption" by Henry's law.
// 
// Copyright 2025 UCPH
//
// This file is part of Daisy.
// 
// Daisy is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// Daisy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
// 
// You should have received a copy of the GNU Lesser Public License
// along with Daisy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#define BUILD_DLL
#include "daisy/chemicals/adsorption.h"
#include "object_model/block_model.h"
#include "object_model/check.h"
#include "daisy/soil/soil.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <cmath>

struct AdsorptionAir : public Adsorption
{
  // Parameters.
  const double K_wa;		// Water-air partitioning coefficient []
  const double T_ref;		// Reference temperature for K_wa [dg C]
  const double B;		// Temperature dependency [K]

  // Simulation.
  double find_Hs (const double T /* [dg C] */) const
  { 
    // Adjust K_wa for temperature.
    const double T_abs = T + 273.15; // [K]
    const double T_ref_abs = T_ref + 273.15; // [K]

    // Ref https://doi.org/10.5194/acp-23-10901-2023 eq 5.
    return K_wa * std::exp (B * (1.0 / T_abs - 1.0 / T_ref_abs)); // [];
  }

  double C_to_M (const Soil& soil, double Theta, double T, int i, 
                 double Cw, double) const
  {
    const double air = soil.Theta_sat (i) - Theta; // []

    // Saturated soil.
    if (air <= 0.0)
      return Theta * Cw;

    const double Hs = find_Hs (T); // []

    // Hs = Ca / Cw => Ca = Hs Cw
    // M = Ca air + Cw Theta => M = Hs Cw air + Cw Theta
    return Hs * Cw * air + Cw * Theta;
  }
  double M_to_C (const Soil& soil, double Theta, double T, int i, 
                 double M, double) const
  {
    const double air = soil.Theta_sat (i) - Theta;

    // Saturated soil.
    if (air <= 0.0)
      return M / Theta;

    const double Hs = find_Hs (T); // []
    // M = Hs Cw air + Cw Theta
    // => Cw = M / (Hs * air + Theta)
    return M / (Hs * air + Theta);
  }

  // Create.
public:
  AdsorptionAir (const BlockModel& al)
    : Adsorption (al),
      K_wa (al.number ("K_wa")),
      T_ref (al.number ("T_ref")),
      B (al.number ("B"))
  { }
};

static struct AdsorptionAirSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  {
    return new AdsorptionAir (al);
  }

  static bool check_alist (const Metalib&, const Frame& al, Treelog& err)
  {
    bool ok = true;
    return ok;
  }
  AdsorptionAirSyntax ()
    : DeclareModel (Adsorption::component, "air", "\
Henry's law, fixed ratio between concentration in air and water.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "henry2023");
    frame.add_check (check_alist);
    frame.declare ("K_wa", Attribute::None (), Check::positive (),
		   Attribute::Const,
		   "Ratio between concentration in air and in water.\n\
K_wa = Ca / Cw\n\
where Ca is mass per volume air, and Cw is mass per volume water.");
    frame.declare ("T_ref", "dg C", Check::none (), Attribute::Const, 
		   "Temperature at which K_aw is valid.");
    frame.set ("T_ref", 25.0);
    frame.declare ("B", "K", Check::positive (), Attribute::Const, 
		   "Temperature effect coefficient.");

  }
} AdsorptionAir_syntax;

// adsorption_air.C ends here.
