// hydraulic_B_BaC.C
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
// Brooks and Corey retention curve model with Burdine theory.

#define BUILD_DLL

#include "daisy/soil/hydraulic.h"
#include "object_model/block_model.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"
#include "object_model/check.h"

class HydraulicB_BaC : public Hydraulic
{
  // Content.
  const double lambda;
  const double h_b;
  const double p;		// Used by 'K'
  const double a;		// Used by 'M'

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
  HydraulicB_BaC (const BlockModel&);
  ~HydraulicB_BaC ();
};

double 
HydraulicB_BaC::Theta (const double h) const
{
  return Se (h) * (Theta_sat - Theta_res) + Theta_res;
}

double 
HydraulicB_BaC::K (const double h) const
{
  return K_sat * pow (Se (h), p);
}

double 
HydraulicB_BaC::Cw2 (const double h) const
{
  if (h < h_b)
    return (Theta_sat - Theta_res)
      * lambda * pow (h_b / h, lambda + 1) / -h_b;
  else
    return 0.0;
}

double 
HydraulicB_BaC::h (const double Theta) const
{
  if (Theta < Theta_sat)
    return h_b / pow((Theta_res - Theta) / (Theta_res - Theta_sat), 1.0 / lambda);
  else
    return h_b;
}

double 
HydraulicB_BaC::M (double h) const
{
  if (h <= h_b)
    return (h_b * K_sat / (a + 1.0)) * std::pow (h_b/h, -a - 1.0);
  else
    return M (h_b) + K_sat * (h - h_b);
}

double 
HydraulicB_BaC::Se (double h) const
{
  if (h < h_b)
    return pow (h_b / h, lambda);
  else
    return 1;
}

HydraulicB_BaC::HydraulicB_BaC (const BlockModel& al)
  : Hydraulic (al),
    lambda (al.number ("lambda")),
    h_b (al.number ("h_b")),
    p (al.number ("p", al.number ("l")+1+2.0/lambda)),
    a (-lambda * p)
{ }

HydraulicB_BaC::~HydraulicB_BaC ()
{ }

// Add the HydraulicB_BaC syntax to the syntax table.

static struct HydraulicB_BaCSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const 
  { return new HydraulicB_BaC (al); }

  HydraulicB_BaCSyntax ()
    : DeclareModel (Hydraulic::component, "B_BaC",
               "Brooks and Corey retention curve model with Burdine theory.")
  { }
   static bool check_alist (const Metalib&, const Frame& al, Treelog& msg)
  { 
    bool ok = true;
    const double lambda = al.number ("lambda");
    const double p = al.number ("p", al.number ("l")+1+2.0/lambda);
    const double a = -lambda * p;
    if (p <= 0)
      {
	msg.error ("l must be larger than -1 - 2/lambda");
	ok = false;
      }
    if (!std::isnormal (a))
      {
	msg.error ("-lambda * p must not be zero");
	ok = false;
      }
    return ok;
  }
 void load_frame (Frame& frame) const
  { 
    Hydraulic::load_Theta_res (frame);
    Hydraulic::load_K_sat (frame);
    frame.declare ("lambda", Attribute::None (), Check::non_zero (), 
		   Attribute::Const,
                "Pore size index.");
    frame.declare ("h_b", "cm", Check::negative (), Attribute::Const,
                "Bubbling pressure.");
    frame.declare ("l", Attribute::None (), Check::none (), Attribute::Const,
		   "Burdine form parameter. Ignored if 'p' is set.");
    frame.set ("l", 2.0);
    frame.declare ("p", Attribute::None (), Check::positive (),
		   Attribute::OptionalConst, "\
Hydraulic conductivity form parameter. By default p=l+1+2/lambda.");
  }
} hydraulicB_BaC_syntax;

// hydraulic_B_BaC.C ends here.
