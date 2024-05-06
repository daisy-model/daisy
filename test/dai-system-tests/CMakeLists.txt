function(dai_system_test name dir)
  add_test(NAME dai_system_test.${dir}.${name}
    COMMAND
    test_daisy
    ${CMAKE_BINARY_DIR}/daisy
    ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/${dir}/test-${name}.dai
    ${CMAKE_SOURCE_DIR}/test/dai-system-tests/baseline/${dir}/${name}
    ${CMAKE_SOURCE_DIR}/test/dai-system-tests/errors/${dir}/${name}
    --path ${CMAKE_SOURCE_DIR}/test/dai-system-tests/tests/common
  )
  set_property(TEST dai_system_test.${dir}.${name}
    PROPERTY ENVIRONMENT "DAISYHOME=${CMAKE_SOURCE_DIR}")
endfunction()

add_subdirectory(tests)
