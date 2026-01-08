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
      ENVIRONMENT_MODIFICATION
      "DAISYHOME=set:${CMAKE_SOURCE_DIR};PYTHONPATH=string_prepend:${CMAKE_SOURCE_DIR}/sample/python"
    )
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
      ENVIRONMENT_MODIFICATION
      "DAISYHOME=set:${CMAKE_SOURCE_DIR};PYTHONPATH=string_prepend:${CMAKE_SOURCE_DIR}/sample/python"
    )
  endfunction()
endif()

add_subdirectory(tests)
