if(${OS} STREQUAL "mingw")
  function(dai_unit_test name)
    add_test(NAME dai_unit_test.${name}
      COMMAND
      C:/msys64/usr/bin/bash.exe
      ${CMAKE_SOURCE_DIR}/scripts/run_dai_unit_test.sh
      ${_daisy_test_exe}
      ${CMAKE_SOURCE_DIR}/test/dai-unit-tests/tests/test-${name}.dai
      --path ${CMAKE_SOURCE_DIR}/test/dai-unit-tests/tests/common
    )
    set_property(TEST dai_unit_test.${name} PROPERTY ENVIRONMENT "DAISYHOME=${CMAKE_SOURCE_DIR}")
    set_property(TEST dai_unit_test.${name} PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_SOURCE_DIR}/sample/python")
  endfunction()
else()
  function(dai_unit_test name)
    add_test(NAME dai_unit_test.${name}
      COMMAND
      ${CMAKE_SOURCE_DIR}/scripts/run_dai_unit_test.sh
      ${_daisy_test_exe}
      ${CMAKE_SOURCE_DIR}/test/dai-unit-tests/tests/test-${name}.dai
      --path ${CMAKE_SOURCE_DIR}/test/dai-unit-tests/tests/common
    )
    set_property(TEST dai_unit_test.${name} PROPERTY ENVIRONMENT "DAISYHOME=${CMAKE_SOURCE_DIR}")
    set_property(TEST dai_unit_test.${name} PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_SOURCE_DIR}/sample/python")
  endfunction()
endif()
dai_unit_test(CO2-mul)
dai_unit_test(document)
