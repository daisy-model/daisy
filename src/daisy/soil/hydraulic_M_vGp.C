// hydraulic_M_vGp.C
// 
// Copyright 1996-2001, 2003 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001, 2003 KVL.
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
// van Genuchten retention curve model with Mualem theory and power function.

#define BUILD_DLL

#include "daisy/soil/hydraulic.h"
#include "object_model/block_model.h"
#include "object_model/plf.h"
#include "util/mathlib.h"
#include "object_model/check.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

class HydraulicM_vGp : public Hydraulic
{
  // Content.
  const double alpha;
  const double a;		// - alpha
  const double n;
  const double m;		// 1 - 1/n
  const double l;               // tortuosity parameter
  mutable PLF M_;
  // Power function.
  const double h_m;		// matrix/macro boundary.
  const double f;		// shape parameter.
  const double p_m_matrix;	// power function for matrix flow.

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
  HydraulicM_vGp (const BlockModel&);
  ~HydraulicM_vGp ();
};

double 
HydraulicM_vGp::Theta (const double h) const
{
  return Se (h) * (Theta_sat - Theta_res) + Theta_res;
}

double 
HydraulicM_vGp::K (const double h) const
{
  if (h < 0.0)
    {
      const double Se_h = Se (h);
      const double M_vG_K =  K_sat * pow (Se_h, l)
	* pow (1.0 - pow (1.0 - pow (Se_h, 1.0/m), m), 2.0);
      const double X = 1.0; 	// [cm^-1]
      const double p_m = (h > h_m)
	? pow (1.0 / (-h * X + 1.0), f)
	: p_m_matrix;
      return M_vG_K * p_m;
    }
  else
    return K_sat;
}

double 
HydraulicM_vGp::Cw2 (const double h) const
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
HydraulicM_vGp::h (const double Theta) const
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
HydraulicM_vGp::M (double h) const
{
  if (M_.size () == 0)
    K_to_M (M_, 500);

  return M_ (h);
}

double 
HydraulicM_vGp::Se (double h) const
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

HydraulicM_vGp::HydraulicM_vGp (const BlockModel& al)
  : Hydraulic (al),
    alpha (al.number ("alpha")),
    a (-alpha),
    n (al.number ("n")),
    m (1 - 1 / n),
    l (al.number ("l")),
    M_ (),
    h_m (al.number ("h_m")),
    f (al.number ("f")),
    p_m_matrix (pow (1.0 / (-h_m * 1.0 + 1.0), f))
{ }

HydraulicM_vGp::~HydraulicM_vGp ()
{ }

// Add the HydraulicM_vGp syntax to the syntax table.

static struct HydraulicM_vGpSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new HydraulicM_vGp (al); }

  HydraulicM_vGpSyntax ()
    : DeclareModel (Hydraulic::component, "M_vGp", 
               "van Genuchten retention curve model with Mualem theory.\n\
A p_m (h) function is multiplied to the conductivity to simulate the\n\
change near maropores.\n\
\n\
p_m = (1/(-h X + 1))^f; h > h_m\n\
p_m = (1/(-h_m X + 1))^f; h <= h_m\n\
X = 1 cm^-1")
  { }
  void load_frame (Frame& frame) const
  { 
    Hydraulic::load_Theta_res (frame);
    frame.set_strings ("cite", "MvGp");
    frame.declare ("K_sat", "cm/h", Check::non_negative (), Attribute::OptionalConst,
                "Water conductivity of saturated soil.");
    frame.declare ("alpha", "cm^-1", Attribute::Const,
                "van Genuchten alpha.");
    frame.declare ("n", Attribute::None (), Attribute::Const,
                "van Genuchten n.");
    frame.declare ("l", Attribute::None (), Attribute::Const,
                "tortuosity parameter.");
    frame.set ("l", 0.5);
    frame.declare ("h_m", "cm", Check::negative (), Attribute::Const,
                "Pressure point of chance between matrix and macropores.");
    frame.declare ("f", Attribute::None (), Check::non_negative (), Attribute::Const,
                "Macropores conductivity curve shape parameter.");

  }
} hydraulicM_vGp_syntax;

// hydraulic_M_vGp.C ends here.
