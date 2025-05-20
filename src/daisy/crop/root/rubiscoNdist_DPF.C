// rubiscoNdist_DPF.C -- Crop rubisco N distribution model of De Pury & Farquhar (1997)
// 
// Copyright 2006 Birgitte Gjettermann and KVL
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU Lesser Public License for more details.
// 
// You should have received a copy of the GNU Lesser Public License
// along with Daisy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#define BUILD_DLL
#include "daisy/crop/root/rubiscoNdist.h"
#include "util/mathlib.h"
#include "object_model/block_model.h"
#include <sstream>
#include "object_model/check.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

static const double Mw = 14.0; //The molecular weight for N [g molÂ¯1]

struct rubiscoNdistDPF : public RubiscoNdist
{
  // Parameters.
private:
  const double kn;  //Extinction coefficient of nitrogen in the canopy
  
  // Simulation.
  void tick (std::vector <double>& , std::vector <double>& , 
	     const double , Treelog&)
  { }
  void rubiscoN_distribution (const Units&, 
                              const std::vector <double>& PAR_height, 
			      const double LAI, const double DS,
			      std::vector <double>& rubiscoNdist/*[mol/mÂ²]*/,  
			      const double cropN /*[g/mÂ²area]*/, Treelog&);
  void output (Log&) const
  { }

  // Create.
  public:
  rubiscoNdistDPF (const BlockModel& al)
    : RubiscoNdist (al),
       kn (al.number ("kn"))
  { }
};

void
rubiscoNdistDPF::rubiscoN_distribution (const Units&,
                                        const std::vector <double>& PAR_height, 
					const double LAI, const double DS, 
					std::vector <double>& rubiscoNdist, 
					const double cropN/*[g]*/, Treelog& msg)
{
  daisy_assert (std::isfinite (cropN));
  daisy_assert (cropN >= 0.0);
  
  double kn_LAI;
  if (LAI < 0.01) {
    kn_LAI = kn / 0.01;
  }
  else {
    kn_LAI = kn / LAI;
  }

  // Crop N in top of the canopy:
  const double divisor = 1. - exp(-kn);
  daisy_assert(divisor > 0.0);
  daisy_assert (std::isnormal(divisor));

  double cropN0 = kn_LAI * cropN / divisor; // [g/mÂ² leaf]
  cropN0 = cropN0 / Mw;  // [mol/mÂ² leaf]
  daisy_assert (cropN0 >= 0.0);

  // Fill photosynthetically active N (cummulative) for each canopy layer in vector
  const int No = rubiscoNdist.size ();

  const double dLAI = (LAI /(No + 0.0));
  for (int i = 0; i < No; i++)
     rubiscoNdist[i] = cropN0 * (exp(-kn_LAI * dLAI *(i+0.5))); //[mol/mÂ² leaf]


}

static struct rubiscoNdistDPFSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new rubiscoNdistDPF (al); }

  rubiscoNdistDPFSyntax ()
    : DeclareModel (RubiscoNdist::component, "exp", 
	       "Boegh et al.(2002) rubisco N-distribution model in the canopy for photosynthesis.")
  { }
  void load_frame (Frame& frame) const
  {

    frame.declare ("kn", Attribute::None (), Check::positive (), Attribute::Const,
                "Extinction coefficient of nitrogen in the canopy, kn = 0.713 (De Pury &Farquhar, 1997)");
    frame.set ("kn", 0.713);
  }
} rubiscoNdistDPF_syntax;

// rubiscoNdist_DPF.C ends here
