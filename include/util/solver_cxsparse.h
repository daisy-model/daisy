#ifndef SOLVER_CXSPARSE_H
#define SOLVER_CXSPARSE_H
#include "util/solver.h"

struct SolverCXSparse : public Solver
{ 
  void solve (Matrix& A, const Vector& b, Vector& x) const; // Solve Ax=b

  SolverCXSparse (symbol type_name);
  SolverCXSparse (const BlockModel& al);
};
#endif
