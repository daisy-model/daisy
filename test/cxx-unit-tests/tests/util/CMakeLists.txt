cxx_unit_test(ut_scope_exchange
  ${CMAKE_SOURCE_DIR}/src/util/scope_exchange.C
  ${CMAKE_SOURCE_DIR}/src/util/scope_model.C
)
cxx_unit_test(ut_iterative
  ${CMAKE_SOURCE_DIR}/src/util/iterative.C
)
cxx_unit_test(ut_mathlib)
cxx_unit_test(ut_solver_ublas
  ${CMAKE_SOURCE_DIR}/src/util/solver_ublas.C
  ${CMAKE_SOURCE_DIR}/src/util/solver.C
)

cxx_unit_test(ut_solver_cxsparse
  ${CMAKE_SOURCE_DIR}/src/util/solver_cxsparse.C
  ${CMAKE_SOURCE_DIR}/src/util/solver.C
)
