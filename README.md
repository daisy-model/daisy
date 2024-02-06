# Daisy

Refactor of Daisy from https://github.com/perabrahamsen/daisy-model

## Building

### Linux
Install dependencies
* cmake
* boost
* suite-sparse 

[CMakePresets.json](CMakePresets.json) define setups for building with gcc and clang on linux.

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release

To see a list of available presets

    cmake ../../ --list-presets

To build with gcc using release options

    cmake ../../ --preset gcc-release
    make
    
To build a portable statically linked binary

    cmake ../../ --preset gcc-release-portable
    make
    
### MacOS
Install dependencies

    brew install cmake boost suite-sparse 
    
Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release
    
Build it

    cmake ../../ --preset clang-release
    make


## Testing
Test scenarios are in [test/dai_test_files](test/dai_test_files).
When adding a test you need to add it as a `dai_test_case` in [test/CMakeLists.txt](test/CMakeLists.txt).

Use [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) to run tests.

    ctest

## Code coverage
To get code coverage you need to set the build type to `Coverage`. This will disable some optimizations.
There is a coverage preset defined for gcc.

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/coverage 
    cd daisy/build/coverage
    cmake ../.. --preset gcc-coverage
    make -j 8
    ctest

Use https://gcovr.com/en/stable/ to summarize coverage

    pip install gcovr
    cd <coverage-build-dir>
    gcovr -r ../../ . --html ../../test/coverage.html
