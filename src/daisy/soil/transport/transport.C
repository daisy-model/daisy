// transport.C -- Solute transport in primary domain.
// 
// Copyright 2006, 2008 Per Abrahamsen and KVL.
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

#include "daisy/soil/transport/transport.h"
#include "daisy/chemicals/chemical.h"
#include "daisy/organic_matter/doe.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/chemicals/adsorption.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "util/assertion.h"
#include "daisy/soil/soil_water.h"
#include "daisy/soil/soil.h"

const char *const Transport::component = "transport";

symbol 
Transport::library_id () const
{
  static const symbol id (component);
  return id;
}

void 
Transport::element (const Geometry& geo, 
                       const Soil& soil, const SoilWater& soil_water,
                       DOE& element, const double diffusion_coefficient, 
                       const double dt, Treelog& msg)
{
 // Cells.
  const size_t cell_size = geo.cell_size ();
  std::vector<double> Theta_old (cell_size); // Water content at start...
  std::vector<double> Theta_new (cell_size); // ...and end of timestep.
  std::vector<double> tortuosity_cell (cell_size);
  std::vector<double> dispersivity (cell_size);
  std::vector<double> dispersivity_transversal (cell_size);
  for (size_t c = 0; c < cell_size; c++)
    {
      Theta_old[c] = soil_water.Theta_primary_old (c);
      Theta_new[c] = soil_water.Theta_primary (c);
      const double Theta_cell_avg = 0.5 * (Theta_old[c] + Theta_new[c]);
      tortuosity_cell[c] = soil.tortuosity_factor (c, Theta_cell_avg);
      dispersivity[c] = soil.dispersivity (c);
      dispersivity_transversal[c] = soil.dispersivity_transversal (c);
    }

  // Edges.
  const size_t edge_size = geo.edge_size ();
  std::vector<double> q (edge_size); // Water flux [cm].
  std::vector<double> tortuosity_edge (edge_size);
  for (size_t e = 0; e < edge_size; e++)
    {
      q[e] = soil_water.q_primary (e);

      // Middle Theta in space and time.
      const int from = geo.edge_from (e);
      const int to = geo.edge_to (e);
      double Theta = 0.0;
      double count = 0.0;
      int cell = Geometry::cell_error;
      
      if (geo.cell_is_internal (to))
	{
	  Theta += Theta_old[to] + Theta_new[to];
	  count += 2.0;
	  cell = to;
	}
      if (geo.cell_is_internal (from))
	{
	  Theta += Theta_old[from] + Theta_new[from];
	  count += 2.0;
	  cell = from;		// We use "from" for internal edges.
	}
      daisy_assert (count > 0.0);
      daisy_assert (geo.cell_is_internal (cell));
      Theta /= count;
      tortuosity_edge[e] = soil.tortuosity_factor (cell, Theta);
    }

   // Upper border.
  std::map<size_t, double> J_forced;
  const std::vector<size_t>& edge_above = geo.cell_edges (Geometry::cell_above);
  const size_t edge_above_size = edge_above.size ();
  for (size_t i = 0; i < edge_above_size; i++)
    {
      const size_t edge = edge_above[i];
      J_forced[edge] = 0.0;
    }

  // Lower border.
  std::map<size_t, double> C_border;
  const std::vector<size_t>& edge_below = geo.cell_edges (Geometry::cell_below);
  const size_t edge_below_size = edge_below.size ();
  for (size_t i = 0; i < edge_below_size; i++)
    {
      const size_t edge = edge_below[i];
      C_border[edge] = 0.0;
    }

  element.tick (cell_size, soil_water, dt);
  static const symbol DOM_name ("DOM");
  flow (geo,
	tortuosity_edge, tortuosity_cell,
	dispersivity, dispersivity_transversal,
	Theta_old, Theta_new, q, DOM_name, 
        element.S, J_forced, C_border, element.C, element.J_matrix, 
        diffusion_coefficient, dt, msg);
  for (size_t c = 0; c < cell_size; c++)
    element.M[c] = element.C[c] * soil_water.Theta (c);
}

bool 
Transport::check (const Geometry&, Treelog&) const
{ return true; }

Transport::Transport (const BlockModel& al)
  : Model (),
    objid (al.type_name ())
{ }

Transport::~Transport ()
{ }

static struct TransportInit : public DeclareComponent 
{
  TransportInit ()
    : DeclareComponent (Transport::component, "\
Solute transport in primary domain.")
  { }
} Transport_init;

// transport.C ends here
