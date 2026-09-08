add_subdirectory(soil)
add_subdirectory(upper_boundary)

cxx_unit_test(ut_daisy_time)
cxx_unit_test(ut_timestep)

set(_cxx_known_failures_file
  "${CMAKE_CURRENT_BINARY_DIR}/cxx_known_failures.cmake")
file(WRITE "${_cxx_known_failures_file}" [=[
set_tests_properties(
  cxx_unit_test.TimeTest.NowTest
  cxx_unit_test.TimeTest.WDayTest
  cxx_unit_test.TimeTest.YDay2WDayTest
  cxx_unit_test.TimeTest.YDay2WeekTest
  cxx_unit_test.TimeTest.ComponentValueTest
  PROPERTIES
  LABELS "cxx-unit;known-failure"
)
]=])
set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES
  "${_cxx_known_failures_file}")
