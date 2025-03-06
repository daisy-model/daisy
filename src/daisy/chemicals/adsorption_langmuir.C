// adsorption_langmuir.C
// 
// Copyright 1996-2001 Per Abrahamsen and SÃ¸ren Hansen
// Copyright 2000-2001 KVL.
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
#include "daisy/soil/soil.h"
#include "object_model/check.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"

static const double c_fraction_in_humus = 0.587;

class AdsorptionLangmuir : public Adsorption
{
  // Parameters.
  const double B;		// = [1/K]
  const double my_max_clay;
  const double my_max_OC;

  // Simulation.
public:
  double C_to_M (const Soil& soil, const double Theta, const double T,
		 const int i, const double C, double sf) const
    {
      const double my_max 
	= my_max_clay * soil.clay (i)
	+ my_max_OC * c_fraction_in_humus * soil.humus (i);
      const double S = (my_max * C) / (B + C);
      return sf * soil.dry_bulk_density (i) * S + Theta * C;
    }
  double M_to_C (const Soil& soil, const double Theta, const double T,
		 const int i, const double M, double sf) const
    {
      // We need to solve the following equation w.r.t. C.
      //
      //     M = rho (my_max C) / (B + C) + Theta C
      // ==>
      //     M (B + C) = rho my_max C + Theta C (B + C)
      // ==> 
      //     0 = Theta C^2 + (rho my_max + Theta B - M) C - M B
      //
      // So we get a square equation.  We use the positive solution.
      
      const double my_max 
	= my_max_clay * soil.clay (i)
	+ my_max_OC * c_fraction_in_humus * soil.humus (i);

      const double a = Theta;
      const double b = sf * soil.dry_bulk_density (i) * my_max + Theta * B - M;
      const double c = - M * B;

      return single_positive_root_of_square_equation (a, b, c);
    }
  // Create.
public:
  AdsorptionLangmuir (const BlockModel& al)
    : Adsorption (al),
      B (1.0/al.number ("K")),
      my_max_clay (al.number ("my_max_clay", 0.0)),
      my_max_OC (al.check ("my_max_OC") 
		    ? al.number ("my_max_OC") 
		    : al.number ("my_max_clay"))
    { }
};

static struct AdsorptionLangmuirSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new AdsorptionLangmuir (al); }
  static bool check_alist (const Metalib&, const Frame& al, Treelog& err)
  {
    bool ok = true;

    const bool has_my_max_clay = al.check ("my_max_clay");
    const bool has_my_max_OC = al.check ("my_max_OC");

    if (!has_my_max_clay && !has_my_max_OC)
      {
        err.entry ("You must specify either 'my_max_clay' or 'my_max_OC'");
        ok = false;
      }
    return ok;
  }
  AdsorptionLangmuirSyntax ()
    : DeclareModel (Adsorption::component, "Langmuir", "\
M = rho (my_max C) / ((1/K) + C) + Theta C")
  { }
  void load_frame (Frame& frame) const
  {
    frame.add_check (check_alist);
    frame.declare ("K", "cm^3/g", Check::positive (), Attribute::Const,
		   "Slope parameter.");
    frame.declare ("my_max_clay", "g/g", Check::non_negative (), 
		   Attribute::OptionalConst,
		   "Max adsorption capacity (clay).\n\
It is multiplied with the soil clay fraction to get the clay part of\n\
'my_max'.  If 'my_max_OC' is specified, 'my_max_clay' defaults to 0.");
    frame.declare ("my_max_OC", "g/g", Check::non_negative (), 
		Attribute::OptionalConst,
		"Max adsorption capacity (humus).\n\
It is multiplied with the soil organic carbon fraction to get the\n\
carbon part of 'my_max'.  By default, 'my_max_OC' is equal to 'my_max_clay'.");
  }
} AdsorptionLangmuir_syntax;

// adsorption_langmuir.C ends here.
