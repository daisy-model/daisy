cxx_unit_test(ut_symbol)
cxx_unit_test(ut_plf)
cxx_unit_test(ut_function)
cxx_unit_test(ut_boolean)
cxx_unit_test(ut_parameter_types
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_const.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_plf.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_apply.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_lisp.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_arit.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/integer.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/integer_arit.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/stringer.C
  ${CMAKE_SOURCE_DIR}/src/util/scope_multi.C
)
cxx_unit_test(ut_rate
  ${CMAKE_SOURCE_DIR}/src/object_model/rate.C
)
cxx_unit_test(ut_format
  ${CMAKE_SOURCE_DIR}/src/util/format.C
  ${CMAKE_SOURCE_DIR}/src/util/format_LaTeX.C
  ${CMAKE_SOURCE_DIR}/src/object_model/version.C
)
cxx_unit_test(ut_scopesel
  ${CMAKE_SOURCE_DIR}/src/util/scopesel.C
)
cxx_unit_test(ut_library)
cxx_unit_test(ut_metalib)
cxx_unit_test(ut_type)
cxx_unit_test(ut_unit)
cxx_unit_test(ut_units)
cxx_unit_test(ut_oldunits)
cxx_unit_test(ut_treelog)
cxx_unit_test(ut_attribute)
cxx_unit_test(ut_check)
cxx_unit_test(ut_frame)
cxx_unit_test(ut_vcheck)
cxx_unit_test(ut_value)
cxx_unit_test(ut_xref
  ${CMAKE_SOURCE_DIR}/src/object_model/traverse.C
  ${CMAKE_SOURCE_DIR}/src/object_model/xref.C
)
target_link_libraries(ut_xref PUBLIC
  Boost::filesystem
)
if(TARGET Boost::system)
  target_link_libraries(ut_xref PUBLIC Boost::system)
endif()

set(_cxx_known_failures_file
  "${CMAKE_CURRENT_BINARY_DIR}/cxx_known_failures.cmake")
file(WRITE "${_cxx_known_failures_file}" [=[
set_tests_properties(
  cxx_unit_test.XRefTest.ModelUsersWithDifferentPathsStayDistinct
  cxx_unit_test.XRefTest.ModelUsersDeduplicateIdenticalEntriesAndOrderByPath
  cxx_unit_test.XRefTest.SubmodelUsersWithDifferentPathsStayDistinct
  cxx_unit_test.XRefTest.SubmodelUsersDefaultConstructAndOrderByPath
  PROPERTIES
  LABELS "cxx-unit;known-failure"
)
]=])
set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES
  "${_cxx_known_failures_file}")
