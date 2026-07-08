cxx_unit_test(ut_symbol)
cxx_unit_test(ut_plf)
cxx_unit_test(ut_type)
cxx_unit_test(ut_unit)
cxx_unit_test(ut_units)
cxx_unit_test(ut_oldunits)
cxx_unit_test(ut_treelog)
cxx_unit_test(ut_attribute)
cxx_unit_test(ut_check)
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
