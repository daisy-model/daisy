// organic.C -- Soil organic matter.
// 
// Copyright 2006 Per Abrahamsen and KVL.
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

#include "daisy/organic_matter/organic.h"
#include "daisy/daisy_registration_internal.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"

const char *const OrganicMatter::component = "organic";

symbol
OrganicMatter::library_id () const
{
  static const symbol id (component);
  return id;
}

OrganicMatter::OrganicMatter (const BlockModel& al)
  : ModelDerived (al.type_name ())
{ }

OrganicMatter::~OrganicMatter ()
{ }

void
register_organic_matter_models ()
{
  static struct OrganicInit : public DeclareComponent
  {
    OrganicInit ()
      : DeclareComponent (OrganicMatter::component, "\
Turnover of organic matter in the soil.")
    { }
  } organic_init;

  register_aom_models ();
  register_som_models ();
  register_smb_models ();
  register_dom_models ();
  register_doe_models ();
  register_domsorp_models ();
  register_clay_om_models ();
  register_am_models ();
  register_bioincorporation_models ();
  register_organic_standard_models ();
  register_organic_none_models ();
}

// organic.C ends here.
