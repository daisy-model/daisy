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

set(_cxx_known_failures_file
  "${CMAKE_CURRENT_BINARY_DIR}/cxx_known_failures.cmake")
file(WRITE "${_cxx_known_failures_file}" [=[
set_tests_properties(
  cxx_unit_test.MathlibTest.pF2h
  cxx_unit_test.MathlibTest.h2pF
  cxx_unit_test.MathlibTest.rate_to_halftime
  cxx_unit_test.MathlibTest.halftime_to_rate
  cxx_unit_test.MathlibTest.single_positive_root_of_square_equation
  cxx_unit_test.MathlibTest.single_positive_root_of_cubic_equation
  PROPERTIES
  LABELS "cxx-unit;known-failure"
)
]=])
set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES
  "${_cxx_known_failures_file}")
