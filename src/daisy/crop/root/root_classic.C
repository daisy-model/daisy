// root_zone.C -- Root zone growth.
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

#include "daisy/crop/root/root_zone.h"
#include "daisy/crop/root/rootdens.h"
#include "daisy/soil/soil_water.h"
#include "daisy/soil/soil.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/output/log.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "util/mathlib.h"

// The 'classic' model.

struct RootClassic : public RootZone
{
  std::unique_ptr<Rootdens> rootdens; // Root density calculation.

  const std::vector<double>& dynamic_root_death () const // [cm/cm^3/h]
  { return rootdens->dynamic_root_death (); }

  double dynamic_root_death_DM () const // [g DM/h]
  { return rootdens->dynamic_root_death_DM (); }

  void tick_dynamic (const Geometry& geo, const SoilHeat&, SoilWater&,
		     const double day_fraction, const double dt, Treelog&);
  void tick_daily (const Geometry&, const Soil&, const SoilWater&,
		   double WRoot, bool root_growth, double DS, Treelog&);
  void set_density (const Geometry& geometry, const Soil& soil,
                    double WRoot, double DS, Treelog&);
  void full_grown (const Geometry&, const Soil&, double WRoot, Treelog&);
  void output (Log& log) const;

  // Create and Destroy
  void initialize (const Geometry& geo, const Soil& soil, 
                   double row_width, double row_pos, const double DS,
		   Treelog& msg);
  void initialize (const Geometry& geo, const Soil& soil, const double DS,
		   Treelog& msg)
  { initialize_uptake (geo, soil, DS, msg); }
  RootClassic (const BlockModel& al)
   : RootZone (al),
     rootdens (al.check ("rootdens") 
	       ? Librarian::build_item<Rootdens> (al, "rootdens")
	       : NULL)
  { }
  ~RootClassic ()
  { }
};

void
RootClassic::tick_dynamic (const Geometry& geo, const SoilHeat& soil_heat,
			   SoilWater& soil_water, const double day_fraction,
			   const double dt, Treelog& msg)
{
  // Root death.
  rootdens->tick (geo, soil_heat, soil_water, Density, dt, msg);

  tick_uptake (geo, soil_heat, soil_water, day_fraction, dt, msg);
}

void
RootClassic::tick_daily (const Geometry& geo, const Soil& soil, 
                        const SoilWater& soil_water,
                        const double WRoot, const bool root_growth,
                        const double DS, Treelog& msg)
{
  const double SoilLimit = -soil.MaxRootingHeight (); // [cm], negative
  const double z = -Depth;	// [cm], negative

  // Penetration.
  if (root_growth)
    {
      // Limit by pressure (pF).
      class pFAccess : public Geometry::Access
      { 
	const SoilWater& soil_water;
	const double pF_min;
	
	double operator()(size_t c) const
	{
	  const double h = soil_water.h (c);
	  if (h < 0.0)
	    return std::max (h2pF (h), pF_min);
	  return pF_min;
	}
      public:
	pFAccess (const SoilWater& sw, const double pF_min_)
	  : soil_water (sw),
	    pF_min (pF_min_)
	{ }
      } pF_accessor (soil_water, 0.0);
      const double pF = geo.access_content_height (pF_accessor, z);
      const double pF_fac = PenpFFac (pF);

      // Limit by clat content.
      const double clay = geo.content_height (soil, &Soil::clay, z);
      const double clay_fac = PenClayFac (clay);

      // Limit by relative water content.
      const double Theta 
        = geo.content_height (soil_water, &SoilWater::Theta, z);
      daisy_assert (Theta >= 0.0);
      const double Theta_sat 
        = geo.content_height (soil, &Soil::Theta_sat, z);
      daisy_assert (Theta_sat > 0.0);

      daisy_assert (Theta_sat <= 1.0);
      const double water = Theta/Theta_sat;
      daisy_assert (water >= 0.0);
      daisy_assert (water <= 1.01);
      double water_fac = PenWaterFac (water);

      // Limit by development stage (DS).
      const double DS_fac = PenDSFac (DS);

      // Limit by horizon.
      const double soil_fac
	= geo.content_height (soil, &Soil::root_retardation, z);
      
      double dp = PenPar1 * pF_fac * clay_fac * water_fac * DS_fac * soil_fac
        * std::max (0.0, soil_temperature - PenPar2);
      PotRtDpt = std::min (PotRtDpt + dp, MaxPen);
      /*max depth determined by crop*/
      Depth = std::min (Depth + dp, MaxPen);
      PotRtDpt = std::max (PotRtDpt, Depth);
      /*max depth determined by crop*/
      Depth = std::min (Depth, SoilLimit); /*or by soil conditions*/
    }
  set_density (geo, soil, WRoot, DS, msg);
}

void
RootClassic::set_density (const Geometry& geo, const Soil& soil,
                         const double WRoot, const double DS, Treelog& msg)
{
  const double SoilLimit = -soil.MaxRootingHeight ();
  rootdens->set_density (geo, SoilLimit, PotRtDpt, 
                         PotRtDpt * (MaxWidth / MaxPen),
                         WRoot, DS, Density, msg);

  const double DS_fac = DensityDSFac (DS);
  daisy_assert (EffectiveDensity.size () == Density.size ());
  daisy_assert (EffectiveDensity.size () == geo.cell_size ());
  for (size_t c = 0; c < geo.cell_size (); c++)
    EffectiveDensity[c] = Density[c] * soil.root_homogeneity (c) * DS_fac;
}

void
RootClassic::full_grown (const Geometry& geo, const Soil& soil,
                        const double WRoot, Treelog& msg)
{
  const double SoilLimit = -soil.MaxRootingHeight ();
  PotRtDpt = MaxPen;
  Depth = std::min (MaxPen, SoilLimit);
  set_density (geo, soil, WRoot, 1.0, msg);
}

void
RootClassic::output (Log& log) const
{
  RootZone::output (log);
  
  output_object (rootdens, "rootdens", log);
}

void
RootClassic::initialize (const Geometry& geo, const Soil& soil,
			 const double row_width, 
			 const double row_pos, const double DS, Treelog& msg)
{
  msg.message ("init 2d");
  const bool is_row_crop = row_width > 0.0;
  if (rootdens.get ())
    /* We already have a root density model. */;
  else if (is_row_crop)
    rootdens = Rootdens::create_row (metalib, msg, row_width, row_pos);
  else
    rootdens = Rootdens::create_uniform (metalib, msg);

  rootdens->initialize (geo, row_width, row_pos, msg);
  initialize_uptake (geo, soil, DS, msg);
}

static struct RootClassicSyntax : DeclareModel
{
  static bool
  check_alist (const Metalib&, const Frame& al, Treelog& msg)
  {
    bool ok = true;
    return ok;
  }

  void load_frame (Frame& frame) const
  {
    frame.declare_object ("rootdens", Rootdens::component, 
			  Attribute::OptionalConst, Attribute::Singleton,
			  "Root density model.");
  }

  bool used_to_be_a_submodel () const
  { return true; }

  Model* make (const BlockModel& al) const
  { return new RootClassic (al); }

  RootClassicSyntax () 
    : DeclareModel (RootSystem::component, "classic", "zone", "\
Classic root system model.")
  { }
} RootClassic_syntax;

// root_classic.C ends here.
