// reaction_std.C -- Transformation between two soil chemicals.
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

#include "daisy/chemicals/reaction.h"
#include "object_model/block_model.h"
#include "daisy/chemicals/transform.h"
#include "daisy/chemicals/chemistry.h"
#include "daisy/chemicals/chemical.h"
#include "daisy/soil/soil.h"
#include "daisy/output/log.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <memory>

struct ReactionStandard : public Reaction
{
  const Units& units;
  // Parameters.
  const symbol name_A;
  const symbol name_B;
  const std::unique_ptr<Transform> transform;
  
  // Output.
  std::vector<double> S_AB;
  void output (Log& log) const
  { output_variable (S_AB, log); }

  // Simulation.
  void tick_soil (const Geometry& geo,
                  const Soil& soil, const SoilWater& soil_water, 
                  const SoilHeat& soil_heat, const AWI&, OrganicMatter&,
                  Chemistry& chemistry, const double dt, Treelog& msg) // FIXME: dt not used
  {
    TREELOG_MODEL (msg);
    const size_t cell_size = soil.size ();
    Chemical& A = chemistry.find (name_A);
    Chemical& B = chemistry.find (name_B);
    std::vector<double> AM (cell_size);
    std::vector<double> BM (cell_size);
    for (size_t i = 0; i < cell_size; i++)
      {
	AM[i] = A.M_primary (i);
	BM[i] = B.M_primary (i);
      }
    transform->tick (units, geo, 
                     soil, soil_water, soil_heat, AM, BM, S_AB, msg);
    A.add_to_transform_sink (S_AB);
    B.add_to_transform_source (S_AB);
  }

  // Create.
  bool check (const Geometry& geo, 
              const Soil& soil, const SoilWater& soil_water,
	      const SoilHeat& soil_heat,
	      const OrganicMatter&, const Chemistry& chemistry,
	      Treelog& msg) const
  { 
    bool ok = true;
    if (!chemistry.know (name_A))
      {
        msg.error ("'" + name_A.name () + "' not traced");
        ok = false;
      }
    if (!chemistry.know (name_B))
      {
        msg.error ("'" + name_B.name () + "' not traced");
        ok = false;
      }
    if (!transform->check (units, geo, soil, soil_water, soil_heat, msg))
      ok = false;

    return ok;
  }
  void initialize (const Geometry& geo, 
                   const Soil& soil, const SoilWater& soil_water,
                   const SoilHeat& soil_heat, const OrganicMatter&,
		   const Chemistry&,
		   const Surface&, Treelog& msg)
  { 
    transform->initialize (units, geo, soil, soil_water, soil_heat, msg); 
    S_AB.insert (S_AB.begin (), soil.size (), 0.0);
    daisy_assert (S_AB.size () == soil.size ());
  }
  explicit ReactionStandard (const BlockModel& al)
    : Reaction (al),
      units (al.units ()),
      name_A (al.name ("A")),
      name_B (al.name ("B")),
      transform (Librarian::build_item<Transform> (al, "transform"))
  { }
};

static struct ReactionStandardSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new ReactionStandard (al); }
  ReactionStandardSyntax ()
    : DeclareModel (Reaction::component, "default", 
	       "Transformation between two soil chemicals.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("transform", Transform::component,
                       "Tranformation process between 'A' to 'B'.");
    frame.declare_string ("A", Attribute::Const,
		"Name of first soil component in equilibrium.");
    frame.declare_string ("B", Attribute::Const,
		"Name of second soil component in equilibrium.");
    frame.declare ("S_AB", "g/cm^3/h", Attribute::LogOnly, Attribute::SoilCells,
		"Converted from A to B this timestep (may be negative).");

  }
} ReactionStandard_syntax;
