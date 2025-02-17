// groundwater_lysimeter.C
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

#include "daisy/lower_boundary/groundwater.h"
#include "daisy/soil/transport/geometry.h"
#include "util/assertion.h"
#include "object_model/librarian.h"

class GroundwaterLysimeter : public Groundwater
{
  // Content.
  double location;                // Location of lysimeter.

  // Groundwater.
public:
  bottom_t bottom_type () const
  { return lysimeter; }
  double q_bottom (size_t) const
  { daisy_notreached (); }
  bool is_lysimeter () const
  { return true; }

  // Simulation.
public:
  void tick (const Geometry&, const Soil&, SoilWater&, double, 
	     const SoilHeat&, const Time&, const Scope&, Treelog&)
  { }
  double table () const
  { return location; }

  // Create and Destroy.
public:
  void initialize (const Geometry& geo, const Time&,
                   const Scope&, Treelog&)
  { location = geo.bottom (); }
  bool check (const Geometry&, const Scope&, Treelog&) const
  { return true; }

  GroundwaterLysimeter (const BlockModel& al)
    : Groundwater (al),
      location (-42.42e42)
  { }
  ~GroundwaterLysimeter ()
  { }
};

static struct GroundwaterLysimeterSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new GroundwaterLysimeter (al); }

  GroundwaterLysimeterSyntax ()
    : DeclareModel (Groundwater::component, "lysimeter", "common", "\
Lysimeter bottom.")
  { }
  void load_frame (Frame&) const
  { }
} GroundwaterLysimeter_syntax;


