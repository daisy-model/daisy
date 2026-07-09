// stringer.C --- Strings in Daisy.
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

#include "object_model/parameter_types/stringer.h"
#include "object_model/object_model_registration_internal.h"
#include "object_model/parameter_types/boolean.h"
#include "object_model/parameter_types/number.h"
#include "object_model/submodeler.h"
#include "object_model/frame.h"
#include "util/memutils.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include "object_model/block_model.h"
#include <sstream>
#include <vector>
#include <memory>

const char *const Stringer::component = "string";

symbol
Stringer::library_id () const
{
  static const symbol id (component);
  return id;
}

const std::string& 
Stringer::title () const
{ return name.name (); }

Stringer::Stringer (const symbol direct_name)
  : name (direct_name)
{ }

Stringer::Stringer (const BlockModel& al)
  : name (al.type_name ())
{ }

Stringer::~Stringer ()
{ }

void
StringerNumber::tick (const Units& units, const Scope& scope, Treelog& msg)
{ number_->tick (units, scope, msg); }

bool
StringerNumber::missing (const Scope& scope) const
{ return number_->missing (scope); }

bool
StringerNumber::initialize (const Units& units, const Scope& scope,
                            Treelog& msg)
{
  Treelog::Open nest (msg, name);
  return number_->initialize (units, scope, msg);
}

bool
StringerNumber::check (const Units& units, const Scope& scope,
                       Treelog& msg) const
{
  Treelog::Open nest (msg, name);
  return number_->check (units, scope, msg);
}

StringerNumber::StringerNumber (const symbol direct_name,
                                std::unique_ptr<Number> number)
  : Stringer (direct_name),
    number_ (std::move (number))
{ }

StringerNumber::StringerNumber (const BlockModel& al)
  : Stringer (al),
    number_ (Librarian::build_item<Number> (al, "number"))
{ }

StringerNumber::~StringerNumber ()
{ }

symbol
StringerValue::value (const Scope& scope) const
{
  std::ostringstream tmp;
  if (precision_ >= 0)
    {
      tmp.precision (precision_);
      tmp.flags (std::ios::right | std::ios::fixed);
    }
  tmp << number_->value (scope);
  return tmp.str ();
}

StringerValue::StringerValue (std::unique_ptr<Number> number, const int precision)
  : StringerNumber ("value", std::move (number)),
    precision_ (precision)
{ }

StringerValue::StringerValue (const BlockModel& al)
  : StringerNumber (al),
    precision_ (al.integer ("precision", -1))
{ }

symbol
StringerDimension::value (const Scope& scope) const
{ return number_->dimension (scope); }

StringerDimension::StringerDimension (std::unique_ptr<Number> number)
  : StringerNumber ("dimension", std::move (number))
{ }

StringerDimension::StringerDimension (const BlockModel& al)
  : StringerNumber (al)
{ }

void
StringerIdentity::tick (const Units&, const Scope&, Treelog&)
{ }

bool
StringerIdentity::missing (const Scope&) const
{ return false; }

symbol
StringerIdentity::value (const Scope&) const
{ return val_; }

bool
StringerIdentity::initialize (const Units&, const Scope&, Treelog&)
{ return true; }

bool
StringerIdentity::check (const Units&, const Scope&, Treelog&) const
{ return true; }

StringerIdentity::StringerIdentity (const symbol value)
  : Stringer ("identity"),
    val_ (value)
{ }

StringerIdentity::StringerIdentity (const BlockModel& al)
  : Stringer (al),
    val_ (al.name ("value"))
{ }

StringerIdentity::~StringerIdentity ()
{ }

void
StringerCond::Clause::load_syntax (Frame& frame)
{
  frame.declare_object ("condition", Boolean::component, "\
Condition to test for.");
  frame.declare_string ("value", Attribute::Const, "\
Value to return.");
  frame.order ("condition", "value");
}

StringerCond::Clause::Clause (std::unique_ptr<Boolean> condition, const symbol value)
  : condition_ (std::move (condition)),
    value_ (value)
{ }

StringerCond::Clause::Clause (const Block& al)
  : condition_ (Librarian::build_item<Boolean> (al, "condition")),
    value_ (al.name ("value"))
{ }

void
StringerCond::Clause::tick (const Units& units, const Scope& scope,
                            Treelog& msg) const
{ condition_->tick (units, scope, msg); }

void
StringerCond::Clause::initialize (const Units& units, const Scope& scope,
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
StringerCond::Clause::matches (const Scope& scope) const
{ return condition_->value (scope); }

symbol
StringerCond::Clause::value () const
{ return value_; }

void
StringerCond::tick (const Units& units, const Scope& scope, Treelog& msg)
{
  for (size_t i = 0; i < clauses_.size (); i++)
    clauses_[i].tick (units, scope, msg);
}

bool
StringerCond::missing (const Scope&) const
{ return false; }

symbol
StringerCond::value (const Scope& scope) const
{
  for (size_t i = 0; i < clauses_.size (); i++)
    if (clauses_[i].matches (scope))
      return clauses_[i].value ();
  throw "No matching conditions";
}

bool
StringerCond::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  bool ok = true;
  for (size_t i = 0; i < clauses_.size (); i++)
    clauses_[i].initialize (units, scope, msg, name, i, ok);
  return ok;
}

bool
StringerCond::check (const Units&, const Scope& scope, Treelog& msg) const
{
  for (size_t i = 0; i < clauses_.size (); i++)
    if (clauses_[i].matches (scope))
      return true;
  msg.error ("No clause matches");
  return false;
}

StringerCond::StringerCond (std::vector<Clause> clauses)
  : Stringer ("cond"),
    clauses_ (std::move (clauses))
{ }

StringerCond::StringerCond (const BlockModel& al)
  : Stringer (al)
{
  auto raw_clauses = map_submodel<Clause> (al, "clauses");
  clauses_.reserve (raw_clauses.size ());
  for (size_t i = 0; i < raw_clauses.size (); ++i)
    {
      clauses_.push_back (std::move (*raw_clauses[i]));
      delete raw_clauses[i];
    }
}

namespace
{
struct StringerCondSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new StringerCond (al); }
  StringerCondSyntax ()
    : DeclareModel (Stringer::component, "cond", "\
Return the value of the first clause whose condition is true.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_submodule_sequence ("clauses", Attribute::Const, "\
List of clauses to match for.",
                                   StringerCond::Clause::load_syntax);
    frame.order ("clauses");
  }
};

struct StringerNumberSyntax : public DeclareBase
{
  StringerNumberSyntax ()
    : DeclareBase (Stringer::component, "number", "\
Extract the value of a number.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("number", Number::component, "\
Number to manipulate."); 
  }
};

struct StringerValueSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new StringerValue (al); }
  StringerValueSyntax ()
    : DeclareModel (Stringer::component, "value", "number", "\
Extract the value of a number as a string.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_integer ("precision", Attribute::OptionalConst, "\
Number of decimals after point.  By default, use a floating format.");
  }
};

struct StringerDimensionSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new StringerDimension (al); }
  StringerDimensionSyntax ()
    : DeclareModel (Stringer::component, "dimension", "number", "\
Extract the dimension of a number as a string.")
  { }
  void load_frame (Frame& frame) const
  {
  }
};

struct StringerIdentitySyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new StringerIdentity (al); }
  StringerIdentitySyntax ()
    : DeclareModel (Stringer::component, "identity", "\
Return the specified value.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_string ("value", Attribute::Const, "\
Constant value.");
  }
};

struct StringerInit : public DeclareComponent 
{
  StringerInit ()
    : DeclareComponent (Stringer::component, "\
Generic representation of strings.")
  { }
};
}

void
register_stringer_models ()
{
  static DeclareSubmodel stringer_cond_clause_submodel (
    StringerCond::Clause::load_syntax, "StringerCondClause", "\
If condition is true, return value.");
  static StringerCondSyntax stringer_cond_syntax;
  static StringerNumberSyntax stringer_number_syntax;
  static StringerValueSyntax stringer_value_syntax;
  static StringerDimensionSyntax stringer_dimension_syntax;
  static StringerIdentitySyntax stringer_identity_syntax;
  static StringerInit stringer_init;
}
