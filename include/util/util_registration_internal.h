// util_registration_internal.h -- Internal util registration declarations.
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

#ifndef UTIL_REGISTRATION_INTERNAL_H
#define UTIL_REGISTRATION_INTERNAL_H

#include "util/format_registration.h"

void register_depth_models ();
void register_scope_exchange_models ();
void register_scope_models ();
void register_scopesel_models ();
void register_solver_cxsparse_models ();
void register_solver_models ();
void register_solver_none_models ();
void register_solver_ublas_models ();

#endif // UTIL_REGISTRATION_INTERNAL_H
