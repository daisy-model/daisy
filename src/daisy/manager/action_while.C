// action_while.C
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
#include "object_model/frame.h"
#include "daisy/output/log.h"
#include "util/assertion.h"
#include "util/memutils.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/block_model.h"

struct ActionWhile : public Action
{
  const std::vector<Action*> actions;

  void tick (const Daisy& daisy, const Scope& scope, Treelog& out)
  { 
    for (unsigned int i = 0; i < actions.size (); i++)
      actions[i]->tick (daisy, scope, out);
  }

  void doIt (Daisy& daisy, const Scope& scope, Treelog& out)
  { 
    for (unsigned int i = 0; i < actions.size (); i++)
      actions[i]->doIt (daisy, scope, out);
  }

  bool done (const Daisy& daisy, const Scope& scope, Treelog& msg) const
  {
    daisy_assert (actions.size () != 0U);
    return (actions[0]->done (daisy, scope, msg)); 
  }

  void output (Log& log) const
  { output_list (actions, "actions", log, Action::component); }

  void initialize (const Daisy& daisy, const Scope& scope, Treelog& out)
  { 
    for (unsigned int i = 0; i < actions.size (); i++)
      actions[i]->initialize (daisy, scope, out);
  }

  bool check (const Daisy& daisy, const Scope& scope, Treelog& err) const
  { 
    Treelog::Open nest (err, "while");
    bool ok = true;
    for (std::vector<Action*>::const_iterator i = actions.begin ();
	 i != actions.end ();
	 i++)
      {
	if (!(*i)->check (daisy, scope, err))
	  ok = false;
      }
    return ok;
  }

  ActionWhile (const BlockModel& al)
    : Action (al),
      actions (Librarian::build_vector<Action> (al, "actions"))
  { }

  ~ActionWhile ()
  { 
    sequence_delete (actions.begin (), actions.end ());
  }
};

static struct ActionWhileSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ActionWhile (al); }

  static bool check_alist (const Metalib&, const Frame& al, Treelog& err)
  {
    bool ok = true;

    if (al.value_size ("actions") < 1)
      {
	err.entry ("You must specify at least one action");
	ok = false;
      }
    return ok;
  }

  ActionWhileSyntax ()
    : DeclareModel (Action::component, "while", "\
Perform all the specified actions in the sequence listed, but in the\n\
same timestep.  The 'while' action is done when the first action in the\n\
list is done.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.add_check (check_alist);
    frame.declare_object ("actions", Action::component, 
                          Attribute::State, Attribute::Variable,
                          "List of actions to perform.");
    frame.order ("actions");
  }
} ActionWhile_syntax;

// action_while.C ends here.
