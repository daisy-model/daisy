// domsorp_std.C -- By default, we transform with a single SOM pool.
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

#include "daisy/organic_matter/domsorp.h"
#include "object_model/block_model.h"
#include "daisy/chemicals/transform.h"
#include "daisy/organic_matter/dom.h"
#include "daisy/organic_matter/som.h"
#include "daisy/soil/soil.h"
#include "daisy/output/log.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <memory>

struct DomsorpStandard : public Domsorp
{
  // Parameters.
  const size_t dom_pool;
  const size_t som_pool;
  const std::unique_ptr<Transform> transform;
  
  // Output.
  std::vector<double> S_C;
  std::vector<double> S_N;
  void output (Log& log) const
  { 
    output_variable (S_C, log); 
    output_variable (S_N, log); 
  }

  // Simulation.
  void tick (const Units& units, const Geometry& geo, const Soil& soil, 
             const SoilWater& soil_water, const SoilHeat& soil_heat,
             const std::vector<DOM*>& dom, const std::vector<SOM*>& som, 
             Treelog& msg)
  { 
    daisy_assert (dom.size () > dom_pool);
    daisy_assert (som.size () > som_pool);
    DOM& d = *dom[dom_pool];
    SOM& s = *som[som_pool];
    std::vector<double> dC;
    for (size_t i = 0; i < soil.size (); i++)
      dC.push_back (d.C_at (i));
    daisy_assert (s.C.size () == soil.size ());
    transform->tick (units, geo, soil, soil_water, soil_heat, dC, s.C, S_C, msg);

    for (size_t i = 0; i < soil.size (); i++)
      {
        if (S_C[i] > 1e-100)    // DOM -> SOM
          {
            const double fraction = S_C[i] / d.C_at (i);
            S_N[i] = fraction * d.N_at (i);
          }
        else if (S_C[i] < -1e-100) // SOM -> DOM
          {
            const double fraction = S_C[i] / s.C[i];
            S_N[i] = fraction * s.N[i];
          }
        else
          S_C[i] = S_N[i] = 0.0;

        d.add_to_source (i, -S_C[i], -S_N[i]);
        s.C[i] += S_C[i];
        s.N[i] += S_N[i];
      }
  }

  // Create.
  bool check (const Units& units, const Geometry& geo, const Soil& soil, 
              const SoilWater& soil_water, const SoilHeat& soil_heat,
              const size_t dom_size, const size_t som_size,
              Treelog& msg) const
  { 
    TREELOG_MODEL (msg);
    bool ok = true;
    {
      Treelog::Open nest (msg, "transform");
      if (!transform->check (units, geo, soil, soil_water, soil_heat, msg))
        ok = false; 
    }
    if (dom_pool >= dom_size)
      { 
        msg.error ("'dom_pool' too high");
        ok = false;
      }
    if (som_pool >= som_size)
      { 
        msg.error ("'som_pool' too high");
        ok = false;
      }
    return ok;
  }
  void initialize (const Units& units, const Geometry& geo,
                   const Soil& soil,  const SoilWater& soil_water, 
                   const SoilHeat& soil_heat, Treelog& msg)
  { 
    transform->initialize (units, geo, soil, soil_water, soil_heat, msg); 
    S_C.insert (S_C.begin (), soil.size (), 0.0);
    daisy_assert (S_C.size () == soil.size ());
    S_N.insert (S_N.begin (), soil.size (), 0.0);
    daisy_assert (S_N.size () == soil.size ());
  }
  DomsorpStandard (const BlockModel& al)
    : Domsorp (al),
      dom_pool (al.integer ("dom_pool") - 1),
      som_pool (al.integer ("som_pool") - 1),
      transform (Librarian::build_item<Transform> (al, "transform"))
  { }
};

static struct DomsorpStandardSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new DomsorpStandard (al); }
  DomsorpStandardSyntax ()
    : DeclareModel (Domsorp::component, "default", 
                    "Transformation between two soil chemicals.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare_object ("transform", Transform::component,
                          "Tranformation process between DOM and SOM.");
    frame.declare_integer ("dom_pool", Attribute::Const,
                   "Number of the DOM pool affected by the transformation.");
    frame.declare_integer ("som_pool", Attribute::Const,
                   "Number of the SOM pool affected by the transformation.");
    frame.declare ("S_C", "g C/cm^3/h", Attribute::LogOnly, Attribute::SoilCells,
                   "Carbon converted from DOM to SOM (may be negative).");
    frame.declare ("S_N", "g N/cm^3/h", Attribute::LogOnly, Attribute::SoilCells,
                   "Carbon converted from DOM to SOM (may be negative).");

  }
} DomsorpStandard_syntax;
