// bound.C --- Specify interval boundary.
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

#include "daisy/soil/transport/bound.h"
#include "object_model/block_model.h"
#include "util/mathlib.h"
#include "object_model/frame.h"
#include "object_model/librarian.h"
#include "daisy/output/log.h"
#include "object_model/vcheck.h"
#include <sstream>
#include <map>

// bound component.

const char *const Bound::component = "bound";

struct BoundInit : public DeclareComponent 
{
  BoundInit ()
    : DeclareComponent (Bound::component, "\
Specify one end of an interval boundary.")
  { }
};

symbol
Bound::library_id () const
{
  static const symbol id (component);
  return id;
}

std::string
Bound::describe () const
{
  std::ostringstream tmp;
  switch (type ())
    {
    case none:
      tmp << "none";
      break;
    case full:
      tmp << "full";
      break;
    case finite:
      tmp << value ();
      break;
    }
  return tmp.str ();
}

Bound::type_t 
Bound::symbol2type (const symbol s)
{
  static struct sym_set_t : std::map<symbol, type_t>
  {
    sym_set_t ()
    {
      insert (std::pair<symbol,type_t> ("finite", finite));
      insert (std::pair<symbol,type_t> ("full", full));
      insert (std::pair<symbol,type_t> ("none", none));
    } 
  } sym_set;
  sym_set_t::const_iterator i = sym_set.find (s);
  daisy_assert (i != sym_set.end ());
  return (*i).second;
}  

symbol 
Bound::type2symbol (const type_t t)
{
  static struct sym_set_t : std::map<type_t, symbol>
  {
    sym_set_t ()
    {
      insert (std::pair<type_t,symbol> (finite, "finite"));
      insert (std::pair<type_t,symbol> (full, "full"));
      insert (std::pair<type_t,symbol> (none, "none"));
    } 
  } sym_set;
  sym_set_t::const_iterator i = sym_set.find (t);
  daisy_assert (i != sym_set.end ());
  return (*i).second;
}

double 
Bound::value () const
{
  daisy_assert (type_ == finite);
  return value_;
}

void 
Bound::set_finite (const double value)
{
  type_ = finite;
  value_ = value;
}

void 
Bound::set_none ()
{
  type_ = none;
  value_ = -43.43e43;
}

void 
Bound::set_full ()
{
  type_ = full;
  value_ = 68.68e68;
}
void 
Bound::output (Log& log) const
{
  output_value (type2symbol (type_), "type", log);
  if (type () == finite)
    output_value (value_, "bound", log);
}

Bound::Bound (const BlockModel& al, const type_t type, const double value)
  : ModelDerived ("state"),
    type_ (type),
    value_ (value)
{ }

Bound::Bound (const char *const, const type_t type, const double value)
  : ModelDerived ("state"),
    type_ (type),
    value_ (value)
{ }

Bound::~Bound ()
{ }

// "none" model.
struct BoundNoneSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Bound (al, Bound::none, -42.42e42); }
  BoundNoneSyntax ()
    : DeclareModel (Bound::component, "none", "No boundary specified.")
  { }
  void load_frame (Frame& frame) const
  { }
};

// "full" model.
struct BoundFullSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Bound (al, Bound::full, 69.69e69); }
  BoundFullSyntax ()
    : DeclareModel (Bound::component, "full", "\
Maximum value for the interval boundary.")
  { }
  void load_frame (Frame& frame) const
  { }
};

// finite model.
struct BoundFiniteSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Bound (al, Bound::finite, al.number ("bound")); }
  BoundFiniteSyntax ()
    : DeclareModel (Bound::component, "finite", "Finite interval bound.")
  { }
  void load_frame (Frame& frame) const
  {
    
    frame.declare ("bound", "cm", Attribute::Const, "Interval bound to use.");
    frame.order ("bound");

  }
};

// state model.
struct BoundStateSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new Bound (al, Bound::symbol2type (al.name ("type")), 
                      al.number ("bound", -42.42e42)); }
  BoundStateSyntax ()
    : DeclareModel (Bound::component, "state", "Bound used for checkpoints.")
  { }
  static bool check_alist (const Metalib&, const Frame& frame, Treelog& msg)
  {
    bool ok = true;
    const bool is_finite 
      = Bound::symbol2type (frame.name ("type")) == Bound::finite;
    
    if (is_finite && !frame.check ("bound"))
      {
        msg.error ("'bound' should be set for the 'finite' type");
        ok = false;
      }
    else if (!is_finite && frame.check ("bound"))
      msg.warning ("'bound' will be ignored");

    return ok;
  }
  void load_frame (Frame& frame) const
  {
    frame.declare_string ("type", Attribute::State, "Bound type");
    static VCheck::Enum type_check ("finite", "full", "none");
    frame.set_check ("type", type_check);
    frame.declare ("bound", "cm", Attribute::OptionalState, "\
Interval bound to use.  Only valid for the 'finite' type.");
  }
};

struct BoundEmptySyntax : public DeclareParam
{
  BoundEmptySyntax ()
    : DeclareParam (Bound::component, "empty", "state", "\
A 'state' model set to 'none.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.set ("type", Bound::type2symbol (Bound::none)); 
  }
};

void
register_bound_models ()
{
  static BoundInit Bound_init;
  static BoundNoneSyntax BoundNone_syntax;
  static BoundFullSyntax BoundFull_syntax;
  static BoundFiniteSyntax BoundFinite_syntax;
  static BoundStateSyntax BoundState_syntax;
  static BoundEmptySyntax BoundEmpty_syntax;
}
// bound.C ends here.
