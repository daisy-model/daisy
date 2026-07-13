// number_const.C -- Simple number objects.
// 
// Copyright 2004, 2005 Per Abrahamsen and KVL.
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
#include "object_model/block_model.h"
#include "util/scope.h"
#include "object_model/units.h"
#include "object_model/unit.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/library.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <sstream>

void
NumberConst::tick (const Units&, const Scope&, Treelog&)
{ }

bool
NumberConst::missing (const Scope&) const
{ return false; }

double
NumberConst::value (const Scope&) const
{ return val_; }

symbol
NumberConst::dimension (const Scope&) const
{ return unit_.native_name (); }

const Unit&
NumberConst::unit () const
{ return unit_; }

bool
NumberConst::initialize (const Units&, const Scope&, Treelog&)
{ return true; }

bool
NumberConst::check (const Units& units, const Scope&, Treelog& msg) const
{
  bool ok = true;
  if (units.is_error (unit_))
    {
      msg.error ("Bad unit");
      ok = false;
    }
  return ok;
}

NumberConst::NumberConst (const double value, const Unit& unit)
  : Number ("const"),
    val_ (value),
    unit_ (unit)
{ }

NumberConst::NumberConst (const BlockModel& al)
  : Number (al),
    val_ (al.number ("value")),
    unit_ (al.units ().get_unit (al.name ("value")))
{
  if (al.units ().is_error (unit_))
    al.msg ().warning ("Unknown unit '" + al.name ("value") + "'");
}

NumberConst::NumberConst (const BlockModel& al, const symbol key)
  : Number (al),
    val_ (al.number (key)),
    unit_ (al.units ().get_unit (al.find_frame (key).dimension (key)))
{
  if (al.units ().is_error (unit_))
    al.msg ().warning ("Unknown unit '"
                       + al.find_frame (key).dimension (key) + "'");
}

static struct NumberConstSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberConst (al); }
  NumberConstSyntax ()
    : DeclareModel (Number::component, "const", 
	       "Always give the specified value.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare ("value", Attribute::User (), Attribute::Const,
		"Fixed value for this number.");
    frame.order ("value");
  }
} NumberConst_syntax;

// The 'x' model.

const symbol NumberX::name_ ("x");

symbol
NumberX::title () const
{ return name_; }

void
NumberX::tick (const Units&, const Scope&, Treelog&)
{ }

symbol
NumberX::dimension (const Scope& scope) const
{
  return scope.dimension (name_);
}

bool
NumberX::missing (const Scope& scope) const
{ return !scope.check (name_); }

double
NumberX::value (const Scope& scope) const
{ return scope.number (name_); }

bool
NumberX::initialize (const Units&, const Scope& scope, Treelog& msg)
{
  if (scope.lookup (name_) != Attribute::Number)
    {
      msg.error ("'" + name_ + "' is not a number");
      return false;
    }
  return true;
}

bool
NumberX::check (const Units&, const Scope& scope, Treelog& msg) const
{
  bool ok = true;
  if (scope.lookup (name_) != Attribute::Number)
    {
      msg.error ("'x' is not a number");
      ok = false;
    }
  return ok;
}

NumberX::NumberX ()
  : Number ("x")
{ }

NumberX::NumberX (const BlockModel& al)
  : Number (al)
{ }

static struct NumberXSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberX (al); }
  NumberXSyntax ()
    : DeclareModel (Number::component, "x", "\
The value of the symbol 'x' in the current scope.")
  { }
  void load_frame (Frame& frame) const
  { }
} NumberX_syntax;

symbol
NumberGet::title () const
{ return name_; }

void
NumberGet::tick (const Units&, const Scope&, Treelog&)
{ }

symbol
NumberGet::dimension (const Scope&) const
{ return unit_.native_name (); }

const Unit&
NumberGet::unit () const
{ return unit_; }

bool
NumberGet::missing (const Scope& scope) const
{ return !scope.check (name_); }

double
NumberGet::value (const Scope& scope) const
{
  daisy_assert (scope.check (name_));
  daisy_assert (scope_unit_);
  const double value = scope.number (name_);
  return Units::unit_convert (*scope_unit_, unit (), value);
}

bool
NumberGet::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  if (scope.lookup (name_) != Attribute::Number)
    {
      msg.error ("'" + name_ + "' is not a number");
      return false;
    }
  const symbol got_dim = scope.dimension (name_);
  scope_unit_ = &units.get_unit (got_dim);
  return true;
}

bool
NumberGet::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  Treelog::Open nest (msg, name_);

  bool ok = true;
  if (units.is_error (unit_))
    {
      msg.error ("Bad unit");
      ok = false;
    }
  if (!scope_unit_)
    {
      msg.error ("'" + name_ + "' is not a number");
      ok = false;
    }
  else if (units.is_error (*scope_unit_))
    {
      daisy_assert (scope.lookup (name_) == Attribute::Number);
      const symbol got_dim = scope.dimension (name_);
      msg.error ("'" + name_ + "' has unknown dimension [" + got_dim + "]");
      ok = false;
    }
  return ok;
}

NumberGet::NumberGet (const symbol name, const Unit& unit)
  : Number ("get"),
    unit_ (unit),
    scope_unit_ (NULL),
    name_ (name)
{ }

NumberGet::NumberGet (const BlockModel& al)
  : Number (al),
    unit_ (al.units ().get_unit (al.name ("dimension"))),
    scope_unit_ (NULL),
    name_ (al.name ("name"))
{
  if (al.units ().is_error (unit_))
    al.msg ().warning ("Unknown unit '" + al.name ("dimension") + "'");
}

NumberGet::NumberGet (const BlockModel& al, const symbol key)
  : Number (al),
    unit_ (al.units ().get_unit (al.name ("dimension"))),
    scope_unit_ (NULL),
    name_ (key)
{ }

static struct NumberGetSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberGet (al); }
  NumberGetSyntax ()
    : DeclareModel (Number::component, "get", 
	       "Get the value of symbol in the current scope.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_string ("name", Attribute::Const, 
                "Name of a the symbol.");
    frame.declare_string ("dimension", Attribute::Const, 
                "Expected dimension for the symbol.");
    frame.order ("name", "dimension");
  }
} NumberGet_syntax;

symbol
NumberFetchGet::title () const
{ return name_; }

void
NumberFetchGet::tick (const Units&, const Scope&, Treelog&)
{ }

symbol
NumberFetchGet::dimension (const Scope&) const
{
  if (scope_unit_)
    return scope_unit_->native_name ();

  return Attribute::Unknown ();
}

const Unit&
NumberFetchGet::unit () const
{
  daisy_assert (scope_unit_);
  return *scope_unit_;
}

bool
NumberFetchGet::missing (const Scope& scope) const
{ return !scope.check (name_); }

double
NumberFetchGet::value (const Scope& scope) const
{
  daisy_assert (scope.check (name_));
  return scope.number (name_);
}

bool
NumberFetchGet::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  if (scope.lookup (name_) != Attribute::Number)
    {
      msg.error ("'" + name_ + "' is not a number");
      return false;
    }
  const symbol got_dim = scope.dimension (name_);
  scope_unit_ = &units.get_unit (got_dim);
  return true;
}

bool
NumberFetchGet::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  Treelog::Open nest (msg, name_);

  bool ok = true;
  if (!scope_unit_)
    {
      msg.error ("'" + name_ + "' is not a number");
      ok = false;
    }
  else if (units.is_error (*scope_unit_))
    {
      daisy_assert (scope.lookup (name_) == Attribute::Number);
      const symbol got_dim = scope.dimension (name_);
      msg.error ("'" + name_ + "' has unknown dimension [" + got_dim + "]");
      ok = false;
    }
  return ok;
}

NumberFetchGet::NumberFetchGet (const symbol name)
  : Number ("fetch"),
    scope_unit_ (NULL),
    name_ (name)
{ }

NumberFetchGet::NumberFetchGet (const BlockModel& al, const symbol key)
  : Number (al),
    scope_unit_ (NULL),
    name_ (key)
{ }

symbol
NumberFetch::title () const
{ return child_->title (); }

std::unique_ptr<Number>
NumberFetch::fetch_child (const BlockModel& al, const symbol key)
{
  std::unique_ptr<Number> result;
  Attribute::type type = al.lookup (key);
  switch (type)
    {
    case Attribute::Number:
      {
        if (!al.check (key))
          {
            const Frame& frame = al.find_frame (key);
            daisy_assert (frame.lookup (key) == Attribute::Number);
            al.error ("Parameter '" + key
                      + "' is declared in '" + frame.type_name ()
                      + "' (" + frame.description ()
                      + ") base '" + frame.base_name ()
                      + "', but has no value");
            break;
          }
        if (al.type_size (key) != Attribute::Singleton)
          {
            al.error ("Parameter '" + key
                      + "' is a sequence, expected singleton");
            break;
          }
        result.reset (new NumberConst (al, key));
      }
      break;
    case Attribute::Model:
      {
        const Frame& frame = al.find_frame (key);
        const symbol component = frame.component (key);
        if (component != Number::component)
          {
            al.error ("'" + key + "' is a '" + component
                      + "' model, expected a '"
                      + Number::component + "'");
            break;
          }
        if (frame.type_size (key) != Attribute::Singleton)
          {
            al.error ("Parameter '" + key
                      + "' is a model sequence, expected singleton");
            break;
          }
        if (!frame.check (key))
          {
            al.error ("'" + key + "' declared, but has no value");
            break;
          }
        if (!frame.check (al))
          break;
        result.reset (Librarian::build_item<Number> (al, key));
      }
      break;
    case Attribute::Error:
      result.reset (new NumberFetchGet (al, key));
      break;
    default:
      al.error ("'" + key + "' is a " + Attribute::type_name (type)
                + ", expected a number");
    }
  return result;
}

void
NumberFetch::tick (const Units& units, const Scope& scope, Treelog& msg)
{ child_->tick (units, scope, msg); }

bool
NumberFetch::missing (const Scope& scope) const
{ return child_->missing (scope); }

double
NumberFetch::value (const Scope& scope) const
{ return child_->value (scope); }

symbol
NumberFetch::dimension (const Scope& scope) const
{ return child_->dimension (scope); }

bool
NumberFetch::initialize (const Units& units, const Scope& scope, Treelog& msg)
{
  if (!child_.get ())
    return false;
  return child_->initialize (units, scope, msg);
}

bool
NumberFetch::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);

  bool ok = true;
  if (!child_.get ())
    {
      msg.error ("Fetch failed");
      ok = false;
    }
  else if (!child_->check (units, scope, msg))
    ok = false;
  return ok;
}

NumberFetch::NumberFetch (std::unique_ptr<Number> child)
  : Number ("fetch"),
    child_ (std::move (child))
{ }

NumberFetch::NumberFetch (const BlockModel& al)
  : Number (al),
    child_ (fetch_child (al, al.name ("name")))
{ }

static struct NumberFetchSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberFetch (al); }
  NumberFetchSyntax ()
    : DeclareModel (Number::component, "fetch", 
	       "Fetch the value and dimension in the current scope.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_string ("name", Attribute::Const, 
                "Name of a the symbol.");
    frame.order ("name");
  }
} NumberFetch_syntax;

NumberChild::NumberChild (const symbol objid, std::unique_ptr<Number> child)
  : Number (objid),
    child_ (std::move (child))
{ }

void
NumberChild::tick (const Units& units, const Scope& scope, Treelog& msg)
{ child_->tick (units, scope, msg); }

bool
NumberChild::initialize (const Units& units, const Scope& scope, Treelog& msg)
{ return child_->initialize (units, scope, msg); }

NumberChild::NumberChild (const BlockModel& al)
  : Number (al),
    child_ (Librarian::build_item<Number> (al, "value"))
{ }

static struct NumberChildSyntax : public DeclareBase
{
  NumberChildSyntax ()
    : DeclareBase (Number::component, "child", "\
Numbers based on another number.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("value", Number::component,
                       "Operand for this function.");
  }
} NumberChild_syntax;

bool
NumberIdentity::missing (const Scope& scope) const
{
  return child_->missing (scope)
    || (known (dim_) && known (child_->dimension (scope))
        && !units_.can_convert (child_->dimension (scope), dim_,
                                child_->value (scope)));
}

double
NumberIdentity::value (const Scope& scope) const
{
  const double v = child_->value (scope);
  if (known (dim_) && known (child_->dimension (scope)))
    return units_.convert (child_->dimension (scope), dim_, v);
  return v;
}

symbol
NumberIdentity::dimension (const Scope& scope) const
{
  if (known (dim_))
    return dim_;
  return child_->dimension (scope);
}

bool
NumberIdentity::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  bool ok = true;

  if (!child_->check (units, scope, msg))
    ok = false;

  if (known (dim_) && known (child_->dimension (scope))
      && !units.can_convert (child_->dimension (scope), dim_))
    {
      msg.error ("Cannot convert [" + child_->dimension (scope)
                 + "] to [" + dim_ + "]");
      ok = false;
    }
  return ok;
}

NumberIdentity::NumberIdentity (std::unique_ptr<Number> child, const Units& units)
  : NumberIdentity (std::move (child), units, Attribute::Unknown ())
{ }

NumberIdentity::NumberIdentity (std::unique_ptr<Number> child,
                                const Units& units,
                                const symbol dimension)
  : NumberChild ("identity", std::move (child)),
    units_ (units),
    dim_ (dimension)
{ }

NumberIdentity::NumberIdentity (const BlockModel& al)
  : NumberChild (al),
    units_ (al.units ()),
    dim_ (al.name ("dimension", Attribute::Unknown ()))
{ }

static struct NumberIdentitySyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberIdentity (al); }
  NumberIdentitySyntax ()
    : DeclareModel (Number::component, "identity", "child", "\
Pass value unchanged.")
  { }
  void load_frame (Frame& frame) const
  {
    Model::load_model (frame);
    frame.declare_string ("dimension", Attribute::OptionalConst,
		"Dimension of this value.");
  }
} NumberIdentity_syntax;

bool
NumberConvert::missing (const Scope& scope) const
{
  return child_->missing (scope)
    || !units_.can_convert (child_->dimension (scope), dim_,
                            child_->value (scope));
}

double
NumberConvert::value (const Scope& scope) const
{
  const double v = child_->value (scope);
  return units_.convert (child_->dimension (scope), dim_, v);
}

symbol
NumberConvert::dimension (const Scope&) const
{ return dim_; }

bool
NumberConvert::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  bool ok = true;

  if (!child_->check (units, scope, msg))
    ok = false;

  if (!units.can_convert (child_->dimension (scope), dim_))
    {
      msg.error ("Cannot convert [" + child_->dimension (scope)
                 + "] to [" + dim_ + "]");
      ok = false;
    }
  return ok;
}

NumberConvert::NumberConvert (std::unique_ptr<Number> child,
                              const Units& units,
                              const symbol dimension)
  : NumberChild ("convert", std::move (child)),
    units_ (units),
    dim_ (dimension)
{ }

NumberConvert::NumberConvert (const BlockModel& al)
  : NumberChild (al),
    units_ (al.units ()),
    dim_ (al.name ("dimension"))
{ }

static struct NumberConvertSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberConvert (al); }
  NumberConvertSyntax ()
    : DeclareModel (Number::component, "convert", "child", "\
Convert to specified dimension.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare_string ("dimension", Attribute::Const,
		"Dimension to convert to.");
    frame.order ("value", "dimension");
  }
} NumberConvert_syntax;

bool
NumberDim::missing (const Scope& scope) const
{ return child_->missing (scope); }

double
NumberDim::value (const Scope& scope) const
{ return child_->value (scope); }

symbol
NumberDim::dimension (const Scope&) const
{ return dim_; }

bool
NumberDim::check (const Units& units, const Scope& scope, Treelog& msg) const
{
  TREELOG_MODEL (msg);
  bool ok = true;

  if (!child_->check (units, scope, msg))
    ok = false;

  if (warn_known_ && known (child_->dimension (scope))
      && child_->dimension (scope) != dim_)
    msg.warning ("Dimension for child [" + child_->dimension (scope)
                 + "] already known, now asserting it is [" + dim_ + "]");

  return ok;
}

NumberDim::NumberDim (std::unique_ptr<Number> child,
                      const symbol dimension,
                      const bool warn_known)
  : NumberChild ("dim", std::move (child)),
    dim_ (dimension),
    warn_known_ (warn_known)
{ }

NumberDim::NumberDim (const BlockModel& al)
  : NumberChild (al),
    dim_ (al.name ("dimension")),
    warn_known_ (al.flag ("warn_known"))
{ }

static struct NumberDimSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new NumberDim (al); }
  NumberDimSyntax ()
    : DeclareModel (Number::component, "dim", "child", "\
Specify dimension for number.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_boolean ("warn_known", Attribute::Const,
                "Issue a warning if the dimensions is already known.");
    frame.set ("warn_known", true);
    frame.declare_string ("dimension", Attribute::Const,
		"Dimension to use.");
    frame.order ("value", "dimension");
  }
} NumberDim_syntax;

// number_const.C ends here.
