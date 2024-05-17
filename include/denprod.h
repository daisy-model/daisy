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

#include "model.h"
#include "symbol.h"

class BlockModel;

class Denprod : public Model
{
  // Content.
public:
  static const char *const component;
  symbol library_id () const;

  // Simulation.
public:
  virtual void split (const double N, const double wfps, const double NO3,
		      const double CO2, const double T,
		      double& N2O) const = 0;

  // Create and Destroy.
protected:
  Denprod ();
public:
  ~Denprod ();
};

#endif // DENPROD_H
