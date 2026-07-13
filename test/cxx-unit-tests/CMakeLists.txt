if (${BUILD_CXX_TESTS})
  find_package(GTest REQUIRED)
  include(GoogleTest)

  add_library(ut_core STATIC
    ${CMAKE_SOURCE_DIR}/src/daisy/daisy_time.C
    ${CMAKE_SOURCE_DIR}/src/daisy/timestep.C
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
    ${CMAKE_SOURCE_DIR}/src/object_model/model_derived.C
    ${CMAKE_SOURCE_DIR}/src/object_model/model_logable.C
    ${CMAKE_SOURCE_DIR}/src/object_model/oldunits.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/boolean.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/boolean_number.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/boolean_string.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/integer.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/integer_arit.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_apply.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_arit.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_const.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/number_plf.C
    ${CMAKE_SOURCE_DIR}/src/object_model/parameter_types/stringer.C
    ${CMAKE_SOURCE_DIR}/src/object_model/plf.C
    ${CMAKE_SOURCE_DIR}/src/object_model/symbol.C
    ${CMAKE_SOURCE_DIR}/src/object_model/treelog.C
    ${CMAKE_SOURCE_DIR}/src/object_model/treelog_store.C
    ${CMAKE_SOURCE_DIR}/src/object_model/treelog_text.C
    ${CMAKE_SOURCE_DIR}/src/object_model/type.C
    ${CMAKE_SOURCE_DIR}/src/object_model/unit.C
    ${CMAKE_SOURCE_DIR}/src/object_model/unit_model.C
    ${CMAKE_SOURCE_DIR}/src/object_model/units.C
    ${CMAKE_SOURCE_DIR}/src/object_model/value.C
    ${CMAKE_SOURCE_DIR}/src/object_model/vcheck.C
    ${CMAKE_SOURCE_DIR}/src/util/assertion.C
    ${CMAKE_SOURCE_DIR}/src/util/filepos.C
    ${CMAKE_SOURCE_DIR}/src/object_model/function.C
    ${CMAKE_SOURCE_DIR}/src/util/mathlib.C
    ${CMAKE_SOURCE_DIR}/src/util/path.C
    ${CMAKE_SOURCE_DIR}/src/util/scope.C
  )

  target_include_directories(ut_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
  target_compile_options(ut_core PRIVATE ${COMPILE_OPTIONS})
  target_link_options(ut_core PRIVATE ${LINKER_OPTIONS})
  if (${OS} STREQUAL "macos")
    target_link_directories(ut_core PUBLIC ${EXTRA_SYSTEM_INCLUDE_DIRECTORIES})
  endif()
  if (WIN32)
    target_compile_definitions(ut_core PUBLIC BUILD_DLL)
  endif()
  set(_ut_core_boost_libs Boost::filesystem)
  if(TARGET Boost::system)
    list(APPEND _ut_core_boost_libs Boost::system)
  endif()
  target_link_libraries(ut_core PUBLIC ${_ut_core_boost_libs})

  # function(cxx_unit_test_mock name)
  #   add_executable(${name} ${CMAKE_SOURCE_DIR}/test/cxx-unit-tests/tests/${name}.C ${ARGN})
  #   target_include_directories(${name} PUBLIC ${CMAKE_SOURCE_DIR}/test/cxx-unit-tests/mock/include)
  #   target_compile_options(${name} PRIVATE ${COMPILE_OPTIONS})
  #   target_link_options(${name} PRIVATE ${LINKER_OPTIONS})
  #   target_link_libraries(${name} PUBLIC
  #     cxsparse
  #     GTest::gtest
  #     GTest::gtest_main
  #   )
  #   gtest_discover_tests(${name} TEST_PREFIX cxx_unit_test.)
  # endfunction()

  function(cxx_unit_test name)
    add_executable(${name} ${name}.C ${ARGN})
    target_include_directories(${name} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_compile_options(${name} PRIVATE ${COMPILE_OPTIONS})
    target_link_options(${name} PRIVATE ${LINKER_OPTIONS})
    if (${OS} STREQUAL "macos")
      target_link_directories(${name} PRIVATE ${EXTRA_SYSTEM_INCLUDE_DIRECTORIES})
    endif()
    target_link_libraries(${name} PUBLIC
      ut_core
      cxsparse
      GTest::gtest
      GTest::gtest_main
    )
    gtest_discover_tests(${name}
      TEST_PREFIX cxx_unit_test.
      PROPERTIES LABELS "cxx-unit"
    )
  endfunction()

  add_subdirectory(tests)
endif()
