// tortuosity_linear.C --- Linear Impedance factor
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

#define BUILD_DLL

#include "daisy/soil/tortuosity.h"
#include "object_model/block_model.h"
#include "daisy/soil/hydraulic.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

static const double h_wp = -15000;

class TortuosityLinear : public Tortuosity
{
  const double a_maybe;
  const double b;

  // Simulation.
public:
  double factor (const Hydraulic& hydraulic, double Theta) const
    {
      double a = a_maybe;
      if (a < 0.0)
	a = hydraulic.Theta (h_wp);
      
      return std::max (1.0e-6, a + b * Theta);
    }

  // Create.
public:
  TortuosityLinear (const BlockModel& al)
    : Tortuosity (al),
      a_maybe (al.number ("a", -42.0)),
      b (al.number ("b"))
    { }
};

static struct TortuosityLinearSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  {
    return new TortuosityLinear (al);
  }

  TortuosityLinearSyntax ()
    : DeclareModel (Tortuosity::component, "linear", "Linear Impedance factor.  a + b Theta.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set_strings ("cite", "nye1977solute");
    frame.declare ("a", "cm^3/cm^3", Attribute::OptionalConst, "\
Theta offset.  By default, this corresponds to the wilting point.");
    frame.declare ("b", Attribute::None (), Attribute::Const, "Theta factor.");
    frame.set ("b", 2.0);
  }
} TortuosityLinear_syntax;

