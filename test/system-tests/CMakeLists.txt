function(dai_test_case name dai_file)
  add_test(NAME systems_test.${name}
    COMMAND
    test_daisy
    ${CMAKE_BINARY_DIR}/daisy
    ${CMAKE_SOURCE_DIR}/test/system-tests/tests/${dai_file}
    ${CMAKE_SOURCE_DIR}/test/system-tests/baseline/${name}
    ${CMAKE_SOURCE_DIR}/test/system-tests/errors/${name}
  )
  set_property(TEST systems_test.${name} PROPERTY ENVIRONMENT "DAISYHOME=${CMAKE_SOURCE_DIR}")
endfunction()

dai_test_case(sample sample.dai)
dai_test_case(sample_2 sample-2.dai)
dai_test_case(test_cp test-cp.dai)
dai_test_case(test_GP2D test-GP2D.dai)
dai_test_case(test_maxroot test-maxroot.dai)
dai_test_case(test_minimize test-minimize.dai)
dai_test_case(test_organic test-organic.dai)
dai_test_case(test_refman test-refman.dai)
dai_test_case(test_scn test-scn.dai)
