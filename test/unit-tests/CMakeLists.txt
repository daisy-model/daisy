include(GoogleTest)

add_library(daisy_core SHARED
  ${CMAKE_SOURCE_DIR}/src/util/assertion.C
  ${CMAKE_SOURCE_DIR}/src/util/filepos.C
  ${CMAKE_SOURCE_DIR}/src/util/function.C
  ${CMAKE_SOURCE_DIR}/src/util/mathlib.C
  ${CMAKE_SOURCE_DIR}/src/util/path.C
  ${CMAKE_SOURCE_DIR}/src/util/scope.C
  ${CMAKE_SOURCE_DIR}/src/object_model/attribute.C
  ${CMAKE_SOURCE_DIR}/src/object_model/block.C
  ${CMAKE_SOURCE_DIR}/src/object_model/block_model.C
  ${CMAKE_SOURCE_DIR}/src/object_model/block_nested.C
  ${CMAKE_SOURCE_DIR}/src/object_model/block_submodel.C
  ${CMAKE_SOURCE_DIR}/src/object_model/block_top.C
  ${CMAKE_SOURCE_DIR}/src/object_model/check.C
  ${CMAKE_SOURCE_DIR}/src/object_model/convert.C
  ${CMAKE_SOURCE_DIR}/src/object_model/frame.C
  ${CMAKE_SOURCE_DIR}/src/object_model/frame_model.C
  ${CMAKE_SOURCE_DIR}/src/object_model/frame_submodel.C
  ${CMAKE_SOURCE_DIR}/src/object_model/intrinsics.C
  ${CMAKE_SOURCE_DIR}/src/object_model/librarian.C
  ${CMAKE_SOURCE_DIR}/src/object_model/library.C
  ${CMAKE_SOURCE_DIR}/src/object_model/metalib.C
  ${CMAKE_SOURCE_DIR}/src/object_model/model.C
  ${CMAKE_SOURCE_DIR}/src/object_model/oldunits.C
  ${CMAKE_SOURCE_DIR}/src/object_model/plf.C
  ${CMAKE_SOURCE_DIR}/src/object_model/symbol.C
  ${CMAKE_SOURCE_DIR}/src/object_model/treelog.C
  ${CMAKE_SOURCE_DIR}/src/object_model/treelog_text.C
  ${CMAKE_SOURCE_DIR}/src/object_model/type.C
  ${CMAKE_SOURCE_DIR}/src/object_model/unit.C
  ${CMAKE_SOURCE_DIR}/src/object_model/unit_model.C
  ${CMAKE_SOURCE_DIR}/src/object_model/units.C
  ${CMAKE_SOURCE_DIR}/src/object_model/value.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/boolean.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number.C
  ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/stringer.C
)

target_include_directories(daisy_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(daisy_core PRIVATE ${COMPILE_OPTIONS})
target_link_options(daisy_core PRIVATE ${LINKER_OPTIONS})
target_link_libraries(daisy_core PUBLIC
  cxsparse
  Boost::filesystem
)

function(add_unit_test name)
  add_executable(${name} tests/${name}.C ${ARGN})
  target_include_directories(${name} PUBLIC ${CMAKE_SOURCE_DIR}/include)
  target_compile_options(${name} PRIVATE ${COMPILE_OPTIONS})
  target_link_options(${name} PRIVATE ${LINKER_OPTIONS})
  target_link_libraries(${name} PUBLIC
    daisy_core
    gtest
    gtest_main
  )
  gtest_discover_tests(${name} TEST_PREFIX unit_test.)
endfunction()



add_unit_test(ut_units)
add_unit_test(ut_scope_exchange
  ${CMAKE_SOURCE_DIR}/src/util/scope_exchange.C
  ${CMAKE_SOURCE_DIR}/src/util/scope_model.C
)
add_unit_test(ut_iterative
  ${CMAKE_SOURCE_DIR}/src/util/iterative.C
)
add_unit_test(ut_mathlib)
