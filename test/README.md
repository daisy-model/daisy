# Testing Daisy
We have three types of tests

1. C++ unit tests (Only on Linux for now, depends on googletest)
2. dai unit tests
3. dai system tests

## Running tests
ctest is used to run all types of test. It is possible to run a single type of test by using ctests pattern matching. For example, to run only dai based unit test you would use

    ctest -R dai_unit_test
    
## Writing tests

### C++ unit tests
C++ unit tests use `googletest`. They are in `cxx-unit-tests/tests`. They should be named `ut_<name-of-source-file-that-is-tested>.C` and should cover the entire source file and any functions defined in the corresponding header.

A unit test is added to the test suite by adding to the end of `cxx-unit-tests/CMakeLists.txt` using the function `cxx_unit_test` with the signature

    cxx_unit_test(test_file_name [extra_sources]*)
    
where `test_file_name` is the name of the test file without the file extension.

A subset of Daisy is built as shared library that tests are linked against. If more sources are needed these are added as `extra_sources` in the `cxx_unit_test` function. For example,

    cxx_unit_test(ut_scope_exchange 
      ${CMAKE_SOURCE_DIR}/src/util/scope_exchange.C 
      ${CMAKE_SOURCE_DIR}/src/util/scope_model.C
    )

### dai unit tests
dai unit tests are in `dai-unit-tests/tests`. They should be named `test-<name>.dai` and should signal failure with a non-zero exit code and success with an exit code of zero. 

A unit test is added to the test suite by adding it to the end of `dai-unit-tests/CMakeLists.txt` using the function `dai_unit_test` with the signature

    dai_unit_test(name)
    
where `name` is the `<name>` part of the file name. Common dai files used for multiple tests can be placed `dai-unit-tests/tests/common` which is added to the Daisy path.

### dai system tests
dai system tests are in `dai-system-tests/tests`. The organization follows the `src/` directory. Test should be named `test-<descriptive-name>.dai`. The output logs (.dlf) from running the test are compared against a baseline output in `dai-system-tests/baseline/`. Baseline files should be generated with a 'portable' build type and the output verified manually.

Files that do not match the baseline during testing are copied to `dai-system-tests/errors/`.

A system test is added to the test suite by adding it to the `CMakeLists.txt` in the corresponding directory using the function `dai_system_test` with the signature

    dai_system_test(name dir)
    
where `name` is the `<descriptive-name>` part of the file name and `dir` is the directory the test is stored relative to `test/dai-system-tests/tests/`. Common dai files used for multiple tests can be placed `dai-system-tests/tests/common` which is added to the Daisy path.

#### Example: Adding a test using uzrect Mollerup component
1. Create a dai test file in `test/dai-system-tests/tests/daisy/soil/transport/test-uzrect-mollerup.dai`
2. Add the line `dai_system_test(uzrect-mollerup daisy/soil/transport)` to `test/dai-system-tests/tests/daisy/soil/transport/CMakeLists.txt`
3. Generate baseline from a portable build using the script `test/dai-system-tests/generate_baseline.sh`

        bash generate_baseline.sh <path-to-daisy-binary> uzrect-mollerup daisy/soil/transport

4. Verify baseline output and delete any files that should not be compared, e.g. daisy.log


## Coverage
There are scripts for generating coverage reports

### Coverage from dai system tests
Run `test/coverage/dai_system_test_coverage.sh` from the build directory. This will run all dai system tests and summarize how they cover all source files. 

Use `test/coverage/dai_system_test_coverage_detals.sh` to generate a detailed coverage report for a subset of the codebase. For example, to get a detailed report for all files under `daisy/bioclimate` you would do

    test/coverage/dai_system_test_coverage_detals.sh bioclimate

### Coverage from C++ unit tests
Run `test/coverage/cxx_unit_test_coverage.sh` from the build directory. This will run all C++ unit tests and summarize how they cover the corresponding source files. The summary is stored in `test/coverage/cxx-unit-tests/coverage.html` and contains annotated source code.
