// aom.h --- A single added organic matter pool.
// 
// Copyright 2002 Per Abrahamsen and KVL.
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


#ifndef AOM_H
#define AOM_H

#include "daisy/organic_matter/om.h"
#include <vector>

class Frame;
class Volume;

class AOM : public OM
{ 
public:
  static const char *const component;
  symbol library_id () const;

  static bool compare_CN (const AOM* a, const AOM* b);
  
  // Parameters.
public:
  const double initial_fraction; // Relative fraction for this om.

  // Content.
public:
  double top_C;			// Carbon on the ground.
  double top_N;			// Nitrogen on the ground.
  double SOM_fraction () const;	// Fraction of SOM generated from turnover.
  
  // Simulation.
public:
  void output (Log&) const;
  void penetrate (const Geometry&, double from, double to, double penetration,
                  double& tillage_N_top, double& tillage_C_top,
                  std::vector<double>& tillage_N_soil,
                  std::vector<double>& tillage_C_soil);
  void penetrate (const Geometry&, const Volume&, double penetration,
                  double& tillage_N_top, double& tillage_C_top,
                  std::vector<double>& tillage_N_soil,
                  std::vector<double>& tillage_C_soil);
  double full_C (const Geometry& geometry) const;
  double full_N (const Geometry& geometry) const;
  double C_at (unsigned int at) const;
  double N_at (unsigned int at) const;
  void pour (std::vector<double>& cc, std::vector<double>& nn);
  void add (unsigned int at, double C, double N);
  void add (double C, double N);// Add dead leafs.
  void add (const Geometry&,	// Add dead roots.
	    double C, double N, 
	    const std::vector<double>& density);
  void tick (const std::vector<bool>&, const double* turnover_factor,
	     const double* N_soil, double* N_used,
	     double* CO2, const std::vector<SMB*>& smb, 
	     double* som_C, double* som_N, const std::vector<DOM*>& dom,
             double dt);
private:
  // Disallow this OM function.
  void tick (const std::vector<bool>&, const double* turnover_factor, 
	     const double* N_soil, double* N_used,
	     double* CO2, 
	     const std::vector<SMB*>& smb, const std::vector<SOM*>&som,
	     const std::vector<DOM*>& dom, double dt);

  // Create & Destroy.
public:
  AOM (const BlockModel& al);
};

#endif // AOM_H
