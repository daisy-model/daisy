// registration.C -- Explicit registration for object_model models.
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

#include "object_model/object_model_registration.h"
#include "object_model/object_model_registration_internal.h"

void
register_object_model_models ()
{
  register_parser_models ();
  register_parser_file_models ();
  register_function_models ();
  register_number_models ();
  register_stringer_models ();
  register_boolean_models ();
  register_integer_models ();
}
