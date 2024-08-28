// denprod.h --- Find products of denitrification.
// 
// Copyright 2018, 2024 KU.
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


#ifndef DENPROD_H
#define DENPROD_H

#include "object_model/model_derived.h"
#include "object_model/symbol.h"
#include <vector>

class Geometry;
class Soil;
class SoilWater;
class Chemical;
class OrganicMatter;
class Treelog;
class BlockModel;

class Denprod : public ModelDerived
{
  // Content.
public:
  static const char *const component;
  symbol library_id () const;

  // Simulation.
public:
  virtual void split (const std::vector<double>& N, const Geometry& geo,
		      const Soil& soil, const SoilWater& soil_water,
		      Chemical& soil_NO3, const OrganicMatter& organic,
		      Treelog& msg) = 0;
  
  // Create and Destroy.
public:
  virtual void initialize (const Geometry&) = 0;
protected:
  Denprod (const BlockModel&);
public:
  ~Denprod ();
};

#endif // DENPROD_H
