// scope_id.C -- A name -> value map.
// 
// Copyright 2004 Per Abrahamsen and KVL.
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

#include "util/scope_id.h"
#include "util/assertion.h"
#include "util/mathlib.h"

void 
ScopeID::entries (std::set<symbol>& all) const
{ all.insert (tag); }

Attribute::type 
ScopeID::lookup (const symbol name) const
{ return (name == tag) ? Attribute::Number : Attribute::Error; }

bool 
ScopeID::check (const symbol name) const
{ return name == tag && std::isfinite (value); }

double 
ScopeID::number (symbol) const
{ return value; }

symbol 
ScopeID::dimension (symbol) const
{ return dim;}

symbol
ScopeID::description (symbol tag) const
{ return symbol ("Use '" + tag.name () + "' as a free variable"); }

  // WScope interface.
void
ScopeID::set (symbol, double val)
{ value = val; }

void
ScopeID::set_dimension (symbol, symbol d)
{ dim = d; }

ScopeID::ScopeID (const symbol name, const symbol d)
  : tag (name),
    value (NAN),
    dim (d)
{ }

ScopeID::~ScopeID ()
{ }

// scope_id.C ends here
