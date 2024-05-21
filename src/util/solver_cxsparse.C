// solver_cxspare.C -- Solve matrix equation Ax=b using CXSparse library.
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

#include "object_model/block_model.h"
#include "object_model/frame.h"
#include "object_model/librarian.h"

#include "util/solver_cxsparse.h"
#include "util/ublas_cxsparse.h" // Must be included after solver_cxsparse.h due to extern "C"


#define MEMCHECK(foo) if (!(foo)) throw "CXSparse: Bad matrix: " #foo ;

void SolverCXSparse::solve (Matrix& A, const Vector& b, Vector& x) const // Solve Ax=b
{
  // declare variable (T=double, size_type=unsigned int)
  CS::cxsparse_type_traits<double, size_t>::lu_type     cs_lu;

  // decompose
  cs_lu = CS::cs_ul_decompose (A, 1.0);

  // check
  MEMCHECK (cs_lu.first);
  MEMCHECK (cs_lu.first->q);
  MEMCHECK (cs_lu.second)
    MEMCHECK (cs_lu.second->U);
  MEMCHECK (cs_lu.second->L);
  MEMCHECK (cs_lu.second->pinv);

  // solve
  x = b;
  CS::cs_ul_solve (cs_lu, x);

  // free
  CS::cs_lu_free (cs_lu);
}

SolverCXSparse::SolverCXSparse(symbol type_name)
  : Solver (type_name)
{ }

SolverCXSparse::SolverCXSparse (const BlockModel& al)
  : SolverCXSparse (al.type_name ())
{ }


static struct SolverCXSparseSyntax : public DeclareModel
{
  Model* make (const BlockModel& al) const
  { return new SolverCXSparse (al); }
  SolverCXSparseSyntax ()
    : DeclareModel (Solver::component, "cxsparse", "\
Solve equation using CXSparse library described in:\n\
\n\
Direct Methods for Sparse Linear Systems, T. A. Davis, SIAM,\n\
Philadelphia, Sept. 2006. Part of the SIAM Book Series on the\n\
Fundamentals of Algorithms.\n\
\n\
The uBLAS interface was provided by Gunter Winkler <guwi17@gmx.de>.")
  { }
  void load_frame (Frame&) const
  {
  }
} SolverCXSparse_syntax;

// solver_cxsparse.C ends here.
