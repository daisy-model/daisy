// number_apply.C -- Apply function on number.
// 
// Copyright 2004, 2005, 2007, 2024 Per Abrahamsen and KVL.
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
#include "object_model/units.h"
#include "util/memutils.h"
#include "object_model/function.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/submodeler.h"
#include "object_model/treelog.h"
#include "object_model/frame_submodel.h"
#include "object_model/metalib.h"
#include <sstream>
#include <memory>

#include "util/assertion.h"

struct NumberApply : public Number
{
  // Parameters.
  const std::unique_ptr<Function> function;
  const double operand;
  const symbol range;

  // Simulation.
  void tick (const Units& units, const Scope& scope, Treelog& msg)
  { }
  bool missing (const Scope& scope) const 
  { return false; }
  double value (const Scope& scope) const
  { return function->value (operand); }
  symbol dimension (const Scope&) const
  { return range; }

  // Create.
  bool initialize (const Units& units, const Scope& scope, Treelog& msg)
  { return true; }
  bool check (const Units& units, const Scope& scope, Treelog& msg) const
  { return true; }

  static bool check_alist (const Metalib& metalib, 
                           const Frame& al, Treelog& msg) 
  {
    bool ok = true;

    const symbol function_domain = al.model ("function").name ("domain");
    const symbol operand_domain = al.name ("operand");

    if (function_domain != operand_domain)
      {
	msg.error (function_domain.name () + " != " + operand_domain.name ());
	ok = false;
      }

    return ok;
  }

  NumberApply (const BlockModel& al)
    : Number (al),
      function (Librarian::build_item<Function> (al, "function")),
      operand (al.number ("operand")),
      range (al.model ("function").name ("range"))
  { }
};

static struct NumberApplySyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberApply (al); }

  NumberApplySyntax ()
    : DeclareModel (Number::component, "apply", 
		    "Apply function to operand.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.add_check (NumberApply::check_alist);

    frame.declare_function ("function", Attribute::User (), Attribute::User (),
			    "Function to apply.");
    frame.declare ("operand", Attribute::User (), Attribute::Const, "\
Operand for this function.");
    frame.order ("function", "operand");
  }
} NumberApply_syntax;

// number_apply.C ends here.
