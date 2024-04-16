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

dai_system_test(ref) # Default transport is Hansen
dai_system_test(transport-convection)
dai_system_test(transport-none)
dai_system_test(uzrect-mollerup)
dai_system_test(uzrect-vh)
dai_system_test(uzrect-vh-richards)
dai_system_test(uzrect-const)
dai_system_test(uzmodel-lr)
dai_system_test(uzmodel-lr-2)
dai_system_test(uzmodel-none)
dai_system_test(uzmodel-richards2)
dai_system_test(heatrect-top)
dai_system_test(heatrect-none)
dai_system_test(heatrect-linear)
dai_system_test(tertiary-biopores)
dai_system_test(drain-lateral)
dai_system_test(groundwater-lysimeter)
dai_system_test(groundwater-flux)
dai_system_test(groundwater-deep)
dai_system_test(groundwater-fixed)
