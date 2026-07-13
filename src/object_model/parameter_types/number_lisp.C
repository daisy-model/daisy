// number_lisp.C -- Lisp like constructs with numbers.
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

#include "object_model/parameter_types/number.h"
#include "object_model/parameter_types/boolean.h"
#include "util/scope_multi.h"
#include "object_model/submodeler.h"
#include "util/memutils.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "util/assertion.h"
#include "object_model/frame.h"
#include "object_model/block_model.h"
#include <sstream>
#include <memory>
#include <map>

void
NumberLet::Clause::load_syntax (Frame& frame)
{
  frame.declare_string ("identifier", Attribute::Const,
                        "Identifier to bind.");
  frame.declare_object ("expr", Number::component,
                        " Value to give it.");
  frame.order ("identifier", "expr");
}

NumberLet::Clause::Clause (const symbol id, std::unique_ptr<Number> expr)
  : id_ (id),
    expr_ (std::move (expr))
{ }

NumberLet::Clause::Clause (const Block& al)
  : id_ (al.name ("identifier")),
    expr_ (Librarian::build_item<Number> (al, "expr"))
{ }

const symbol&
NumberLet::Clause::id () const
{ return id_; }

Number&
NumberLet::Clause::expr ()
{ return *expr_; }

const Number&
NumberLet::Clause::expr () const
{ return *expr_; }

void
NumberLet::ScopeClause::tick (const Units& units, const Scope& scope, Treelog& msg)
{
  numbers_.clear ();
  dimensions_.clear ();
  for (size_t i = 0; i < clauses_.size (); i++)
    {
      const symbol id = clauses_[i].id ();
      Number& expr = clauses_[i].expr ();
      expr.tick (units, scope, msg);
      if (!expr.missing (scope))
        {
          numbers_[id] = expr.value (scope);
          dimensions_[id] = expr.dimension (scope);
        }
    }
}

void
NumberLet::ScopeClause::entries (std::set<symbol>& all) const
{
  for (std::map<symbol, double>::const_iterator i = numbers_.begin ();
       i != numbers_.end (); ++i)
    all.insert ((*i).first);
}

Attribute::type
NumberLet::ScopeClause::lookup (const symbol id) const
{ return check (id) ? Attribute::Number : Attribute::Error; }

bool
NumberLet::ScopeClause::check (symbol id) const
{
  const std::map<symbol, double>::const_iterator i = numbers_.find (id);
  return i != numbers_.end ();
}

double
NumberLet::ScopeClause::number (symbol id) const
{
  const std::map<symbol, double>::const_iterator i = numbers_.find (id);
  daisy_assert (i != numbers_.end ());
  return (*i).second;
}

symbol
NumberLet::ScopeClause::dimension (symbol id) const
{
  const std::map<symbol, symbol>::const_iterator i = dimensions_.find (id);
  daisy_assert (i != dimensions_.end ());
  return (*i).second;
}

symbol
NumberLet::ScopeClause::description (symbol) const
{ return symbol ("Descriptions not implemented yet"); }

bool
NumberLet::ScopeClause::initialize (const Units& units, const Scope& scope,
                                    Treelog& msg)
{
  bool ok = true;
  for (size_t i = 0; i < clauses_.size (); i++)
    {
      std::ostringstream tmp;
      tmp << "clauses[" << i << "]";
      Treelog::Open nest (msg, tmp.str ());
      if (!clauses_[i].expr ().initialize (units, scope, msg))
        ok = false;
    }
  if (ok)
    tick (units, scope, msg);
  return ok;
}

bool
NumberLet::ScopeClause::check (const Units& units, const Scope& scope,
                               Treelog& msg) const
{
  bool ok = true;
  for (size_t i = 0; i < clauses_.size (); i++)
    {
      std::ostringstream tmp;
      tmp << "clauses[" << i << "]";
      Treelog::Open nest (msg, tmp.str ());
      if (!clauses_[i].expr ().check (units, scope, msg))
        ok = false;
    }
  return ok;
}

void
NumberLet::ScopeClause::load_syntax (Frame& frame)
{
  frame.declare_submodule_sequence ("clauses", Attribute::Const, "\
List of identifiers and values to bind in this scope.", Clause::load_syntax);
}

void
NumberLet::load_syntax (Frame& frame)
{
  ScopeClause::load_syntax (frame);
  frame.declare_object ("expr", Number::component, "\
Expression to evaluate.");
  frame.order ("clauses", "expr");
}

NumberLet::ScopeClause::ScopeClause (std::vector<Clause> clauses)
  : clauses_ (std::move (clauses))
{ }

NumberLet::ScopeClause::ScopeClause (const BlockModel& al)
  : clauses_ ()
{
  auto raw_clauses = map_submodel<Clause> (al, "clauses");
  clauses_.reserve (raw_clauses.size ());
  for (size_t i = 0; i < raw_clauses.size (); ++i)
    {
      clauses_.push_back (std::move (*raw_clauses[i]));
      delete raw_clauses[i];
    }
}

bool
NumberLet::missing (const Scope& inherit_scope) const
{
  ScopeMulti scope (scope_clause_, inherit_scope);
  return expr_->missing (scope);
}

double
NumberLet::value (const Scope& inherit_scope) const
{
  ScopeMulti scope (scope_clause_, inherit_scope);
  return expr_->value (scope);
}

symbol
NumberLet::dimension (const Scope& inherit_scope) const
{
  ScopeMulti scope (scope_clause_, inherit_scope);
  return expr_->dimension (scope);
}

void
NumberLet::tick (const Units& units, const Scope& inherit_scope, Treelog& msg)
{
  scope_clause_.tick (units, inherit_scope, msg);
  expr_->tick (units, inherit_scope, msg);
}

bool
NumberLet::initialize (const Units& units,
                       const Scope& inherit_scope, Treelog& msg)
{
  TREELOG_MODEL (msg);
  if (!scope_clause_.initialize (units, inherit_scope, msg))
    return false;
  ScopeMulti scope (scope_clause_, inherit_scope);
  return expr_->initialize (units, scope, msg);
}

bool
NumberLet::check (const Units& units,
                  const Scope& inherit_scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  if (!scope_clause_.check (units, inherit_scope, msg))
    return false;
  scope_clause_.tick (units, inherit_scope, msg);
  ScopeMulti scope (scope_clause_, inherit_scope);
  return expr_->check (units, scope, msg);
}

NumberLet::NumberLet (std::vector<Clause> clauses, std::unique_ptr<Number> expr)
  : Number ("let"),
    scope_clause_ (std::move (clauses)),
    expr_ (std::move (expr))
{ }

NumberLet::NumberLet (const BlockModel& al)
  : Number (al),
    scope_clause_ (al),
    expr_ (Librarian::build_item<Number> (al, "expr"))
{ }

static struct NumberLetSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberLet (al); }
  NumberLetSyntax ()
    : DeclareModel (Number::component, "let", "\
Bind symbols in 'clauses' in a new scope, and evaluate 'expr' in that scope.")
  { }
  void load_frame (Frame& frame) const
  {
    NumberLet::load_syntax (frame);
  }
} NumberLet_syntax;

bool
NumberIf::missing (const Scope& scope) const
{
  return if_b_->missing (scope)
    || then_n_->missing (scope)
    || else_n_->missing (scope);
}

double
NumberIf::value (const Scope& scope) const
{
  return if_b_->value (scope)
    ? then_n_->value (scope)
    : else_n_->value (scope);
}

symbol
NumberIf::dimension (const Scope& scope) const
{
  const symbol then_dim = then_n_->dimension (scope);
  const symbol else_dim = else_n_->dimension (scope);
  if (then_dim == else_dim)
    return then_dim;

  return Attribute::Unknown ();
}

void
NumberIf::tick (const Units& units, const Scope& scope, Treelog& msg)
{
  TREELOG_MODEL (msg);
  if_b_->tick (units, scope, msg);
  then_n_->tick (units, scope, msg);
  else_n_->tick (units, scope, msg);
}

bool
NumberIf::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  TREELOG_MODEL (msg);
  bool ok = true;
  if (!if_b_->initialize (units, scope, msg))
    ok = false;
  if (!then_n_->initialize (units, scope, msg))
    ok = false;
  if (!else_n_->initialize (units, scope, msg))
    ok = false;
  return ok;
}

bool
NumberIf::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  bool ok = true;
  if (!if_b_->check (units, scope, msg))
    ok = false;
  if (!then_n_->check (units, scope, msg))
    ok = false;
  if (!else_n_->check (units, scope, msg))
    ok = false;
  return ok;
}

NumberIf::NumberIf (std::unique_ptr<Boolean> if_b,
                    std::unique_ptr<Number> then_n,
                    std::unique_ptr<Number> else_n)
  : Number ("if"),
    if_b_ (std::move (if_b)),
    then_n_ (std::move (then_n)),
    else_n_ (std::move (else_n))
{ }

NumberIf::NumberIf (const BlockModel& al)
  : Number (al),
    if_b_ (Librarian::build_item<Boolean> (al, "if")),
    then_n_ (Librarian::build_item<Number> (al, "then")),
    else_n_ (Librarian::build_item<Number> (al, "else"))
{ }

NumberIf::~NumberIf ()
{ }

static struct NumberIfSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberIf (al); }
  NumberIfSyntax ()
    : DeclareModel (Number::component, "if", "\
Select between two numbers depending on a boolean expression.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("if", Boolean::component, 
                          "Select which number to use.");
    frame.declare_object ("then", Number::component, 
                          "Use this if true.");
    frame.declare_object ("else", Number::component, 
                          "Use this if false.");
    frame.order ("if", "then", "else");
  }
} NumberIf_syntax;

// number_lisp.C ends here
