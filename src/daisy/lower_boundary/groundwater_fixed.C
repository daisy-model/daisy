// groundwater_fixed.C --- Fixed, high groundwater level.
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
#include "object_model/block_model.h"
#include "object_model/check.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

class GroundwaterFixed : public Groundwater
{
  // Content.
private:
  const double depth;
  
  // Groundwater.
public:
  bottom_t bottom_type () const
  { return pressure; }
  double q_bottom (size_t) const
  { daisy_notreached (); }

  // Simulation.
public:
  void tick (const Geometry&,
             const Soil&, SoilWater&, double, 
	     const SoilHeat&, const Time&, const Scope&, Treelog&)
  { }
  double table () const
  { return depth; }

  // Create and Destroy.
public:
  void initialize (const Geometry&, const Time&, const Scope&, Treelog&)
  { }
  bool check (const Geometry&, const Scope&, Treelog&) const
  { return true; }
  GroundwaterFixed (const BlockModel& al)
    : Groundwater (al),
      depth (al.number ("table"))
  { }
  ~GroundwaterFixed ()
  { }
};

static struct GroundwaterFixedSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new GroundwaterFixed (al); }
  GroundwaterFixedSyntax ()
    : DeclareModel (Groundwater::component, "fixed", "common", "\
Fixed high groundwater level.")
  { }
  void load_frame (Frame& frame) const
  { 
    frame.declare ("table", "cm", Check::none (), Attribute::Const,
		"Groundwater level (negative number below surface).");
    frame.order ("table");
  }
} GroundwaterFixed_syntax;


