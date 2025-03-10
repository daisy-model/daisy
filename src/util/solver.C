// solver.C -- Solve matrix equation Ax=b.
// 
// Copyright 2007 Per Abrahamsen and KVL.
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

#include "util/solver.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"

const char *const Solver::component = "solver";

symbol
Solver::library_id () const
{
  static const symbol id (component);
  return id;
}

void 
Solver::Matrix::resize (size_t size)
{ SMatrix_type::resize (size, size, false); }

Solver::Matrix::Matrix (const size_t size)
  : SMatrix_type (size, size)
{
#ifdef USE_DENSE_MATRIX
  *this = ublas::zero_matrix<double> (size, size); 
#endif // USE_DENSE_MATRIX
}

Solver::Solver (symbol type_name)
  : name (type_name)
{ }


Solver::Solver (const BlockModel& al)
  : Solver (al.type_name ())
{ }

Solver::~Solver ()
{ }

static struct SolverInit : public DeclareComponent 
{
  SolverInit ()
    : DeclareComponent (Solver::component, "\
A way to solve the matrix equation 'A x = b'.")
  { }
} Solver_init;

// solver.C ends here.
