#include <gtest/gtest.h>

#include "util/solver_ublas.h"

TEST(SolverUBLASTest, solve) {
  symbol type_name = "solver_ublas";
  SolverUBLAS solver = SolverUBLAS (type_name);

  SolverUBLAS::Matrix A(2);
  SolverUBLAS::Vector b(2);
  SolverUBLAS::Vector x(2);

  A(0,0) = 1;
  A(0,1) = 2;
  A(1,0) = 2;
  A(1,1) = 1;

  b(0) = -1;
  b(1) = 1;

  solver.solve(A, b, x);
  EXPECT_FLOAT_EQ(x(0), 1);
  EXPECT_FLOAT_EQ(x(1), -1);
}
