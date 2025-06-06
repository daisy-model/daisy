// hydraulic_B_BaC_Bimodal.C
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
// Bimodal hydraulic conductivity curve.

#define BUILD_DLL

#include "daisy/soil/hydraulic.h"
#include "object_model/block_model.h"
#include "object_model/check.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

class HydraulicB_BaC_Bimodal : public Hydraulic
{
  // Content.
  const double lambda;
  const double h_b;
  const double Theta_b;
  const double K_b;

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
  HydraulicB_BaC_Bimodal (const BlockModel&);
  ~HydraulicB_BaC_Bimodal ();
};

double
HydraulicB_BaC_Bimodal::Theta (const double h) const
{
  if (h < h_b)
    return Se (h) * (Theta_b - Theta_res) + Theta_res;
  else if (h < 0.0)
    return (Theta_sat - Theta_b) / (-h_b) * (h - h_b) + Theta_b;
  else
    return Theta_sat;
}

double
HydraulicB_BaC_Bimodal::K (const double h) const
{
  if (h < h_b)
    return K_b * pow (Se (h), (2 + 3 * lambda) / lambda);
  else
    return (K_sat - K_b) / (-h_b) * (h - h_b) + K_b;
}

double
HydraulicB_BaC_Bimodal::Cw2 (const double h) const
{
  if (h < h_b)
    return (Theta_b - Theta_res)
      * lambda * pow (h_b / h, lambda + 1) / -h_b;
  else if (h < 0)
    return (Theta_sat - Theta_b) / (-h_b);
  else
    return 0.0;
}

double
HydraulicB_BaC_Bimodal::h (const double Theta) const
{
  daisy_assert (Theta > Theta_res);
  if (Theta < Theta_b)
    return h_b / pow((Theta - Theta_res) / (Theta_b - Theta_res), 1.0 / lambda);
  else if (Theta < Theta_sat)
    return h_b * ( 1 - (Theta - Theta_b) / (Theta_sat - Theta_b));
  else
    return 0;
}

double
HydraulicB_BaC_Bimodal::M (double h) const
{
  if (h <= h_b)
    return K_b * (-h_b / (1 + 3*lambda)) * pow (h_b / h, 1 + 3*lambda);
  else
    return M (h_b) + K_b * (h - h_b);
}

double
HydraulicB_BaC_Bimodal::Se (double h) const
{
  if (h < h_b)
    return pow (h_b / h, lambda);
  else
    return 1;
}

HydraulicB_BaC_Bimodal::HydraulicB_BaC_Bimodal (const BlockModel& al)
  : Hydraulic (al),
    lambda (al.number ("lambda")),
    h_b (al.number ("h_b")),
    Theta_b (al.number ("Theta_b")),
    K_b (al.number ("K_b"))
{ }

HydraulicB_BaC_Bimodal::~HydraulicB_BaC_Bimodal ()
{ }

// Add the HydraulicB_BaC_Bimodal syntax to the syntax table.

static struct HydraulicB_BaC_BimodalSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new HydraulicB_BaC_Bimodal (al); }

  HydraulicB_BaC_BimodalSyntax ()
    : DeclareModel (Hydraulic::component, "B_BaC_Bimodal", 
               "Brooks and Corey retention curve model with Burdine theory.\n\
Bimodal hydraulic conductivity curve.")
  { }
  void load_frame (Frame& frame) const
  {
    Hydraulic::load_Theta_res (frame);
    frame.declare ("K_sat", "cm/h", Check::non_negative (), Attribute::OptionalConst,
                "Water conductivity of saturated soil.");
    frame.declare ("lambda", Attribute::None (), Attribute::Const,
                "Pore size index.");
    frame.declare ("h_b", "cm", Check::negative (), Attribute::Const,
                "Bubbling pressure.");
    frame.declare ("Theta_b", Attribute::None (), Attribute::Const,
                "Water content at 'h_b'.");
    frame.declare ("K_b", "cm/h", Attribute::Const,
                "Water conductivity at 'h_b'.");

  }
} HydraulicB_BaC_Bimodal_syntax;
