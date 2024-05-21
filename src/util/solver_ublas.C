// solver_ublas.C -- Solve matrix equation Ax=b using ublas.
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

#include "util/solver_ublas.h"
#include "util/assertion.h"
#include "object_model/block_model.h"
#include "object_model/librarian.h"
#include "object_model/frame.h"

#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>


void SolverUBLAS::solve (Matrix& A, const Vector& b, Vector& x) const // Solve Ax=b
{
  namespace ublas = boost::numeric::ublas;

  const size_t size = b.size ();
  ublas::permutation_matrix<double> piv (size);
  const bool singular = ublas::lu_factorize(A, piv);
  daisy_assert (!singular);
  x = b; 			// x should contain b as a start.
  ublas::lu_substitute (A, piv, x);
}

SolverUBLAS::SolverUBLAS (symbol type_name)
  : Solver(type_name)
{ }

SolverUBLAS:: SolverUBLAS (const BlockModel& al)
  : Solver (al.type_name ())
{ }

static struct SolverUBLASSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new SolverUBLAS (al); }
  SolverUBLASSyntax ()
    : DeclareModel (Solver::component, "ublas", "Solve equation using UBLAS lu functions.")
  { }
  void load_frame (Frame&) const
  {
  }
} SolverUBLAS_syntax;

// solver_ublas.C ends here.
