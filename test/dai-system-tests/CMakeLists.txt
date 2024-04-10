function(dai_system_test name)
  add_test(NAME dai_system_test.${name}
    COMMAND
    test_daisy
    ${CMAKE_BINARY_DIR}/daisy
    ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/test-${name}.dai
    ${CMAKE_SOURCE_DIR}/test/dai-system-tests/baseline/${name}
    ${CMAKE_SOURCE_DIR}/test/dai-system-tests/errors/${name}
    --path ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/common
  )
  set_property(TEST dai_system_test.${name} PROPERTY ENVIRONMENT "DAISYHOME=${CMAKE_SOURCE_DIR}")
endfunction()

dai_system_test(ref)
