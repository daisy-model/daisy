// action_stop.C
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
#include "daisy/daisy.h"
#include "object_model/librarian.h"

struct ActionStop : public Action
{
  void doIt (Daisy& daisy, const Scope&, Treelog&)
  { daisy.stop (); }

  void tick (const Daisy&, const Scope&, Treelog&)
  { }
  void initialize (const Daisy&, const Scope&, Treelog&)
  { }
  bool check (const Daisy&, const Scope&, Treelog& err) const
  { return true; }

  ActionStop (const BlockModel& al)
    : Action (al)
    { }
};

static struct ActionStopSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ActionStop (al); }
  ActionStopSyntax ()
    : DeclareModel  (Action::component, "stop", "Stop the simulation.")
  { }
  void load_frame (Frame&) const
  { }
} ActionStop_syntax;

// action_stop.C ends here.
