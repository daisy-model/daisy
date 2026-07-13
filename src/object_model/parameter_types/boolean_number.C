// boolean_number.C --- Boolean operations on numbers.
// 
// Copyright 2006 Per Abrahamsen and KVL.
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

#include "object_model/parameter_types/boolean.h"
#include "object_model/frame.h"
#include "object_model/librarian.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/treelog.h"
#include "object_model/block_model.h"
#include <sstream>
#include <vector>

namespace
{
std::vector<std::unique_ptr<Number>>
build_number_operands (const BlockModel& al)
{
  std::vector<std::unique_ptr<Number>> result;
  std::vector<Number*> operands = Librarian::build_vector<Number> (al, "operands");
  result.reserve (operands.size ());
  for (size_t i = 0; i < operands.size (); ++i)
    result.emplace_back (operands[i]);
  return result;
}
} // namespace

BooleanNumbers::BooleanNumbers (const symbol direct_name,
                                std::vector<std::unique_ptr<Number>> operands)
  : Boolean (direct_name),
    operands_ (std::move (operands))
{ }

void
BooleanNumbers::tick (const Units& units, const Scope& scope, Treelog& msg)
{
  for (size_t i = 0; i < operands_.size (); i++)
    operands_[i]->tick (units, scope, msg);
}

bool
BooleanNumbers::missing (const Scope& scope) const
{
  for (size_t i = 0; i < operands_.size (); i++)
    if (operands_[i]->missing (scope))
      return true;

  return false;
}

bool
BooleanNumbers::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  bool ok = true;

  for (size_t i = 0; i < operands_.size (); i++)
    if (!operands_[i]->initialize (units, scope, msg))
      {
        std::ostringstream tmp;
        tmp << name << "[" << i << "]";
        Treelog::Open nest (msg, tmp.str ());
        ok = false;
      }
  return ok;
}

bool
BooleanNumbers::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  Treelog::Open nest (msg, name);
  bool ok = true;

  symbol dim = Attribute::Unknown ();
  for (size_t i = 0; i < operands_.size (); i++)
    if (!operands_[i]->check (units, scope, msg))
      ok = false;
    else
      {
        static const symbol blank ("");
        symbol new_dim = operands_[i]->dimension (scope);
        if (new_dim == Attribute::None ()
            || new_dim == Attribute::Fraction ())
          new_dim = blank;
        if (new_dim != dim)
          {
            if (dim == Attribute::Unknown ())
              dim = new_dim;
            else if (new_dim != Attribute::Unknown ())
              {
                msg.error ("I don't know how to compare [" + dim + "] with ["
                           + new_dim + "]");
                dim = new_dim;
                ok = false;
              }
          }
      }
  return ok;
}

BooleanNumbers::BooleanNumbers (const BlockModel& al)
  : Boolean (al),
    operands_ (build_number_operands (al))
{ }

namespace
{
struct BooleanNumbersSyntax : public DeclareBase
{
  BooleanNumbersSyntax ()
    : DeclareBase (Boolean::component, "numbers", "\
Base class for boolean expressions involving numbers.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("operands", Number::component,
                       Attribute::Const, Attribute::Variable, "\
List of operands to compare.");
    frame.order ("operands");
  }
};
}

bool
BooleanNumGT::value (const Scope& scope) const
{
  const size_t size = operands_.size ();
  if (size < 1)
    return true;
  double prev = operands_[0]->value (scope);
  for (size_t i = 1; i < size; i++)
    {
      const double next = operands_[i]->value (scope);
      if (!(prev > next))
        return false;
      prev = next;
    }
  return true;
}

BooleanNumGT::BooleanNumGT (std::vector<std::unique_ptr<Number>> operands)
  : BooleanNumbers (">", std::move (operands))
{ }

BooleanNumGT::BooleanNumGT (const BlockModel& al)
  : BooleanNumbers (al)
{ }

namespace
{
struct BooleanNumGTSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new BooleanNumGT (al); }
  BooleanNumGTSyntax ()
    : DeclareModel (Boolean::component, ">", "numbers",
                    "True iff each operand is larger than the next.")
  { }
  void load_frame (Frame&) const
  { }
};
}

bool
BooleanNumGTE::value (const Scope& scope) const
{
  const size_t size = operands_.size ();
  if (size < 1)
    return true;
  double prev = operands_[0]->value (scope);
  for (size_t i = 1; i < size; i++)
    {
      const double next = operands_[i]->value (scope);
      if (!(prev >= next))
        return false;
      prev = next;
    }
  return true;
}

BooleanNumGTE::BooleanNumGTE (std::vector<std::unique_ptr<Number>> operands)
  : BooleanNumbers (">=", std::move (operands))
{ }

BooleanNumGTE::BooleanNumGTE (const BlockModel& al)
  : BooleanNumbers (al)
{ }

namespace
{
struct BooleanNumGTESyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new BooleanNumGTE (al); }
  BooleanNumGTESyntax ()
    : DeclareModel (Boolean::component, ">=", "numbers", "\
True iff each operand is at least as large as the next.")
  { }
  void load_frame (Frame&) const
  { }
};
}

bool
BooleanNumLT::value (const Scope& scope) const
{
  const size_t size = operands_.size ();
  if (size < 1)
    return true;
  double prev = operands_[0]->value (scope);
  for (size_t i = 1; i < size; i++)
    {
      const double next = operands_[i]->value (scope);
      if (!(prev < next))
        return false;
      prev = next;
    }
  return true;
}

BooleanNumLT::BooleanNumLT (std::vector<std::unique_ptr<Number>> operands)
  : BooleanNumbers ("<", std::move (operands))
{ }

BooleanNumLT::BooleanNumLT (const BlockModel& al)
  : BooleanNumbers (al)
{ }

namespace
{
struct BooleanNumLTSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new BooleanNumLT (al); }
  BooleanNumLTSyntax ()
    : DeclareModel (Boolean::component, "<", "numbers",
                    "True iff each operand is smaller than the next.")
  { }
  void load_frame (Frame&) const
  { }
};
}

bool
BooleanNumLTE::value (const Scope& scope) const
{
  const size_t size = operands_.size ();
  if (size < 1)
    return true;
  double prev = operands_[0]->value (scope);
  for (size_t i = 1; i < size; i++)
    {
      const double next = operands_[i]->value (scope);
      if (!(prev <= next))
        return false;
      prev = next;
    }
  return true;
}

BooleanNumLTE::BooleanNumLTE (std::vector<std::unique_ptr<Number>> operands)
  : BooleanNumbers ("<=", std::move (operands))
{ }

BooleanNumLTE::BooleanNumLTE (const BlockModel& al)
  : BooleanNumbers (al)
{ }

namespace
{
struct BooleanNumLTESyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new BooleanNumLTE (al); }
  BooleanNumLTESyntax ()
    : DeclareModel (Boolean::component, "<=", "numbers", "\
True iff each operand is smaller than or equal to the next.")
  { }
  void load_frame (Frame&) const
  { }
};
}

void
register_boolean_number_models ()
{
  static BooleanNumbersSyntax boolean_numbers_syntax;
  static BooleanNumGTSyntax boolean_num_gt_syntax;
  static BooleanNumGTESyntax boolean_num_gte_syntax;
  static BooleanNumLTSyntax boolean_num_lt_syntax;
  static BooleanNumLTESyntax boolean_num_lte_syntax;
}

// boolean_number.C ends here.
