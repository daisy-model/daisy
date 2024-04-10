function(dai_test_case name dai_file)
  add_test(NAME systems_test.${name}
    COMMAND
    test_daisy
    ${CMAKE_BINARY_DIR}/daisy
    ${CMAKE_SOURCE_DIR}/test/system-tests/tests/${dai_file}
    ${CMAKE_SOURCE_DIR}/test/system-tests/baseline/${name}
    ${CMAKE_SOURCE_DIR}/test/system-tests/errors/${name}
    --path ${CMAKE_SOURCE_DIR}/test/system-tests/tests/common
  )
  set_property(TEST systems_test.${name} PROPERTY ENVIRONMENT "DAISYHOME=${CMAKE_SOURCE_DIR}")
endfunction()

dai_test_case(ref test-ref.dai)
