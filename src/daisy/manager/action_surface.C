// action_surface.C
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
#include "daisy/field.h"
#include "object_model/check.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"

struct ActionSetSurfaceDetentionCapacity : public Action
{
  // Content.
  const double height;

  // Simulation.
  void doIt (Daisy& daisy, const Scope&, Treelog& out)
  {
    out.message ("Adjusting surface detention capacity");
    daisy.field ().set_surface_detention_capacity (height);
  }

  void tick (const Daisy&, const Scope&, Treelog&)
  { }
  void initialize (const Daisy&, const Scope&, Treelog&)
  { }
  bool check (const Daisy&, const Scope&, Treelog& err) const
  { return true; }

  ActionSetSurfaceDetentionCapacity (const BlockModel& al)
    : Action (al),
      height (al.number ("height"))
  { }
};

static struct ActionSurfaceSyntax : DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ActionSetSurfaceDetentionCapacity (al); }
  
  ActionSurfaceSyntax ()
    : DeclareModel (Action::component, "set_surface_detention_capacity",  "\
Set amount of ponding the surface can retain.")
  { }
  void load_frame (Frame& frame) const
  { 
    frame.declare ("height", "mm", Check::non_negative (), Attribute::Const,
		"Max ponding height before runoff.");
    frame.order ("height");
  }
} ActionSurface_syntax;

// action_surface.C ends here.
