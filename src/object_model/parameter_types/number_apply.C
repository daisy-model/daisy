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

namespace
{
bool
check_apply_alist (const Metalib&, const Frame& al, Treelog& msg)
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
} // namespace

void
NumberApply::tick (const Units&, const Scope&, Treelog&)
{ }

bool
NumberApply::missing (const Scope&) const
{ return false; }

double
NumberApply::value (const Scope&) const
{ return function_->value (operand_); }

symbol
NumberApply::dimension (const Scope&) const
{ return range_; }

bool
NumberApply::initialize (const Units&, const Scope&, Treelog&)
{ return true; }

bool
NumberApply::check (const Units&, const Scope&, Treelog&) const
{ return true; }

NumberApply::NumberApply (std::unique_ptr<Function> function,
                          const double operand,
                          const symbol range)
  : Number ("apply"),
    function_ (std::move (function)),
    operand_ (operand),
    range_ (range)
{ }

NumberApply::NumberApply (const BlockModel& al)
  : Number (al),
    function_ (Librarian::build_item<Function> (al, "function")),
    operand_ (al.number ("operand")),
    range_ (al.model ("function").name ("range"))
{ }

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
    frame.add_check (check_apply_alist);

    frame.declare_function ("function", Attribute::User (), Attribute::User (),
			    "Function to apply.");
    frame.declare ("operand", Attribute::User (), Attribute::Const, "\
Operand for this function.");
    frame.order ("function", "operand");
  }
} NumberApply_syntax;

// number_apply.C ends here.
