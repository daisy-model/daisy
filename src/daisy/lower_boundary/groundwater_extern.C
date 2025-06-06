// groundwater_extern.C --- Get groundwater table from external model.
// 
// Copyright 2007 Per Abrahamsen and KVL.
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

#include "daisy/lower_boundary/groundwater.h"
#include "daisy/output/output.h"
#include "object_model/parameter_types/number.h"
#include "object_model/block_model.h"
#include "object_model/units.h"
#include "object_model/check.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"
#include <sstream>

class GroundwaterExtern : public Groundwater
{
  // Content.
private:
  const Units& units;
  const std::unique_ptr<Number> expr;
  bool has_table;
  double depth;
  
  // Groundwater.
public:
  bottom_t bottom_type () const
  { return has_table ? pressure : free_drainage; }
  double q_bottom (size_t) const
  { daisy_notreached (); }

  // Simulation.
public:
  void tick (const Geometry&,
             const Soil&, SoilWater&, double, 
	     const SoilHeat&, const Time&, const Scope& scope, Treelog& msg)
  { has_table = expr->tick_value (units, depth, Units::cm (), scope, msg); }
  double table () const
  { return depth; }

  // Create and Destroy.
public:
  void initialize (const Geometry&, const Time&, const Scope& scope,
                   Treelog& msg)
  { expr->initialize (units, scope, msg); }

  bool check (const Geometry&, const Scope& scope, Treelog& msg) const
  {
    bool ok = true;
    if (!expr->check_dim (units, scope, Units::cm (), msg))
      ok = false;
    return ok;
  }
      
  GroundwaterExtern (const BlockModel& al)
    : Groundwater (al),
      units (al.units ()),
      expr (Librarian::build_item<Number> (al, "table")),
      has_table (al.check ("initial_table")),
      depth (al.number ("initial_table", -42.42e42))
  { }
  ~GroundwaterExtern ()
  { }
};

static struct GroundwaterExternSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new GroundwaterExtern (al); }
  GroundwaterExternSyntax ()
    : DeclareModel (Groundwater::component, "extern", "common", "\
Look up groundwater table in an scope.  ")
  { }
  void load_frame (Frame& frame) const
  { 
    frame.declare_object ("table", Number::component, 
                       Attribute::Const, Attribute::Singleton, "\
Expression that evaluates to groundwate table in.");
    frame.declare ("initial_table", "cm", Check::none (), Attribute::OptionalConst,
		"Groundwater level for initialization of soil water.");
  }
} GroundwaterExtern_syntax;
