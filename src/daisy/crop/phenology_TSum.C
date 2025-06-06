// phenology_TSum.C -- Pure temperature sum crop phenology model.
// 
// Copyright 2003 Per Abrahamsen and Søren Hansen
// Copyright 2003 KVL.
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

#define BUILD_DLL

#include "daisy/crop/phenology.h"
#include "object_model/block_model.h"
#include "daisy/crop/production.h"
#include "daisy/crop/vernalization.h"
#include "object_model/plf.h"
#include "util/assertion.h"
#include "object_model/librarian.h"
#include "object_model/treelog.h"
#include "object_model/frame.h"

class PhenologyTSum : public Phenology
{
  // Parameters.
private:
  const double EmrTSum;		// Soil temp sum at emergence.
  const double EmrThrs;		// Minimum soil temp for emergence.
  const double VegTSum;		// Air temp sum for vegetative fase.
  const double VegThrs;		// Minimum air temp for emergence.
  const double RepTSum;		// Air temp sum for reproductive fase.
  const double RepThrs;		// Minimum air temp for reproductive.

  // Simulation.
private:
  void tick_daily (const Scope&, double Ta, bool leaf_growth, 
		   Production&, Vernalization&, double cut_stress, Treelog&);
  void emergence (const Scope&, double h, double T, double dt, Treelog&);

  // Create.
public:
  PhenologyTSum (const BlockModel&);
};

void
PhenologyTSum::tick_daily (const Scope&,
			   const double Ta, const bool /* leaf_growth */, 
			   Production& production, 
			   Vernalization& vernalization,
			   const double /*cut_stress*/, Treelog& out)
{
  // Update final day length.
  day_length = partial_day_length;
  partial_day_length = 0.0;

  // Update DS.
  daisy_assert (DS >= 0.0);

  if (DS < 1.0)
    {
      if (Ta > VegThrs)
	DS += Ta / VegTSum;

      vernalization (Ta, DS);

      if (DS >= 1.0)
	out.message ("Flowering");
    }
  else if (Ta > RepThrs)
    {
      DS += Ta / RepTSum;
      
      if (DS > 2.0)
       {
	 out.message ("Ripe");
	 DS = 2.0;
	 production.none ();
       }
    }
}

void
PhenologyTSum::emergence (const Scope&,
			  const double /*h*/, const double T, const double dt,
			  Treelog&)
{
  if (T > EmrThrs)
    DS += dt * T / EmrTSum;

  if (DS > 0.0)
    DS = 0.01;
}

PhenologyTSum::PhenologyTSum (const BlockModel& al)
  : Phenology (al),
    EmrTSum (al.number ("EmrTSum")),
    EmrThrs (al.number ("EmrThrs")),
    VegTSum (al.number ("VegTSum")),
    VegThrs (al.number ("VegThrs")),
    RepTSum (al.number ("RepTSum")),
    RepThrs (al.number ("RepThrs"))
{ }

static struct PhenologyTSumSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new PhenologyTSum (al); }

  PhenologyTSumSyntax ()
    : DeclareModel (Phenology::component, "TSum", 
	       "Crop phenology model purely based on temperature sums.\n\
The length of emergence, and the vegetative and reproductive fase are all\n\
based on the specified temperature sums.  Temperatures below the specified\n\
thresholds do not contribute to the temeprature sum.\n\
Cut stress and leaf respiration does not affect this phenology model.")
  { }
  void load_frame (Frame& frame) const
  {
    // Parameters.
    frame.declare ("EmrTSum", "dg C d", Attribute::Const,
		"Soil temperature sum at emergence.");
    frame.declare ("EmrThrs", "dg C", Attribute::Const,
		"Minimum soil temperature for emergence.\n\
Temperature below this will not count in the sum.");
    frame.set ("EmrThrs", 0.0);
    frame.declare ("VegTSum", "dg C d", Attribute::Const,
		"Air temperature sum for vegetative fase.");
    frame.declare ("VegThrs", "dg C", Attribute::Const,
		"Minimum air temperature for development in vegetative fase.\n\
Temperature below this will not count in the sum.");
    frame.set ("VegThrs", 0.0);
    frame.declare ("RepTSum", "dg C d", Attribute::Const,
		"Air temperature sum for vegetative fase.");
    frame.declare ("RepThrs", "dg C", Attribute::Const,
		"Minimum air temperature for development in vegetative fase.\n\
Temperature below this will not count in the sum.");
    frame.set ("RepThrs", 0.0);

  }
} PhenologyTSum_syntax;
