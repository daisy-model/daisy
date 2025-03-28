// horizon.h
// 
// Copyright 1996-2001, 2003 Per Abrahamsen and Søren Hansen
// Copyright 2000-2001, 2003 KVL.
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


#ifndef HORIZON_H
#define HORIZON_H

#include "object_model/model_derived.h"
#include "object_model/symbol.h"
#include <vector>
#include <set>
#include <memory>

class Log;
class Hydraulic;
class Tortuosity;
class Texture;
class Treelog;
class BlockModel;
class Secondary;

class Horizon : public ModelDerived
{
  // Content.
private:
  struct Implementation;
  std::unique_ptr<Implementation> impl;
  double fast_clay;
  double fast_humus;
public:
  static const char *const component;
  symbol library_id () const;

  // Water.
public:
  std::unique_ptr<Hydraulic> hydraulic;
  double anisotropy () const;
  double heat_conductivity (double Theta, double Ice) const; // [erg/cm/h/dg C]
  double heat_capacity (double Theta, double Ice) const; // [erg/cm^3/dg C]
  const Secondary& secondary_domain () const;
  double primary_sorption_fraction () const;

  // Texture.
public:
  virtual const Texture& texture () const = 0;
  double CEC () const;
  std::unique_ptr<Tortuosity> tortuosity;
  double dry_bulk_density () const;
  virtual double texture_below (double size /* [um] */) const = 0;
  double clay () const;
  double humus () const;
  double humus_C () const;
  const std::vector<double>& SOM_fractions () const;
  const std::vector<double>& SOM_C_per_N () const;
  double C_per_N () const;
  double turnover_factor () const;
  double root_homogeneity () const;
  double root_retardation () const;
  double K_factor () const;

  // Chemistry.
public:
  bool has_attribute (symbol) const;
  double get_attribute (symbol) const;
  symbol get_dimension (symbol name) const;
  void append_attributes (std::set<symbol>& all) const;

  // Simulation.
public:
  void nitrification (const double M, const double C, const double Theta,
                      const double h, const double T,
                      double& NH4, double& N2O, double& NO3) const;
 
void output (Log&) const;

  // Create and Destroy.
public:
  explicit Horizon (const BlockModel&);
  virtual void initialize (bool top_soil, int som_size, double center_z, 
                           Treelog&) = 0;
protected:
  void initialize_base (bool top_soil, int som_size, double center_z, const Texture& texture, 
                        Treelog&);
public:
  ~Horizon ();
};

#endif // HORIZON_H
