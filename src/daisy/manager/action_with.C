// action_with.C --- restrict actions to a specific columns
// 
// Copyright 1996-2001 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001 KVL.
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

#include "daisy/manager/action.h"
#include "object_model/block_model.h"
#include "daisy/daisy.h"
#include "object_model/frame.h"
#include "daisy/field.h"
#include "daisy/output/log.h"
#include "util/memutils.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"

struct ActionWithColumn : public Action
{
  const symbol column;
  std::vector<Action*> actions;

public:
  void tick (const Daisy& daisy, const Scope& scope, Treelog& out)
  { 
    Field::Restrict restriction (daisy.field (), column);
    Treelog::Open nest (out, column);
    for (std::vector<Action*>::iterator i = actions.begin ();
	 i != actions.end ();
	 i++)
      {
	(*i)->tick (daisy, scope, out);
      }
  }

  void doIt (Daisy& daisy, const Scope& scope, Treelog& out)
  { 
    Field::Restrict restriction (daisy.field (), column);
    Treelog::Open nest (out, column);
    for (std::vector<Action*>::iterator i = actions.begin ();
	 i != actions.end ();
	 i++)
      {
	(*i)->doIt (daisy, scope, out);
      }
  }

  bool done (const Daisy& daisy, const Scope& scope, Treelog& out) const
  {
    bool all_done = true;
    Field::Restrict restriction (daisy.field (), column);
    Treelog::Open nest (out, column);
    for (std::vector<Action*>::const_iterator i = actions.begin ();
	 i != actions.end ();
	 i++)
      {
	if (!(*i)->done (daisy, scope, out))
	  all_done = false;
      }
    return all_done;
  }

  void output (Log& log) const
  { output_list (actions, "actions", log, Action::component); }

  void initialize (const Daisy& daisy, const Scope& scope, Treelog& out)
  { 
    Field::Restrict restriction (daisy.field (), column);
    Treelog::Open nest (out, column);
    for (std::vector<Action*>::iterator i = actions.begin ();
	 i != actions.end ();
	 i++)
      {
	(*i)->initialize (daisy, scope, out);
      }
  }

  bool check (const Daisy& daisy, const Scope& scope, Treelog& err) const
  { 
    Treelog::Open nest (err, std::string ("with") + column);
    bool ok = true;
    for (std::vector<Action*>::const_iterator i = actions.begin ();
	 i != actions.end ();
	 i++)
      {
	if (!(*i)->check (daisy, scope, err))
	  ok = false;
      }
    if (!daisy.field ().find (column))
      {
	err.entry (std::string ("No column '") + column + "'");
	ok = false;
      }
    return ok;
  }

  ActionWithColumn (const BlockModel& al)
    : Action (al),
      column (al.name ("column")),
      actions (Librarian::build_vector<Action> (al, "actions"))
  { }
public:
  ~ActionWithColumn ()
  { sequence_delete (actions.begin (), actions.end ()); }
};

static struct ActionWithColumnSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ActionWithColumn (al); }

  ActionWithColumnSyntax ()
    : DeclareModel (Action::component, "with-column", "\
Perform actions on a specific column.")
  { }
  void load_frame (Frame& frame) const
  { 
    frame.declare_string ("column", Attribute::Const, 
		"Name of column to perform actions on.");
    frame.declare_object ("actions", Action::component, 
                       Attribute::State, Attribute::Variable,
                       "Actions to perform on the specified column.");
    frame.order ("column", "actions");
  }
} ActionWithColumn_syntax;

// action_with.C ends here.
