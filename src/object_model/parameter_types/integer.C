// integer.C --- Integers in Daisy.
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

#include "object_model/parameter_types/integer.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/parameter_types/boolean.h"
#include "object_model/submodeler.h"
#include "object_model/block_model.h"
#include "util/memutils.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <sstream>

const char *const Integer::component = "integer";

symbol
Integer::library_id () const
{
  static const symbol id (component);
  return id;
}

const std::string& 
Integer::title () const
{ return name.name (); }

Integer::Integer (const symbol direct_name)
  : name (direct_name)
{ }

Integer::Integer (const BlockModel& al)
  : name (al.type_name ())
{ }

Integer::~Integer ()
{ }

bool
IntegerConst::missing (const Scope&) const
{ return false; }

int
IntegerConst::value (const Scope&) const
{ return val_; }

bool
IntegerConst::initialize (const Units&, const Scope&, Treelog&)
{ return true; }

bool
IntegerConst::check (const Scope&, Treelog&) const
{ return true; }

IntegerConst::IntegerConst (const int value)
  : Integer ("const"),
    val_ (value)
{ }

IntegerConst::IntegerConst (const BlockModel& al)
  : Integer (al),
    val_ (al.integer ("value"))
{ }

namespace
{
struct IntegerConstSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerConst (al); }
  IntegerConstSyntax ()
    : DeclareModel (Integer::component, "const", 
	       "Always give the specified value.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_integer ("value", Attribute::Const,
		"Fixed value for this integer.");
    frame.order ("value");
  }
};

struct IntegerCondSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new IntegerCond (al); }
  IntegerCondSyntax ()
    : DeclareModel (Integer::component, "cond", "\
Return the value of the first clause whose condition is true.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_submodule_sequence ("clauses", Attribute::Const, "\
List of clauses to match for.",
                                   IntegerCond::Clause::load_syntax);
    frame.order ("clauses");
  }
};

struct IntegerInit : public DeclareComponent 
{
  IntegerInit ()
    : DeclareComponent (Integer::component, "\
Generic representation of integers.")
  { }
};
}

void
IntegerCond::Clause::load_syntax (Frame& frame)
{
  frame.declare_object ("condition", Boolean::component, "\
Condition to test for.");
  frame.declare_integer ("value", Attribute::Const, "\
Value to return.");
  frame.order ("condition", "value");
}

IntegerCond::Clause::Clause (std::unique_ptr<Boolean> condition, const int value)
  : condition_ (std::move (condition)),
    value_ (value)
{ }

IntegerCond::Clause::Clause (const Block& al)
  : condition_ (Librarian::build_item<Boolean> (al, "condition")),
    value_ (al.integer ("value"))
{ }

void
IntegerCond::Clause::initialize (const Units& units, const Scope& scope,
                                 Treelog& msg, const symbol& owner_name,
                                 const size_t index, bool& ok) const
{
  std::ostringstream tmp;
  tmp << owner_name << "[" << index << "]";
  Treelog::Open nest (msg, tmp.str ());
  if (!condition_->initialize (units, scope, msg))
    ok = false;
}

bool
IntegerCond::Clause::matches (const Scope& scope) const
{ return condition_->value (scope); }

int
IntegerCond::Clause::value () const
{ return value_; }

bool
IntegerCond::missing (const Scope&) const
{ return false; }

int
IntegerCond::value (const Scope& scope) const
{
  for (size_t i = 0; i < clauses_.size (); i++)
    if (clauses_[i].matches (scope))
      return clauses_[i].value ();
  throw "No matching conditions";
}

bool
IntegerCond::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  bool ok = true;
  for (size_t i = 0; i < clauses_.size (); i++)
    clauses_[i].initialize (units, scope, msg, name, i, ok);
  return ok;
}

bool
IntegerCond::check (const Scope& scope, Treelog& msg) const
{
  for (size_t i = 0; i < clauses_.size (); i++)
    if (clauses_[i].matches (scope))
      return true;
  msg.error ("No clause matches");
  return false;
}

IntegerCond::IntegerCond (std::vector<Clause> clauses)
  : Integer ("cond"),
    clauses_ (std::move (clauses))
{ }

IntegerCond::IntegerCond (const BlockModel& al)
  : Integer (al)
{
  auto raw_clauses = map_submodel<Clause> (al, "clauses");
  clauses_.reserve (raw_clauses.size ());
  for (size_t i = 0; i < raw_clauses.size (); ++i)
    {
      clauses_.push_back (std::move (*raw_clauses[i]));
      delete raw_clauses[i];
    }
}

void
register_integer_models ()
{
  static DeclareSubmodel integer_cond_clause_submodel (
    IntegerCond::Clause::load_syntax, "IntegerCondClause", "\
If condition is true, return value.");
  static IntegerConstSyntax integer_const_syntax;
  static IntegerCondSyntax integer_cond_syntax;
  static IntegerInit integer_init;
}

// integer.C ends here
