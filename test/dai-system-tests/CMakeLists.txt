set(_disabled_dai_system_tests)
if(${OS} STREQUAL "macos")
  list(APPEND _disabled_dai_system_tests
    "dai_system_test.daisy/lower_boundary.groundwater-deep"
    "dai_system_test.programs.hydraulic"
    "dai_system_test.programs.spawn"
  )
elseif(${OS} STREQUAL "mingw")
  list(APPEND _disabled_dai_system_tests
    "dai_system_test.programs.hydraulic"
    "dai_system_test.programs.spawn"
  )
elseif(${OS} STREQUAL "linux")
  list(APPEND _disabled_dai_system_tests
    "dai_system_test.programs.hydraulic"
    "dai_system_test.programs.spawn"
  )
endif()

if(${OS} STREQUAL "mingw")
  function(dai_system_test name dir)
    set(_test_name "dai_system_test.${dir}.${name}")
    add_test(NAME ${_test_name}
      COMMAND
      C:/msys64/usr/bin/bash.exe
      ${CMAKE_SOURCE_DIR}/scripts/run_dai_system_test.sh
      ${_daisy_test_exe}
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/${dir}/test-${name}.dai
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/baseline/${dir}/${name}
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/errors/${dir}/${name}
      --path ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/common
    )
    set_tests_properties(
      ${_test_name}
      PROPERTIES
      ENVIRONMENT_MODIFICATION
      "DAISYHOME=set:${CMAKE_SOURCE_DIR};PYTHONPATH=string_prepend:${CMAKE_SOURCE_DIR}/sample/python"
    )
    if(_test_name IN_LIST _disabled_dai_system_tests)
      message(STATUS "Disabling ${_test_name} on ${OS}: known-failure")
      set_tests_properties(
        ${_test_name}
        PROPERTIES
        DISABLED TRUE
        LABELS "disabled;${OS};known-failure"
      )
    endif()
  endfunction()
else()
  function(dai_system_test name dir)
    set(_test_name "dai_system_test.${dir}.${name}")
    add_test(NAME ${_test_name}
      COMMAND
      ${CMAKE_SOURCE_DIR}/scripts/run_dai_system_test.sh
      ${_daisy_test_exe}
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/${dir}/test-${name}.dai
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/baseline/${dir}/${name}
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/errors/${dir}/${name}
      --path ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/common
    )
    set_tests_properties(
      ${_test_name}
      PROPERTIES
      ENVIRONMENT_MODIFICATION
      "DAISYHOME=set:${CMAKE_SOURCE_DIR};PYTHONPATH=string_prepend:${CMAKE_SOURCE_DIR}/sample/python"
    )
    if(_test_name IN_LIST _disabled_dai_system_tests)
      message(STATUS "Disabling ${_test_name} on ${OS}: known-failure")
      set_tests_properties(
        ${_test_name}
        PROPERTIES
        DISABLED TRUE
        LABELS "disabled;${OS};known-failure"
      )
    endif()
  endfunction()
endif()

add_subdirectory(tests)
