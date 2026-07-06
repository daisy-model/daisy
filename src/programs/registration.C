// registration.C -- Explicit registration for program models.
//
// Copyright 2026 The Daisy Authors.
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

#include "programs/program_registration.h"
#include "programs/program_registration_internal.h"

void
register_program_models ()
{
  register_program_astdump_models ();
  register_program_GP2D_models ();
  register_program_KM2_models ();
  register_program_core_models ();
  register_program_batch_models ();
  register_program_cpedata_models ();
  register_program_document_models ();
  register_program_file_models ();
  register_program_hmovie_models ();
  register_program_nwaps_models ();
  register_program_optimize_models ();
  register_program_osvaldo_models ();
  register_program_post_models ();
  register_program_rootmatch_models ();
  register_program_sbrdata_models ();
  register_program_extract_models ();
  register_program_spawn_models ();
  register_program_weather_models ();
}
