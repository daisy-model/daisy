if(${OS} STREQUAL "mingw")
  function(dai_system_test name dir)
    add_test(NAME dai_system_test.${dir}.${name}
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
      dai_system_test.${dir}.${name}
      PROPERTIES
      LABELS "dai-system"
      ENVIRONMENT_MODIFICATION
      "DAISYHOME=set:${CMAKE_SOURCE_DIR};PYTHONPATH=string_prepend:${CMAKE_SOURCE_DIR}/sample/python"
    )
    if(_test_name IN_LIST _disabled_dai_system_tests)
      message(STATUS "Disabling ${_test_name} on ${OS}: known-failure")
      set_tests_properties(
        ${_test_name}
        PROPERTIES
        DISABLED TRUE
        LABELS "dai-system;disabled;${OS};known-failure"
      )
    endif()
  endfunction()
else()
  function(dai_system_test name dir)
    add_test(NAME dai_system_test.${dir}.${name}
      COMMAND
      ${CMAKE_SOURCE_DIR}/scripts/run_dai_system_test.sh
      ${_daisy_test_exe}
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/${dir}/test-${name}.dai
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/baseline/${dir}/${name}
      ${CMAKE_SOURCE_DIR}/test/dai-system-tests/errors/${dir}/${name}
      --path ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/common
    )
    set_tests_properties(
      dai_system_test.${dir}.${name}
      PROPERTIES
      LABELS "dai-system"
      ENVIRONMENT_MODIFICATION
      "DAISYHOME=set:${CMAKE_SOURCE_DIR};PYTHONPATH=string_prepend:${CMAKE_SOURCE_DIR}/sample/python"
    )
    if(_test_name IN_LIST _disabled_dai_system_tests)
      message(STATUS "Disabling ${_test_name} on ${OS}: known-failure")
      set_tests_properties(
        ${_test_name}
        PROPERTIES
        DISABLED TRUE
        LABELS "dai-system;disabled;${OS};known-failure"
      )
    endif()
  endfunction()
endif()

add_subdirectory(tests)
