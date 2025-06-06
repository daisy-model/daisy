// unit.C -- Specify unit for scalar.
// 
// Copyright 2007, 2008 Per Abrahamsen and KVL.
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

#include "object_model/unit.h"

symbol
Unit::pressure ()
{
  static const symbol unit ("m^-1 kg s^-2");
  return unit;
}

symbol
Unit::mass_per_volume ()
{
  static const symbol unit ("m^-3 kg");
  return unit;
}

symbol
Unit::amount_of_substance_per_area_per_time ()
{
  static const symbol unit ("m^-2 s^-1 mol");
  return unit;
}

symbol
Unit::energy_per_area_per_time ()
{
  static const symbol unit ("kg s^-3");
  return unit;
}

symbol 
Unit::mass_per_area_per_time ()
{
  static const symbol unit ("m^-2 kg s^-1");
  return unit;
}

symbol
Unit::length_per_time ()
{
  static const symbol unit ("m s^-1");
  return unit;
}

Unit::Unit ()
{ }

Unit::~Unit ()
{ }

// unit.C ends here.
