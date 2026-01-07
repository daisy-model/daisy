// tseries.C -- Time series.
// 
// Copyright 2025 Per Abrahamsen and KU.
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

#include "util/tseries.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"

const char *const TSeries::component = "tseries";

symbol
TSeries::library_id () const
{
  static const symbol id (component);
  return id;
}

TSeries::TSeries (symbol type_name)
  : name (type_name)
{ }


TSeries::TSeries (const BlockModel& al)
  : TSeries (al.type_name ())
{ }

TSeries::~TSeries ()
{ }

static struct TSeriesInit : public DeclareComponent 
{
  TSeriesInit ()
    : DeclareComponent (TSeries::component, "\
Map simulation to any number of data points.")
  { }
} TSeries_init;

// tseries.C ends here.
