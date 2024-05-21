#ifndef SOLVER_UBLAS_H
#define SOLVER_UBLAS_H
#include "util/solver.h"

struct SolverUBLAS : public Solver
{ 
  void solve (Matrix& A, const Vector& b, Vector& x) const; // Solve Ax=b

  SolverUBLAS (symbol type_name);
  SolverUBLAS (const BlockModel& al);
};
#endif
