// root_uptake.h -- Root uptake base model.
// 
// Copyright 1996-2001 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001 KVL.
// Copyright 2026 KU.
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

#ifndef ROOT_UPTAKE_H
#define ROOT_UPTAKE_H

#include "daisy/crop/root/root_system.h"
#include "object_model/plf.h"

#include <memory>

class ABAProd;
class Solupt;
class Metalib;

struct RootUptake : public RootSystem
{
 // Content.
  const Metalib& metalib;

  // Components.
  std::unique_ptr<ABAProd> ABAprod;   // Root density calculation.
  std::unique_ptr<Solupt> NH4_uptake; // Ammonium uptake.
  std::unique_ptr<Solupt> NO3_uptake; // Nitrate uptake.

  // Parameters.
  const double Rad;		// Root radius [cm]
  const double h_wp;		// Matrix potential at wilting
  const double MxNH4Up;		// Max NH4 uptake per unit root length
  const double MxNO3Up;		// Max NO3 uptake per unit root length
  const double Rxylem;		// Transport resistence in xyleme
  const PLF DensityDSFac;	// DS influence on homogeneity [DS] -> []

  // State.
  double PotRtDpt;	        // Potential Root Penetration Depth [cm]
  double Depth;			// Rooting Depth [cm]
  std::vector<double> Density;	// Root density [cm/cm3] in soil layers
  std::vector<double> EffectiveDensity;	// Effective root density [cm/cm3]
  const std::vector<double>& actual_density () const
  { return Density; }
  const std::vector<double>& effective_density () const
  { return EffectiveDensity; }
  virtual const std::vector<double>&
  /**/ dynamic_root_death () const = 0; // [cm/cm^3/h]
  virtual double dynamic_root_death_DM () const = 0; // [g DM/h]
  std::vector<double> H2OExtraction; // Extraction of H2O in soil [cm³/cm³/h]
  std::vector<double> NH4Extraction; // Extraction of NH4-N in soil [gN/cm³/h]
  std::vector<double> NO3Extraction; // Extraction of NH4-N in soil [gN/cm³/h]
  std::vector<double> ABAExtraction; // Extraction of ABA in soil [g ABA/cm³/h]
  double ABAConc;		// ABA concentration in uptake [g/cm^3]
  double h_x;			// Root extraction at surface
  double partial_soil_temperature; // Accumaleted soil temperature [°C]
  double partial_day;           // Accuumalted time [h]
  double soil_temperature;	// Soil temperature previous day [°C]

  // Log.
  double water_stress_;		// Fraction of requested water we didn't got
  double water_stress_days_;	// Accumulated water stress
  double production_stress_;	// SVAT induced stress, -1 if not applicable
  double Ept;			// Potential evapotranspiration
  double H2OUpt;		// H2O uptake [mm/h]
  double NH4Upt;		// NH4-N uptake [g/m2/h]
  double NO3Upt;		// NO3-N uptake [g/m2/h]

  double ABA_concentration () const;
  double soil_T () const;	   // [dg C]
  double crown_potential () const; // [cm]
  double depth () const;	   // [cm]
  double water_stress () const;
  double water_stress_days () const;
  double production_stress () const;
  void set_production_stress (double);

  // Uptake.
  double potential_water_uptake (double h_x,
                                 const Geometry&,
				 const Soil& soil,
				 const SoilWater& soil_water,
                                 double dt);
  double water_uptake (double Ept,
                       const Geometry&,
		       const Soil& soil, const SoilWater& soil_water,
                       double EvapInterception, double dt, Treelog&);
  double nitrogen_uptake (const Geometry&,
                          const Soil& soil,
			  const SoilWater& soil_water,
			  Chemistry& chemistry,
			  double NH4_root_min,
			  double NO3_root_min,
			  double PotNUpt);

  void tick_uptake (const Geometry& geo, const SoilHeat& soil_heat,
		    SoilWater& soil_water, const double day_fraction,
		    const double dt, Treelog& msg);
  
  void output (Log& log) const;

  // Create and Destroy
  void initialize_uptake (const Geometry& geo, const Soil& soil,
			  const double DS, Treelog& msg);
  bool check (const Geometry& geo, Treelog& msg) const;
  RootUptake (const BlockModel& al);
  ~RootUptake ();
};

#endif // ROOT_UPTAKE_H
