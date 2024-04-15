#include "block_model.h"
#include "../../../src/util/solver_ublas.C"

#include <gtest/gtest.h>

TEST(SolverUBLASTest, solve) {
  BlockModel block_model;
  SolverUBLAS solver = SolverUBLAS(block_model);

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
