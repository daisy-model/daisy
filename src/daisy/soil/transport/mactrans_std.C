// mactrans_std.C -- Standard preferential flow model.
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

#include "daisy/soil/transport/mactrans.h"
#include "daisy/soil/soil_water.h"
#include "daisy/soil/transport/geometry1d.h"
#include "object_model/plf.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"
#include <sstream>
#include <vector>

struct MactransStandard : public Mactrans
{
  // Simulation.
 void tick (const Geometry1D&, const SoilWater&,
	    const std::vector<double>& M, const std::vector<double>& C,
	    std::vector<double>& S, std::vector<double>& S_p,
	    std::vector<double>& J_p, double dt, Treelog&);

  // Create and Destroy.
  bool check (const Geometry&, Treelog&) const;
  MactransStandard (const BlockModel& al)
    : Mactrans (al)
    { }
  MactransStandard (const symbol n)
    : Mactrans (n)
    { }
  ~MactransStandard ()
    { }
};

std::unique_ptr<Mactrans> 
Mactrans::create_default ()
{ return std::unique_ptr<Mactrans> (new MactransStandard (symbol ("default"))); }

void 
MactransStandard::tick (const Geometry1D& geo, const SoilWater& soil_water,
			const std::vector<double>& M,
                        const std::vector<double>& C,
			std::vector<double>& S_m, std::vector<double>& S_p,
			std::vector<double>& J_p, const double dt, 
                        Treelog& out)
{ 
  double max_delta_matter = 0.0; // [g/cm^2/h]

  for (size_t i = 0; i < geo.cell_size (); i++)
    {
      const double dz = geo.dz (i);

      // Amount of water entering this layer through macropores.
      const double water_in_above = -soil_water.q_tertiary (i); // [cm/h]
      const double water_out_below = -soil_water.q_tertiary (i+1); // [cm/h]
      const double delta_water = water_in_above - water_out_below; // [cm/h]
      
      // Amount of matter entering this layer through macropores.
      const double matter_in_above = -J_p[i]; // [g/cm^2/h]
      if (matter_in_above < 0.0)
        {
          std::ostringstream tmp;
          tmp << "Matter seem to pour out through the top, at a rate of "
                 << -J_p[i] * (100 * 100) * (100 * 100) 
                 << " [g/ha/h].  Strange";
          out.warning (tmp.str ());
        }
      double delta_matter;	// [g/cm^2/h]

      if (water_out_below < 1.0e-60)
	{
	  // No outgoing water, leave matter here.
	  delta_matter = -matter_in_above;
	}
      else if (delta_water < -1.0e-60)
	{
	  // More is going out below of the pore than comming in above.  
	  // Water enter here from the matrix with the local concentration.
	  delta_matter = std::min (-C[i] * delta_water,
                                   (M[i] / dt + S_m[i]) * dz - 1e-16);
	  if (delta_matter < 0.0)
	    delta_matter = 0.0;
	}
      else if (delta_water > 1.0e-60)
	{
	  // More water is comming in above than leaving below.
	  // Water leave the pore here and enters the matrix. 

	  if (water_in_above > 0.0)
	    {
	      // Fraction of water entering the layer through the
	      // macropore, which also stayes here.
	      /*const*/ double water_fraction 
			  = approximate (delta_water, water_in_above) 
			  ? 1.0
			  : delta_water / water_in_above;
	      if (water_fraction < 0.0 || water_fraction > 1.0)
		{
		  Treelog::Open nest (out, "mactrans default");
		  std::ostringstream tmp;
		  tmp << __FILE__ << ":" <<  __LINE__
			 << ": BUG: water fraction from macropore = " 
			 << water_fraction;
		  out.error (tmp.str ());
		  set_bound (0.0, water_fraction, 1.0);
		}

	      // Matter stayes with the water.
	      delta_matter = -matter_in_above * water_fraction;
	      // daisy_assert (delta_matter <= 0.0);
	    }
	  else
	    {
	      // Water through macropores from below... 
	      delta_matter = 0.0; // Just assume pure water.
	    }
	}
      else
	delta_matter = 0.0;
      
      const double abs_delta_matter = fabs (delta_matter);
      if (abs_delta_matter > max_delta_matter)
	max_delta_matter = abs_delta_matter;
      if (matter_in_above > max_delta_matter)
	max_delta_matter = matter_in_above;

      // Find amount of stuff leaving the layer.
      if (abs_delta_matter < 1e-60)
	{
	  // Everything go to the bottom.
	  J_p[i+1] = J_p[i];
	  S_p[i] = 0.0;
	}
      else if (approximate (matter_in_above, -delta_matter))
	{
	  // Everything go to the layer.
	  J_p[i+1] = 0.0;
	  // daisy_assert (matter_in_above > 0.0);
	  S_p[i] = matter_in_above / dz;
	}
      else
	{
	  // We split between layer and bottom.
	  J_p[i+1] = -(matter_in_above + delta_matter);
	  // daisy_assert (J_p[i+1] < 0.0);
	  S_p[i] = -delta_matter / dz;
	}
      S_m[i] += S_p[i];
    }
  
  // Check that the sink terms add up.
  if (fabs (geo.total_surface (S_p) + J_p[0] - J_p[geo.edge_size () - 1])
      > max_delta_matter * 1e-8)
    {
      Treelog::Open nest (out, "mactrans default");
      std::ostringstream tmp;
      tmp << __FILE__ << ":" <<  __LINE__
	     << ": BUG: Total S_p = '"
          << (geo.total_surface (S_p) + J_p[0]  - J_p[geo.cell_size ()])
	     << "' solute\n";
      out.error (tmp.str ());
    }
}

bool 
MactransStandard::check (const Geometry& geo, Treelog& msg) const
{ 
  bool ok = true;
  if (!dynamic_cast<const GeometryVert*> (&geo))
    {
      msg.error ("\
This tertiary solute transport model only works with vertical geometries");
      ok = false;
    }
  return ok; 
}

static struct MactransStandardSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
    { return new MactransStandard (al); }
  MactransStandardSyntax ()
    : DeclareModel (Mactrans::component, "default", "Solute follows water.")
  { }
  void load_frame (Frame& frame) const
    {
    }
} MactransStandard_syntax;

// mactrans_std.C ends here.
