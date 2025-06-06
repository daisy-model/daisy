 // bioclimate.C
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

#define BUILD_DLL

#include "daisy/upper_boundary/bioclimate/bioclimate.h"
#include "daisy/upper_boundary/weather/weather.h"
#include "object_model/block_model.h"
#include "util/mathlib.h"
#include "object_model/librarian.h"

const char *const Bioclimate::component = "bioclimate";

symbol 
Bioclimate::library_id () const
{
  static const symbol id (component);
  return id;
}

double 
Bioclimate::day_fraction (const double dt) const
{
  if (daily_global_radiation () > 0.0)
    return (dt * global_radiation ())/ (24.0 * daily_global_radiation ());
  return 0.0;
}

void
Bioclimate::radiation_distribution (const size_t No, const double LAI,
                                    const double Ref,
                                    const double Si,
                                    const double Ext,
                                    std::vector <double>& Rad)
{
  // Fraction of Photosynthetically Active Radiation in Shortware
  // incoming radiation. 
  static const double PARinSi = 0.50;	

  const double PAR0 = (1 - Ref) * PARinSi * Si;
  intensity_distribution (No, LAI, PAR0, Ext, Rad);
}

void
Bioclimate::intensity_distribution (const size_t No, const double LAI,
                                    const double Rad0,
                                    const double Ext,
                                    std::vector <double>& Rad)
{
  daisy_assert (Rad.size () == No + 1);
  const double dLAI = (LAI / (No + 0.0));
    
  for (size_t i = 0; i <= No; i++)
    Rad[i] = Rad0 * exp (- Ext * dLAI * i);
}

Bioclimate::Bioclimate (const BlockModel& al)
  : ModelFramed (al)
{ }

Bioclimate::~Bioclimate ()
{ }

static struct BioclimateInit : public DeclareComponent 
{
  BioclimateInit ()
    : DeclareComponent (Bioclimate::component, "\
The 'bioclimate' component is responsible for distributing the water\n\
and energy provided by the weather component among the crops and soil\n\
for a given column.")
  { }
} Bioclimate_init;

// bioclimate.C ends here.
