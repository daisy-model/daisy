// registration.C -- Explicit registration for util models.
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

#include "util/util_registration.h"
#include "util/util_registration_internal.h"

void
register_util_models ()
{
  register_format_models ();
  register_scope_models ();
  register_scope_exchange_models ();
  register_depth_models ();
  register_scopesel_models ();
  register_solver_models ();
  register_solver_none_models ();
  register_solver_ublas_models ();
  register_solver_cxsparse_models ();
}
