// root_system.C -- Root development and uptake.
// 
// Copyright 1996-2001 Per Abrahamsen and SÃ¸ren Hansen
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

#include "daisy/crop/root/root_system.h"
#include "object_model/block_model.h"
#include "object_model/metalib.h"
#include "object_model/librarian.h"

// The 'root' component.

const char *const RootSystem::component = "root";

symbol
RootSystem::library_id () const
{
  static const symbol id (component);
  return id;
}

RootSystem::RootSystem (const BlockModel& al)
  : ModelDerived (al.type_name ())
{ }

RootSystem::~RootSystem ()
{ }

static struct RootSystemInit : public DeclareComponent 
{
  static bool
  check_alist (const Metalib&, const Frame& al, Treelog& msg)
  {
    bool ok = true;
    return ok;
  }

  void load_frame (Frame&) const
  { }

  RootSystemInit ()
    : DeclareComponent (RootSystem::component, "\
Root processes.")
  { }
} RootSystem_init;

// root_system.C ends here.
