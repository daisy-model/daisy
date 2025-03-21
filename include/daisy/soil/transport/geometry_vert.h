// geometry_vert.h --- A discretization based on vertical columns.
// 
// Copyright 2006 Per Abrahamsen and KVL.
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


#ifndef GEOMETRY_VERT_H
#define GEOMETRY_VERT_H

#include "daisy/soil/transport/geometry.h"
#include <vector>

class Block;

class GeometryVert : public Geometry
{
  // Parameters.
protected:
  std::vector<double> zplus_;	// Lower boundary of each interval.
  std::vector<double> z_;       // Center of each interval.
  std::vector<double> dz_;      // Size of each interval.

public:
  // Accessors.
  inline double zplus (size_t n) const
  { return zplus_[n]; }
  inline double cell_z (size_t n) const
  { return z_[n]; }
  inline double dz (size_t n) const
  { return dz_[n]; }
  virtual double zminus (size_t n) const = 0;
  double cell_bottom (size_t n) const // Lowest point in cell [cm]
  { return zplus (n); }
  double cell_top (size_t n) const // Highest point in cell [cm]
  { return zminus (n); }

  bool contain_z (size_t i, double z) const;

  // Creation.
  bool check_z_border (const double value, Treelog& err) const;
public:
  GeometryVert ();
  GeometryVert (const Block&);
  ~GeometryVert ();
};

#endif // GEOMETRY_VERT_H
