// root_system.h -- Root development and uptake.
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

#ifndef ROOT_SYSTEM_H
#define ROOT_SYSTEM_H

#include "object_model/model_derived.h"
#include <vector>

class Geometry;
class Soil;
class SoilHeat;
class SoilWater;
class Chemistry;
class Log;
class Treelog;
class BlockModel;

class RootSystem : public ModelDerived
{
 // Content.
public:
  static const char *const component;
  symbol library_id () const;

  // State.
public:
  virtual double ABA_concentration () const = 0;
  virtual double soil_T () const = 0;	   // [dg C]
  virtual double crown_potential () const = 0; // [cm]
  virtual const std::vector<double>& actual_density () const = 0;
  virtual const std::vector<double>& effective_density () const = 0;
  virtual double depth () const = 0;
  virtual double water_stress () const = 0;
  virtual double water_stress_days () const = 0;
  virtual double production_stress () const = 0;
  virtual void set_production_stress (double) = 0;
  
  // Uptake.
private:
  virtual double potential_water_uptake (double h_x,
					 const Geometry&,
					 const Soil& soil,
					 const SoilWater& soil_water,
					 double dt) = 0;
public:
  virtual double water_uptake (double Ept,
			       const Geometry&,
			       const Soil& soil, const SoilWater& soil_water,
			       double EvapInterception, double dt,
			       Treelog&) = 0;
public:
  virtual double nitrogen_uptake (const Geometry&,
				  const Soil& soil,
				  const SoilWater& soil_water,
				  Chemistry& chemistry,
				  double NH4_root_min,
				  double NO3_root_min,
				  double PotNUpt) = 0;

  // Simulation.
  virtual const std::vector<double>&
  /**/ dynamic_root_death () const = 0; // [cm/cm^3/h]
  virtual double dynamic_root_death_DM () const = 0; // [g DM/h]
public:
  virtual void tick_dynamic (const Geometry& geo, const SoilHeat&, SoilWater&,
			     const double day_fraction, const double dt,
			     Treelog&) = 0;
  virtual void tick_daily (const Geometry&, const Soil&, const SoilWater&,
			   double WRoot, bool root_growth, double DS,
			   Treelog&) = 0;
  virtual void set_density (const Geometry& geometry, const Soil& soil,
			    double WRoot, double DS, Treelog&) = 0;
  virtual void full_grown (const Geometry&, const Soil&, double WRoot,
			   Treelog&) = 0;
  virtual void output (Log& log) const = 0;

  // Create and Destroy
public:
  virtual void initialize (const Geometry& geo, const Soil& soil, 
			   double row_width, double row_pos, const double DS,
			   Treelog& msg) = 0;
  virtual void initialize (const Geometry& geo, const Soil&, const double DS,
			   Treelog& msg) = 0;
  virtual bool check (const Geometry& geo, Treelog& msg) const = 0;
  RootSystem (const BlockModel& al);
  ~RootSystem ();
};

#endif // ROOT_SYSTEM_H
