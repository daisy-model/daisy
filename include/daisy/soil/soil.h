// soil.h
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


#ifndef SOIL_H
#define SOIL_H

#include "object_model/symbol.h"
#include <vector>
#include <set>
#include <memory>
#include <boost/noncopyable.hpp>

class Horizon;
class Log;
class Geometry;
class Groundwater;
class Frame;
class Block;
class Treelog;
class Time;
class SoilWater;
class SoilHeat;
class OrganicMatter;
class Hydraulic;

class Soil : private boost::noncopyable
{
  // Content.
public:
  struct Implementation;
  std::unique_ptr<Implementation> impl;

public:
  size_t size () const;
  const Horizon& horizon (size_t i) const;
private:
  const Hydraulic& hydraulic (size_t i) const;
  
  // Water.
public:
  double K (size_t i, double h, double h_ice, double T) const;
  double Cw1 (size_t i, double h, double h_ice) const;
  double Cw2 (size_t i, double h) const;
  double Theta (size_t i, double h, double h_ice) const;
  double Theta_res (size_t i) const;
  double Theta_sat (size_t i) const;
  double h (size_t i, double Theta) const;
  double M (size_t i, double h) const;
public:
  double primary_sorption_fraction (size_t c) const;
  double dispersivity (size_t) const;
  double dispersivity_transversal (size_t) const;  
  void tillage (const Geometry& geo, const double from, const double to,
                const double surface_loose, const double RR0, 
                const SoilWater& soil_water, const OrganicMatter&);
  void tick (const double dt /* [h] */, const double rain /* [mm/h] */,
             const Geometry& geo, const SoilWater& soil_water,
	     const SoilHeat& soil_heat, Treelog&);
  void set_porosity (size_t i, double Theta);
  // Activation pressure for secondary domain. [cm] 
  double h_secondary (size_t i) const;
  // Exchange rate between primary and secondary water.  [h^-1] 
  double alpha (size_t i) const; 

  // Texture.
  double tortuosity_factor (size_t i, double Theta) const; // [cm^3/cm^3]
  double anisotropy_cell (size_t c) const;
  double anisotropy_edge (size_t e) const;
  double dry_bulk_density (size_t i) const;
  double clay (size_t i) const;
  double texture_below (size_t i, double size /* [um] */) const;
  double /* [um] */ texture_fractile (size_t i, double fraction /* [] */) const;
  double humus (size_t i) const;	      // []
  double humus_C (size_t i) const;	      // [g C/cm^3]
  const std::vector<double>& SOM_fractions (size_t i) const;
  const std::vector<double>& SOM_C_per_N (size_t i) const;
  double C_per_N (size_t i) const;
  double turnover_factor (size_t i) const;
  double root_homogeneity (size_t i) const;
  double root_retardation (size_t i) const;
  
  // Thermic.
  double heat_conductivity (size_t i, double Theta, double Ice) const;
  double heat_capacity (size_t i, double Theta, double Ice) const;
  
  // Chemistry.
  bool has_attribute (const symbol name, Treelog& msg) const;
  bool has_attribute (const symbol name) const;
  bool has_attribute (size_t i, const symbol name) const;
  double get_attribute (size_t i, const symbol name) const;
  symbol get_dimension (size_t i, const symbol name) const;
  void append_attributes (size_t i, std::set<symbol>& all) const;

  // Simulation.
public:
  void output (Log&) const;
  void nitrification (const size_t i,
                      const double M, const double C, const double Theta,
                      const double h, const double T,
                      double& NH4, double& N2O, double& NO3) const;

  // Calculations.
  double MaxRootingHeight () const;
  double end_of_first_horizon () const;

  // Creation.
  bool check (int som_size, Geometry& geo, Treelog&) const;
  bool check_z_border (double, Treelog& err) const;
  bool check_x_border (double, Treelog& err) const;
  bool check_y_border (double, Treelog& err) const;

  static void load_syntax (Frame&);
public:
  explicit Soil (const Block&);
  void initialize (const Time&, Geometry&, Groundwater&, int som_size, Treelog& msg);
  ~Soil ();
};

#endif // SOIL_H
