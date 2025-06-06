// column.C
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

#include "daisy/column.h"
#include "object_model/block_model.h"
#include "daisy/output/log.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"
#include "object_model/submodeler.h"
#include "object_model/check.h"
#include "util/point.h"
#include <map>

const char *const Column::component = "column";

symbol
Column::library_id () const
{
  static const symbol id (component);
  return id;
}

const std::vector<const XYPoint*>&
Column::location () const
{ return location_; }

void
Column::output (Log& log) const
{
  output_variable (area, log);
}

Column::Column (const BlockModel& al)
  : ModelFramed (al),
    area (al.number ("area")),
    location_ (map_submodel_const<XYPoint> (al, "location"))
{ }

Column::~Column ()
{ }

static struct ColumnInit : public DeclareComponent
{
  void load_frame (Frame& frame) const
  { 
    Model::load_model (frame);
    frame.declare ("area", "m^2", Check::positive (), Attribute::State, "\
Area covered by this column.\n\
When logging multiple columns, the values are weighted by relative area.");
    frame.set ("area", 1.0);

    frame.declare_submodule_sequence ("location", Attribute::Const, "\
Location of this column.\n\
\n\
The meaning depends on the number of point in the sequence.\n\
0 points: The column has no specific location.\n\
1 point: The column has a location, but no specific area.\n\
3 or more points: The column represents the area specified by a\n\
polygon with the specified corner points.", XYPoint::load_syntax);
    frame.set_empty ("location");
  }
  ColumnInit ()
    : DeclareComponent (Column::component, "\
A 'column' is an one-dimensional vertical description of the\n\
soil/crop/atmosphere system.  The column component contains most of\n\
the other processes in Daisy as submodels.")
  { }
} Column_init;

// column.C ends here.
