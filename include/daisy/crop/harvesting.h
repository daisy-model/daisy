// harvesting.h -- Harvest parameters for the default crop model.
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

#ifndef HARVESTING_H
#define HARVESTING_H

#include "daisy/daisy_time.h"
#include "object_model/plf.h"
#include "object_model/symbol.h"
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

class Frame;
class FrameModel;
class Log;
class Geometry;
class Production;
class AM;
class Harvest;
class Metalib;
class Treelog;

class Harvesting 
{
  const Metalib& metalib;

  // Parameters.
private:
  const std::vector<boost::shared_ptr<const FrameModel>/**/>& Stem; // Stem AM parameters.
  const std::vector<boost::shared_ptr<const FrameModel>/**/>& Leaf; // Leaf AM parameters.
public:
  const std::vector<boost::shared_ptr<const FrameModel>/**/>& Dead; // Dead AM parameters.
private:
  const std::vector<boost::shared_ptr<const FrameModel>/**/>& SOrg; // SOrg AM parameters.
public:
  const std::vector<boost::shared_ptr<const FrameModel>/**/>& Root; // Root AM parameters.
private:
  const double EconomicYield_W; // Frac. of economic yield (DM) in storage org.
  const double EconomicYield_N; // Frac. of economic yield (N) in storage org.
  const double DSmax;		// Maximal development stage for which
				// the crop survives harvest.
public:
  const double DSnew;		// Maximal development stage after harvest.
private:
  Time last_cut;		// Date of last cut.
  double production_delay;	// Production delay after cut [d]
  const PLF cut_delay;		// -||- as function of removed fraction.
public:
  double cut_stress;		// Cut induced stress.
  double total_water_use;       // Accumulated water use [kg H2O].

public:
  const double sorg_height;     // Location above ground of storage organ [cm].
                                // If this is negative, harvest will kill the
                                // plant and imply a mix operation.
  // Simulation.
public:
  const Harvest& harvest (symbol column_name,
                          symbol crop_name,
                          const std::vector<double>& density,
                          const Time& sow_time,
                          const Time& emerge_time,
                          const Time& flowering__time,
                          const Time& ripe_time,
                          const Time& harvest_time,
                          const Geometry& geometry,
                          Production& production,
                          double& DS,
                          const double stem_harvest,
                          const double leaf_harvest,
                          const double sorg_harvest,
                          const double stem_harvest_frac,
                          const double leaf_harvest_frac,
                          const double sorg_harvest_frac,
                          const bool kill_off,
                          std::vector<AM*>& residuals,
                          double& residuals_DM,
                          double& residuals_N_top, double& residuals_C_top,
                          std::vector<double>& residuals_N_soil,
                          std::vector<double>& residuals_C_soil,
                          const bool combine,
                          double& water_stress_days,
                          double& nitrogen_stress_days,
                          Treelog&);
  void tick (const Time& time);
  void water_use (double amount /* [kg H2O] */);
  void output (Log& log) const;

  // Create and Destroy.
public:
  static void load_syntax (Frame&);
  Harvesting (const Block&);
  ~Harvesting ();
};

#endif // HARVESTING_H
