// rubiscoNdist_Uniform.C -- Rubisco crop N distribution model of De Pury & Farquhar (1997)
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
#include <sstream>
#include "object_model/check.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

static const double Mw = 14.0; //The molecular weight for N [g molÂ¯1]

struct rubiscoNdistUniform : public RubiscoNdist
{
  // Parameters.
private:
  const double f_photo; //Fraction of photosynthetically active N in canopy
  
  // Simulation.
  void rubiscoN_distribution (const Units&,
                              const std::vector <double>& PAR_height,
			      const double LAI, const double DS,
			      std::vector <double>& rubiscoNdist, 
			      const double cropN/*[g]*/, Treelog& msg);
  void output (Log&) const
  { }

  // Create.
  public:
  rubiscoNdistUniform (const BlockModel& al)
    : RubiscoNdist (al),
       f_photo (al.number ("f_photo"))
  { }
};

void
rubiscoNdistUniform::rubiscoN_distribution (const Units&,
                                            const std::vector <double>& PAR_height,
					    const double LAI, const double DS,
					    std::vector <double>& rubiscoNdist, 
					    const double cropN/*[g]*/, Treelog& msg)
{
  daisy_assert (std::isfinite (cropN));
  daisy_assert (cropN >= 0.0);
  
  // Crop N in top of the canopy:
  double cropN0 = cropN; // [g/mÂ² area]
  cropN0 = cropN0 / Mw;  // [mol/mÂ² area]
  cropN0 = cropN0 / LAI; // [mol/mÂ² leaf] 
  daisy_assert (cropN0 >= 0.0);

  // Fill photosynthetically active N (cummulative) for each canopy layer in vector
  const int No = rubiscoNdist.size ();

  for (int i = 0; i < No; i++)
     rubiscoNdist[i] = f_photo * cropN0; //[mol/mÂ² leaf]

}

static struct rubiscoNdistUniformSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new rubiscoNdistUniform (al); }
  rubiscoNdistUniformSyntax ()
    : DeclareModel (RubiscoNdist::component, "uniform", 
	       "Uniform  rubisco N-distribution model in the canopy for photosynthesis.")
  { }
  void load_frame (Frame& frame) const
  {
    frame.declare ("f_photo", Attribute::None (), Check::positive (), Attribute::Const,
                "Fraction of photosynthetically active N in canopy, f_photo = 0.75 (Boegh et al., 2002). However, non-functional N is already substracted from leaf-N in the cropN_std module, therefore f_photo = 1.0 as default.");
    frame.set ("f_photo", 1.0);

  }
} rubiscoNdistUniform_syntax;


