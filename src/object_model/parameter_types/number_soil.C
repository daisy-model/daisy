// number_soil.C -- Extract soil properties.
// 
// Copyright 2005 Per Abrahamsen and KVL.
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

#include "object_model/parameter_types/number.h"
#include "object_model/metalib.h"
#include "object_model/library.h"
#include "object_model/block_model.h"
#include "daisy/column.h"
#include "daisy/soil/horizon.h"
#include "daisy/soil/hydraulic.h"
#include "daisy/daisy_time.h"
#include "object_model/librarian.h"
#include "util/scope.h"
#include "object_model/units.h"
#include "object_model/treelog.h"
#include "object_model/frame_model.h"
#include "daisy/soil/transport/secondary.h"
#include <memory>

void
NumberByDepth::tick (const Units& units, const Scope& scope, Treelog& msg)
{
  h_->tick (units, scope, msg);
  z_->tick (units, scope, msg);
}

bool
NumberByDepth::missing (const Scope& scope) const
{
  if (h_->missing (scope)
      || !units_.can_convert (h_->dimension (scope), Units::cm (),
                              h_->value (scope))
      || z_->missing (scope)
      || !units_.can_convert (z_->dimension (scope), Units::cm (),
                              z_->value (scope)))
    return true;

  const double height = units_.convert (z_->dimension (scope),
                                        Units::cm (),
                                        z_->value (scope));
  if (height > 0 || height < max_depth_)
    return true;

  return false;
}

bool
NumberByDepth::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  bool ok = true;
  TREELOG_MODEL (msg);
  if (!h_->initialize (units, scope, msg))
    ok = false;
  if (!z_->initialize (units, scope, msg))
    ok = false;
  return ok;
}

bool
NumberByDepth::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  bool ok = true;
  TREELOG_MODEL (msg);
  if (!h_->check (units, scope, msg))
    ok = false;
  else if (!units.can_convert (h_->dimension (scope), Units::cm ()))
    {
      msg.error ("Cannot convert pressure [" + h_->dimension (scope)
                 + "] to [cm] for soil hydraulics");
      ok = false;
    }
  if (!z_->check (units, scope, msg))
    ok = false;
  else if (!units.can_convert (z_->dimension (scope), Units::cm ()))
    {
      msg.error ("Cannot convert height [" + z_->dimension (scope)
                 + "] to [cm] for soil hydraulics");
      ok = false;
    }
  return ok;
}

NumberByDepth::NumberByDepth (const BlockModel& al)
  : Number (al),
    units_ (al.units ()),
    column_ (Librarian::build_item<Column> (al, "column")),
    max_depth_ (0.0),
    h_ (Librarian::build_item<Number> (al, "h")),
    z_ (Librarian::build_item<Number> (al, "z"))
{
  column_->initialize (al);
  max_depth_ = column_->bottom ();
}

NumberByDepth::~NumberByDepth ()
{ }

static struct NumberDepthSyntax : public DeclareBase
{
  NumberDepthSyntax ()
    : DeclareBase (Number::component, "depth", 
                   "Find soil value at specific depth.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("column", Column::component, "\
The soil column whose properties we want to examine.");
    frame.declare_object ("h", Number::component, "\
The tension we want to compare with.");
    frame.declare_object ("z", Number::component, "\
The height we want to compare with.");
  }
} NumberDepth_syntax;

double
NumberDepthTheta::value (const Scope& scope) const
{
  const double pressure
    = units_.convert (h_->dimension (scope), Units::cm (), h_->value (scope));
  const double height
    = units_.convert (z_->dimension (scope), Units::cm (), z_->value (scope));
  const Horizon& horizon = column_->horizon_at (height, 0.5, 0.5);
  return horizon.hydraulic->Theta (pressure);
}

symbol
NumberDepthTheta::dimension (const Scope&) const
{ return Attribute::Fraction (); }

NumberDepthTheta::NumberDepthTheta (const BlockModel& al)
  : NumberByDepth (al)
{ }

static struct NumberDepthThetaSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberDepthTheta (al); }
  NumberDepthThetaSyntax ()
    : DeclareModel (Number::component, "depth_Theta", "depth",
                    "Find water content (Theta) for a given pressure (h).")
  { }
  void load_frame (Frame&) const
  { }
} NumberDepthTheta_syntax;

double
NumberDepthK::value (const Scope& scope) const
{
  const double pressure
    = units_.convert (h_->dimension (scope), Units::cm (), h_->value (scope));
  const double height
    = units_.convert (z_->dimension (scope), Units::cm (), z_->value (scope));
  const Horizon& horizon = column_->horizon_at (height, 0.5, 0.5);
  return horizon.hydraulic->KT20 (pressure);
}

symbol
NumberDepthK::dimension (const Scope&) const
{ return units_.cm_per_h (); }

NumberDepthK::NumberDepthK (const BlockModel& al)
  : NumberByDepth (al)
{ }

static struct NumberDepthKSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberDepthK (al); }
  NumberDepthKSyntax ()
    : DeclareModel (Number::component, "depth_K", "depth", 
                    "Find water conductivity (K) for a given pressure (h).")
  { }
  void load_frame (Frame&) const
  { }
} NumberDepthK_syntax;

void
NumberByTension::tick (const Units& units, const Scope& scope, Treelog& msg)
{ h_->tick (units, scope, msg); }

bool
NumberByTension::missing (const Scope& scope) const
{
  return h_->missing (scope)
    || !units_.can_convert (h_->dimension (scope), Units::cm (),
                            h_->value (scope));
}

bool
NumberByTension::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  bool ok = true;
  TREELOG_MODEL (msg);
  if (!h_->initialize (units, scope, msg))
    ok = false;
  return ok;
}

bool
NumberByTension::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  if (!h_->check (units, scope, msg))
    return false;
  if (!units.can_convert (h_->dimension (scope), Units::cm ()))
    {
      msg.error ("Cannot convert [" + h_->dimension (scope)
                 + "] to [cm] for soil hydraulics");
      return false;
    }
  return true;
}

NumberByTension::NumberByTension (const BlockModel& al)
  : Number (al),
    units_ (al.units ()),
    horizon_ (Librarian::build_item<Horizon> (al, "horizon")),
    h_ (Librarian::build_item<Number> (al, "h"))
{
  horizon_->initialize (al.flag ("top_soil"), 2,
                        al.flag ("top_soil") ? -10.0 : -50.0,
                        al.msg ());
}

NumberByTension::~NumberByTension ()
{ }

static struct NumberHorizonSyntax : public DeclareBase
{
  NumberHorizonSyntax ()
    : DeclareBase (Number::component, "horizon", 
                   "Find soil value at specific horizon.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("horizon", Horizon::component, "\
The soil horizon whose properties we want to examine.");
    frame.declare_object ("h", Number::component, "\
The tension we want to compare with.");
    frame.declare_boolean ("top_soil", Attribute::Const, "\
Set this to true for the A horizon.");
  }
} NumberHorizon_syntax;


void
NumberSoilTheta::tick (const Units&, const Scope&, Treelog&)
{ }

double
NumberSoilTheta::value (const Scope& scope) const
{
  return horizon_->hydraulic->Theta (units_.convert (h_->dimension (scope),
                                                     Units::cm (),
                                                     h_->value (scope)));
}

symbol
NumberSoilTheta::dimension (const Scope&) const
{ return Attribute::Fraction (); }

NumberSoilTheta::NumberSoilTheta (const BlockModel& al)
  : NumberByTension (al)
{ }

static struct NumberSoilThetaSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSoilTheta (al); }
  NumberSoilThetaSyntax ()
    : DeclareModel (Number::component, "soil_Theta", "horizon",
	       "Find water content (Theta) for a given pressure (h).")
  { }
  void load_frame (Frame&) const
  { }
} NumberSoilTheta_syntax;

double
NumberSoilK::value (const Scope& scope) const
{
  const double h_cm = units_.convert (h_->dimension (scope),
                                      Units::cm (),
                                      h_->value (scope));
  const double K_primary = horizon_->hydraulic->KT20 (h_cm);
  const double K_secondary = horizon_->secondary_domain ().K (h_cm);
  const double K_factor = horizon_->K_factor ();
  return K_factor * std::max (K_primary, K_secondary);
}

symbol
NumberSoilK::dimension (const Scope&) const
{ return units_.cm_per_h (); }

NumberSoilK::NumberSoilK (const BlockModel& al)
  : NumberByTension (al)
{ }

static struct NumberSoilKSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSoilK (al); }
  NumberSoilKSyntax ()
    : DeclareModel (Number::component, "soil_K", "horizon",
	       "Find hydraulic conductivity (K) for a given pressure (h).")
  { }
  void load_frame (Frame&) const
  { }
} NumberSoilK_syntax;

double
NumberSoilHeatCapacity::value (const Scope& scope) const
{
  const symbol my_dim = h_->dimension (scope);
  const double my_val = h_->value (scope);
  const double my_h = units_.convert (my_dim, Units::cm (), my_val);
  const double Theta = horizon_->hydraulic->Theta (my_h);
  return horizon_->heat_capacity (Theta, 0.0);
}

symbol
NumberSoilHeatCapacity::dimension (const Scope&) const
{
  static const symbol dim ("erg/cm^3/dg C");
  return dim;
}

NumberSoilHeatCapacity::NumberSoilHeatCapacity (const BlockModel& al)
  : NumberByTension (al)
{ }

static struct NumberSoilHeatCapacitySyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSoilHeatCapacity (al); }
  NumberSoilHeatCapacitySyntax ()
    : DeclareModel (Number::component, "soil_heat_capacity", "horizon",
	       "Find heat capacity for a given pressure (h).")
  { }
  void load_frame (Frame&) const
  { }
} NumberSoilHeatCapacity_syntax;

double
NumberSoilHeatConductivity::value (const Scope& scope) const
{
  const symbol my_dim = h_->dimension (scope);
  const double my_val = h_->value (scope);
  const double my_h = units_.convert (my_dim, Units::cm (), my_val);
  const double Theta = horizon_->hydraulic->Theta (my_h);
  return horizon_->heat_conductivity (Theta, 0.0);
}

symbol
NumberSoilHeatConductivity::dimension (const Scope&) const
{
  static const symbol dim ("erg/cm/h/dg C");
  return dim;
}

NumberSoilHeatConductivity::NumberSoilHeatConductivity (const BlockModel& al)
  : NumberByTension (al)
{ }

static struct NumberSoilHeatConductivitySyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberSoilHeatConductivity (al); }
  NumberSoilHeatConductivitySyntax ()
    : DeclareModel (Number::component, "soil_heat_conductivity", "horizon",
	       "Find heat conductivity for a given pressure (h).")
  { }
  void load_frame (Frame&) const
  { }
} NumberSoilHeatConductivity_syntax;

void
NumberTensionByTheta::tick (const Units& units, const Scope& scope, Treelog& msg)
{ theta_->tick (units, scope, msg); }

bool
NumberTensionByTheta::missing (const Scope& scope) const
{
  return theta_->missing (scope)
    || !units_.can_convert (theta_->dimension (scope), Attribute::Fraction (),
                            theta_->value (scope));
}

double
NumberTensionByTheta::value (const Scope& scope) const
{
  return horizon_->hydraulic->h (units_.convert (theta_->dimension (scope),
                                                 Attribute::Fraction (),
                                                 theta_->value (scope)));
}

symbol
NumberTensionByTheta::dimension (const Scope&) const
{ return Units::cm (); }

bool
NumberTensionByTheta::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  TREELOG_MODEL (msg);
  return theta_->initialize (units, scope, msg);
}

bool
NumberTensionByTheta::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  if (!theta_->check (units, scope, msg))
    return false;
  if (!units.can_convert (theta_->dimension (scope), Attribute::Fraction ()))
    {
      msg.error ("Cannot convert [" + theta_->dimension (scope)
                 + "] to fraction for soil hydraulics");
      return false;
    }
  return true;
}

NumberTensionByTheta::NumberTensionByTheta (const BlockModel& al)
  : Number (al),
    units_ (al.units ()),
    horizon_ (Librarian::build_item<Horizon> (al, "horizon")),
    theta_ (Librarian::build_item<Number> (al, "Theta"))
{
  horizon_->initialize (al.flag ("top_soil"), 2,
                        al.flag ("top_soil") ? -10.0 : -50.0,
                        al.msg ());
}

NumberTensionByTheta::~NumberTensionByTheta ()
{ }

static struct NumberTensionByThetaSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberTensionByTheta (al); }
  NumberTensionByThetaSyntax()
    : DeclareModel (Number::component, "daisy/soil/soil.h", 
                    "Find pressure (h) for a given water content (Theta).")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_object ("horizon", Horizon::component, "\
The soil horizon whose properties we want to examine.");
    frame.declare_object ("Theta", Number::component, "\
The water content we want to compare with.");
    frame.declare_boolean ("top_soil", Attribute::Const, "\
Set this to true for the A horizon.");
  }
} NumberTensionByTheta_syntax;

// number_soil.C ends here
