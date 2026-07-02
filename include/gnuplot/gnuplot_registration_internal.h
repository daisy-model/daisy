// gnuplot_registration_internal.h -- Internal gnuplot registration declarations.
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

#ifndef GNUPLOT_REGISTRATION_INTERNAL_H
#define GNUPLOT_REGISTRATION_INTERNAL_H

void register_gnuplot_core_models ();
void register_gnuplot_base_models ();
void register_gnuplot_multi_models ();
void register_gnuplot_profile_models ();
void register_gnuplot_program_models ();
void register_gnuplot_soil_models ();
void register_gnuplot_source_models ();
void register_gnuplot_time_models ();
void register_gnuplot_vector_models ();
void register_gnuplot_xysource_models ();
void register_gnuplot_xy_models ();

#endif // GNUPLOT_REGISTRATION_INTERNAL_H
