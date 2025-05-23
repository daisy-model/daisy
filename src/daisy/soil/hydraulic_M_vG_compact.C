// hydraulic_M_vG_compact.C
// 
// Copyright 1996-2001 Per Abrahamsen and Søren Hansen
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
//
// van Genuchten retention curve model with Mualem theory and compaction.

#define BUILD_DLL

#include "daisy/soil/hydraulic.h"
#include "object_model/block_model.h"
#include "object_model/plf.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

class HydraulicM_vG_compact : public Hydraulic
{
  // Reference values.
  const double ref_alpha;
  const double ref_n;
  const double ref_K_sat;
  
  // Modifiers.
  const PLF mod_alpha;
  const PLF mod_n;
  const PLF mod_K_sat;

  // Actual values depending on porosity.
  double alpha;
  double a;		// - alpha
  double n;
  double m;		// 1 - 1/n
  void set_porosity (double Theta);
  mutable PLF M_;

  // Use.
public:
  double Theta (double h) const;
  double K (double h) const;
  double Cw2 (double h) const;
  double h (double Theta) const;
  double M (double h) const;
private:
  double Se (double h) const;
  
  // Create and Destroy.
public:
  HydraulicM_vG_compact (const BlockModel&);
  ~HydraulicM_vG_compact ();
};

void
HydraulicM_vG_compact::set_porosity (double Theta)
{
  Hydraulic::set_porosity (Theta);
  alpha = ref_alpha * mod_alpha (Theta);
  a = -alpha;
  n = ref_n * mod_n (Theta);
  m = 1 - 1 / n;
  K_sat = ref_K_sat * mod_K_sat (Theta);
}


double 
HydraulicM_vG_compact::Theta (const double h) const
{
  return Se (h) * (Theta_sat - Theta_res) + Theta_res;
}

double 
HydraulicM_vG_compact::K (const double h) const
{
  if (h < 0.0)
    {
      const double Se_h = Se (h);
      return K_sat * sqrt (Se_h)
	* pow (1.0 - pow (1.0 - pow (Se_h, 1.0/m), m), 2.0);
    }
  else
    return K_sat;
}

double 
HydraulicM_vG_compact::Cw2 (const double h) const
{
  if (h < 0.0)
    return - (  (Theta_sat - Theta_res)
	      * (m * (  pow (1.0 / (1.0 + pow (a * h, n)), m - 1.0)
		      * (n * (pow (a * h, n - 1.0) * a))))
	      / pow (1.0 + pow(a * h, n), 2.0));
  else
    return 0.0;
}

double 
HydraulicM_vG_compact::h (const double Theta) const
{
  daisy_assert (Theta_res <= Theta);
  if (Theta < Theta_sat)
    return pow(pow(Theta_res / (Theta_res - Theta_sat) 
		   + Theta / (Theta_sat - Theta_res), -1.0 / m)
	       - 1.0, 1.0 / n) / a;
  else
    return 0.0;
}

double 
HydraulicM_vG_compact::M (double h) const
{
  if (M_.size () == 0)
    K_to_M (M_, 500);

  return M_ (h);
}

double 
HydraulicM_vG_compact::Se (double h) const
{
  if (h < 0.0)
    {
      const double Se_h = pow (1.0 / (1.0 + pow (a * h, n)), m);
      daisy_assert (Se_h >= 0.0);
      daisy_assert (Se_h <= 1.0);
      return Se_h;
    }
  else
    return 1.0;
}

HydraulicM_vG_compact::HydraulicM_vG_compact (const BlockModel& al)
  : Hydraulic (al),
    ref_alpha (al.number ("ref_alpha")),
    ref_n (al.number ("ref_n")),
    ref_K_sat (al.number ("ref_K_sat")),
    mod_alpha (al.plf ("mod_alpha")),
    mod_n (al.plf ("mod_n")),
    mod_K_sat (al.plf ("mod_K_sat")),
    M_ ()
{ set_porosity (Theta_sat); }

HydraulicM_vG_compact::~HydraulicM_vG_compact ()
{ }

// Register the HydraulicM_vG_compact syntax.
static struct HydraulicM_vG_compactSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new HydraulicM_vG_compact (al); }

  HydraulicM_vG_compactSyntax ()
    : DeclareModel (Hydraulic::component, "M_vG_compact", 
	       "van Genuchten retention curve model with Mualem theory\n\
and compaction.")
  { }
  void load_frame (Frame& frame) const
  { 
    Hydraulic::load_Theta_res (frame);
    frame.declare ("ref_alpha", "cm^-1", Attribute::Const,
		"Reference van Genuchten alpha.");
    frame.declare ("ref_n", Attribute::None (), Attribute::Const,
		"Reference van Genuchten n.");
    frame.declare ("ref_K_sat", "cm/h", Attribute::Const,
		"Reference water conductivity of saturated soil.");
    frame.declare ("mod_alpha", Attribute::Fraction (), Attribute::None (), 
		Attribute::Const,
		"Porosity modifier for van Genuchten alpha.");
    frame.declare ("mod_n", Attribute::Fraction (), Attribute::None (), Attribute::Const,
		"Porosity modifier for van Genuchten n.");
    frame.declare ("mod_K_sat", Attribute::Fraction (), Attribute::None (),
		Attribute::Const,
		"Porosity modifier for water conductivity of saturated soil.");

  }
} hydraulicM_vG_compact_syntax;

// hydraulic:M_vG_compact.C ends here.
