cxx_unit_test_v2(ut_symbol
  ${CMAKE_SOURCE_DIR}/src/object_model/symbol.C
  ${CMAKE_SOURCE_DIR}/src/util/assertion.C
  ${CMAKE_SOURCE_DIR}/src/util/mathlib.C
)

cxx_unit_test_v2(ut_plf
  ${CMAKE_SOURCE_DIR}/src/object_model/plf.C
  ${CMAKE_SOURCE_DIR}/src/object_model/symbol.C
  ${CMAKE_SOURCE_DIR}/src/util/assertion.C
  ${CMAKE_SOURCE_DIR}/src/util/mathlib.C
)
