// bdconv.C -- Bulk density conversion for select models.
// 
// Copyright 2008 Per Abrahamsen and KVL.
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

#include "daisy/output/bdconv.h"
#include "daisy/soil/transport/geometry.h"
#include "daisy/soil/soil.h"
#include "daisy/soil/transport/volume.h"
#include "object_model/units.h"
#include "util/assertion.h"
#include "util/mathlib.h"
#include <sstream>

double
BD_convert::operator()(double value) const
{ 
  daisy_assert (bulk > 0.0);
  return out (in (value) / bulk); 
}

bool
BD_convert::valid (double value) const
{
  daisy_assert (bulk > 0.0);
  return in.valid (value) && out.valid (in (value) / bulk);
}

BD_convert::BD_convert (const Units& units, const symbol has, const symbol want,
                        const symbol bulk_unit)
  : in (units.get_convertion (has, bulk_unit)),
    out (units.get_convertion (Units::dry_soil_fraction (), want)),
    bulk (-42.42e42)
{
}

// bdconv.C ends here.
