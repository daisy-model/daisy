// clayom.h --- Clay effect on organic matter.
// 
// Copyright 2002 KVL and Per Abrahamsen.
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


#ifndef CLAYOM_H
#define CLAYOM_H

#include "object_model/model.h"
#include "object_model/symbol.h"
#include <vector>

class SMB;
class Soil;
class Treelog;
class BlockModel;

class ClayOM : public Model
{
  // Content.
public:
  const symbol name;
  static const char *const component;
  symbol library_id () const;


  // Simulation.
public:
  virtual void set_rates (const Soil& soil, 
                          const std::vector<SMB*>& smb) const = 0;
  virtual double factor (const double clay) const = 0;
  virtual bool smb_use_clay (unsigned int pool) const = 0;
  virtual bool som_use_clay (unsigned int pool) const = 0;

  // Create and Destroy.
public:
  virtual bool check (const std::vector<SMB*>& smb, Treelog& err) const = 0;
protected:
  ClayOM (const BlockModel& al);
public:
  ~ClayOM ();
};

#endif // CLAYOM_H
