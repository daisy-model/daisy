# Build a standalone package-verification test bundle.
#
# The bundle is meant to be downloaded alongside a packaged Daisy build and run
# without CTest metadata from the original build tree. It contains the dai test
# files, baselines, Python helper modules, known-failure metadata, and the
# standalone packaged-test runner.

if(NOT DEFINED DAISY_SOURCE_DIR)
  message(FATAL_ERROR "DAISY_SOURCE_DIR is required")
endif()

if(NOT DEFINED DAISY_BUNDLE_DIR)
  message(FATAL_ERROR "DAISY_BUNDLE_DIR is required")
endif()

if(NOT DEFINED DAISY_VERSION)
  message(FATAL_ERROR "DAISY_VERSION is required")
endif()

set(_bundle_name "daisy-test-suite-${DAISY_VERSION}")
set(_bundle_root "${DAISY_BUNDLE_DIR}/${_bundle_name}")
set(_bundle_archive "${DAISY_BUNDLE_DIR}/${_bundle_name}.zip")

file(REMOVE_RECURSE "${_bundle_root}")
file(REMOVE "${_bundle_archive}")
file(MAKE_DIRECTORY "${_bundle_root}")

file(COPY
  "${DAISY_SOURCE_DIR}/test/dai-unit-tests/tests"
  DESTINATION "${_bundle_root}/dai-unit-tests"
)
file(COPY
  "${DAISY_SOURCE_DIR}/test/dai-system-tests/tests"
  DESTINATION "${_bundle_root}/dai-system-tests"
)
file(COPY
  "${DAISY_SOURCE_DIR}/test/dai-system-tests/baseline"
  DESTINATION "${_bundle_root}/dai-system-tests"
)
file(COPY
  "${DAISY_SOURCE_DIR}/sample/python"
  DESTINATION "${_bundle_root}/sample"
)
file(COPY
  "${DAISY_SOURCE_DIR}/test/package/known_failures.json"
  DESTINATION "${_bundle_root}/metadata"
)
file(COPY
  "${DAISY_SOURCE_DIR}/scripts/run_packaged_tests.py"
  DESTINATION "${_bundle_root}"
)

file(CHMOD
  "${_bundle_root}/run_packaged_tests.py"
  PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

file(WRITE "${_bundle_root}/bundle-info.json"
  "{\n"
  "  \"bundle_version\": \"1\",\n"
  "  \"daisy_version\": \"${DAISY_VERSION}\",\n"
  "  \"dai_unit_tests_dir\": \"dai-unit-tests/tests\",\n"
  "  \"dai_system_tests_dir\": \"dai-system-tests/tests\",\n"
  "  \"dai_system_baseline_dir\": \"dai-system-tests/baseline\",\n"
  "  \"python_helpers_dir\": \"sample/python\",\n"
  "  \"known_failures\": \"metadata/known_failures.json\",\n"
  "  \"runner\": \"run_packaged_tests.py\"\n"
  "}\n"
)

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E tar cf "${_bundle_archive}" --format=zip "${_bundle_name}"
  WORKING_DIRECTORY "${DAISY_BUNDLE_DIR}"
  COMMAND_ERROR_IS_FATAL ANY
)
