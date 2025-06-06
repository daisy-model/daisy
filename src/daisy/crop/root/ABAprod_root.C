// ABAprod_root.C  -- ABA production based on root length.
// 
// Copyright 2007 Per Abrahamsen and KVL.
//
// This file is part of Daisy.
// 
// Daisy is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.5
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

#include "daisy/crop/root/ABAprod.h"
#include "object_model/parameter_types/number.h"
#include "util/scope_id.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/soil/soil_water.h"
#include "object_model/units.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"
#include "object_model/treelog.h"
#include "object_model/block_model.h"

struct ABAProdRoot : public ABAProd
{
  // Units.
  static const symbol h_name;
  static const symbol ABA_unit;

  // Parameters.
  mutable ScopeID scope;
  const std::unique_ptr<Number> expr;
  
  // Solve.
  void production (const Geometry&, const SoilWater&,
		   const std::vector<double>& S /* [cm^3/cm^3/h] */,
		   const std::vector<double>& l /* [cm/cm^3] */,
		   std::vector<double>& ABA /* [g/cm^3/h] */,
		   Treelog&) const;
  void output (Log&) const
  { }

  // Create and Destroy.
  void initialize (Treelog&);
  bool check (Treelog&) const;
  ABAProdRoot (const BlockModel& al);
  ~ABAProdRoot ();
};

const symbol 
ABAProdRoot::h_name ("h");

const symbol 
ABAProdRoot::ABA_unit ("g/cm/h");

void
ABAProdRoot::production (const Geometry& geo, const SoilWater& soil_water,
			 const std::vector<double>& /* [cm^3/cm^3/h] */,
			 const std::vector<double>& l /* [cm/cm^3] */,
			 std::vector<double>& ABA    /* [g/cm^3/h] */,
			 Treelog& msg) const
{
  // Check input.
  const size_t cell_size = geo.cell_size ();
  daisy_assert (ABA.size () == cell_size);
  daisy_assert (l.size () == cell_size);
  
  // For all cells.
  for (size_t c = 0; c < cell_size; c++)
    {
      // Set up 'h' in scope.
      scope.set (h_name, soil_water.h (c));

      // Find root value.
      double value = 0.0;
      if (!expr->tick_value (units, value, ABA_unit, scope, msg))
	msg.error ("No ABA production value found");

      // Find ABA uptake.
      ABA[c] = value * l[c];	// [g/cm^3 S/h] = [g/cm/h] * [cm/cm^3 S]
    }
}

void 
ABAProdRoot::initialize (Treelog& msg)
{ expr->initialize (units, scope, msg); }

bool 
ABAProdRoot::check (Treelog& msg) const
{
  bool ok = true;

  if (!expr->check_dim (units, scope, ABA_unit, msg))
    ok = false;

  return ok;
}

ABAProdRoot::ABAProdRoot (const BlockModel& al)
  : ABAProd (al),
    scope (h_name, Units::cm ()),
    expr (Librarian::build_item<Number> (al, "expr"))
{ }

ABAProdRoot::~ABAProdRoot ()
{ }

static struct ABAProdRootSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ABAProdRoot (al); }
  ABAProdRootSyntax ()
    : DeclareModel (ABAProd::component, "root", "\
ABA production based on production in roots.\n\
\n\
The assumptions are that that each length of root will produce ABA\n\
with a rate that depends solely on the water pressure in that cell,\n\
and that all the ABA will be included in the water uptake.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("expr", Number::component, 
                      Attribute::Const, Attribute::Singleton, "\
Expression to evaluate to ABA production per root length [g/cm/h].\n\
The symbol 'h' will be bound to the water pressure [cm].");
  }
} ABAProdRoot_syntax;

// ABAprod_root.C ends here

