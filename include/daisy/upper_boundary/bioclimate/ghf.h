// ghf.h -- Ground heat flux
// 
// Copyright 2021 KU
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


#ifndef GHF_H
#define GHF_H

#include "object_model/model.h"

class Geometry;
class Soil;
class SoilWater;
class SoilHeat;
class Weather;
class Treelog;
class BlockModel;

class GHF : public Model
{
  // Content.
public:
  static const char *const component;

  // Simulation.
public:
  virtual double value (const Geometry&, const Soil&,
			const SoilWater&, const SoilHeat&, const Weather&,
			const double Rn_ref, Treelog&) const = 0; // [W/m^2]

  // Create and Destroy.
protected:
  GHF (const BlockModel&);
public:
  ~GHF ();
};

#endif // GHF_H

// gfh.h ends here.
