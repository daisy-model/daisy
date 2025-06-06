// clayom.C --- The effect of clay on organic matter.
// 
// Copyright 2002 KVL and Per Abrahamsen.
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

#include "daisy/organic_matter/clayom.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"

const char *const ClayOM::component = "ClayOM";

symbol
ClayOM::library_id () const
{
  static const symbol id (component);
  return id;
}

ClayOM::ClayOM (const BlockModel& al)
  : name (al.type_name ())
{ }

ClayOM::~ClayOM ()
{ }

static struct ClayOMInit : public DeclareComponent 
{
  void load_frame (Frame& frame) const
  {
    Model::load_model (frame);
  }
  ClayOMInit ()
    : DeclareComponent (ClayOM::component, "\
Find the effect of clay on organic matter processing.")
  { }
} ClayOM_init;

// clayom.C ends here.
