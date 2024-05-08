function(dai_unit_test name)
  add_test(NAME dai_unit_test.${name}
    COMMAND
    check_daisy
    ${CMAKE_BINARY_DIR}/daisy
    ${CMAKE_SOURCE_DIR}/test/dai-unit-tests/tests/test-${name}.dai
    --path ${CMAKE_SOURCE_DIR}/test/dai-unit-tests/tests/common
  )
  set_property(TEST dai_unit_test.${name} PROPERTY ENVIRONMENT "DAISYHOME=${CMAKE_SOURCE_DIR}")
endfunction()

dai_unit_test(CO2-mul)
dai_unit_test(document)
