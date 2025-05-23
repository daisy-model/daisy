// hydraulic_M_C.C -- Campbell retention curve model with Mualem theory.
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
// Campbell retention curve model with Mualem theory.

#define BUILD_DLL

#include "daisy/soil/hydraulic.h"
#include "object_model/block_model.h"
#include "object_model/check.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

class HydraulicM_C : public Hydraulic
{
  // Content.
  const double h_b;
  const double b;
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
  double Sr (double h) const;
  
  // Create and Destroy.
public:
  HydraulicM_C (const BlockModel&);
  ~HydraulicM_C ();
};

double 
HydraulicM_C::Theta (const double h) const
{
  return Sr (h) * Theta_sat;
}

double 
HydraulicM_C::K (const double h) const
{
  return K_sat * pow (Sr (h), p);
}

double 
HydraulicM_C::Cw2 (const double h) const
{
  if (h < h_b)
    return -(Theta_sat*(pow(h_b / h, 1.0 / b - 1.0)*h_b) / (pow(h, 2.0)*b));
  else
    return 0.0;
}

double 
HydraulicM_C::h (const double Theta) const
{
  if (Theta < Theta_sat)
    return h_b / pow(Theta / Theta_sat, b);
  else
    return h_b;
}

double 
HydraulicM_C::M (double h) const
{
  if (h <= h_b)
    return (h_b * K_sat / (a + 1.0)) * std::pow (h_b/h, -a - 1.0);
  else
    return M (h_b) + K_sat * (h - h_b);
}

double 
HydraulicM_C::Sr (double h) const
{
  if (h < h_b)
    return pow (h_b / h, 1.0 / b);
  else
    return 1;
}

HydraulicM_C::HydraulicM_C (const BlockModel& al)
  : Hydraulic (al),
    h_b (al.number ("h_b")),
    b (al.number ("b")),
    p (al.number ("p", al.number ("l")+2+2*b)),
    a (-p/b)
{ }

HydraulicM_C::~HydraulicM_C ()
{ }

// Add the HydraulicM_C syntax to the syntax table.

static struct HydraulicM_CSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  {
    return new HydraulicM_C (al);
  }

  HydraulicM_CSyntax ()
    : DeclareModel (Hydraulic::component, "M_C", 
	       "Campbell retention curve model with Mualem theory.")
  { }
  static bool check_alist (const Metalib&, const Frame& al, Treelog& msg)
  { 
    bool ok = true;
    const double b = al.number ("b");
    const double p = al.number ("p", al.number ("l")+2+2*b);
    const double a = -p/b;
    if (p <= 0.0)
      {
	msg.error ("l must be larger than -2 - 2b'");
	ok = false;
      }
    if (!std::isnormal (a))
      {
	msg.error ("-p/b must not be zero");
	ok = false;
      }
    return ok;
  }
  void load_frame (Frame& frame) const
  { 
    frame.add_check (check_alist);
    frame.set_strings ("cite", "campbell1974simple", "mualem1976");
    Hydraulic::load_Theta_sat (frame);
    Hydraulic::load_K_sat (frame);
    frame.declare ("h_b", "cm", Check::negative (), Attribute::Const,
		"Bubbling pressure.");
    frame.declare ("b", Attribute::None (), Check::positive (), 
                   Attribute::Const, "Campbell parameter.");
    frame.declare ("l", Attribute::None (), Check::none (), Attribute::Const,
		   "Mualem form parameter. Ignored if 'p' is set.");
    frame.set ("l", 0.5);
    frame.declare ("p", Attribute::None (), Check::positive (),
		   Attribute::OptionalConst, "\
Hydraulic conductivity form parameter. By default p=l+2+2*b.");
  }
} hydraulicM_C_syntax;

// hydraulic_M_C.C ends here.
