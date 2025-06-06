// program.C --- Run a program.
// 
// Copyright 2004 Per Abrahamsen and KVL.
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

#include "programs/program.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"

const char *const Program::component = "program";

symbol
Program::library_id () const
{
  static const symbol id (component);
  return id;
}

void
Program::attach_ui (Run *const run, const std::vector<Log*>& l)
{
  ui = run; 
  logs = l;
}

void
Program::propagate_ui (Program *const child)
{ child->attach_ui (ui, logs); }

bool 
Program::ui_running () const
{ return !ui || ui->running (); }

void
Program::ui_set_progress (const double value)
{ 
  if (!ui)
    return;

  ui->set_progress (value); 
}

const std::vector<const Scope*>& 
Program::scopes () const
{
  static const std::vector<const Scope*> empty;
  return empty;
}

Program::Program (const BlockModel& al)
  : objid (al.type_name ()),
    ui (NULL)
{ }

Program::~Program ()
{ }

static struct ProgramInit : public DeclareComponent
{
  void load_frame (Frame& frame) const
  { Model::load_model (frame); }
  ProgramInit ()
    : DeclareComponent (Program::component, "\
Run a program.")
  { }
} Program_init;

// program.C ends here.
