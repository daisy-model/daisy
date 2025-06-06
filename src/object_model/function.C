// function.C  -- Pure function of one parameter.
// 
// Copyright 2023 KU
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

#include "object_model/function.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/plf.h"

// The 'function' component.

const char *const Function::component = "function";

void
Function::plot_xy (std::vector<double>& x, std::vector<double>& y) const
{ }

Function::Function (const BlockModel&)
{ }

Function::~Function ()
{ }

static struct FunctionInit : public DeclareComponent 
{
  void load_frame (Frame& frame) const
  {
    Model::load_model (frame);
    frame.declare_string ("domain", Attribute::Const, "Function domain.");
    frame.set ("domain", Attribute::Unknown ());
    frame.declare_string ("range", Attribute::Const, "Function range.");
    frame.set ("range", Attribute::Unknown ());
    frame.declare_string ("formula", Attribute::OptionalConst, "\
LaTeX formula for the function, for the reference manual.");
  }
  FunctionInit ()
    : DeclareComponent (Function::component, "\
Pure function of one parameter.")
  { }
} Function_init;

// The 'plotable' base model.

void
FunctionPlotable::plot_xy (std::vector<double>& x, std::vector<double>& y) const
{
  const double min = x_min ();
  const double max = x_max ();
  const double step = (max - min) / 50.0;
  for (double p = min; p <= max; p += step)
    {
      x.push_back (p);
      y.push_back (this->value (p));
    }
}

FunctionPlotable::FunctionPlotable (const BlockModel& al)
  : Function (al)
{ }

FunctionPlotable::~FunctionPlotable ()
{ }

// The 'const' model.

struct FunctionConst : public Function
{
  const double value_;
  
  // Simulation.
  double value (const double) const
  { return value_; }

  // Create.
  FunctionConst (const BlockModel& al)
    : Function (al),
      value_ (al.number ("value"))
  { }
};

static struct FunctionConstSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new FunctionConst (al); }
  FunctionConstSyntax ()
    : DeclareModel (Function::component, "const", 
		    "Always return the same number.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("value", Attribute::User (), Attribute::Const, "\
The number to return.");
    frame.order ("value");
  }
} FunctionConst_syntax;

// The 'plf' model.

struct FunctionPLF : public Function
{
  const PLF plf;
  
  // Simulation.
  double value (const double x) const
  { return plf (x); }

  // Create.
  FunctionPLF (const BlockModel& al)
    : Function (al),
      plf (al.plf ("plf"))
  { }
};

static struct FunctionPLFSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new FunctionPLF (al); }
  FunctionPLFSyntax ()
    : DeclareModel (Function::component, "plf", 
		    "A piecewise linear function.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("plf", Attribute::User (), Attribute::User (),
		   Attribute::Const, "\
The piecewise linear function.");
    frame.order ("plf");
  }
} FunctionPLF_syntax;

// function.C ends here.
