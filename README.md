# Daisy

Refactor of Daisy from https://github.com/perabrahamsen/daisy-model

## Building
There are presets defined in `CMakePresets.json` for building on different platforms.

To see a list of available presets run

    cmake --list-presets

in the source dirrectory.

### Linux
Install dependencies
* cmake
* boost (filesystem and system)
* suitesparse 

Using apt

    apt install cmake libsuitesparse-dev libboost-filesystem-dev libboost-system-dev

[CMakePresets.json](CMakePresets.json) define setups for building with gcc and clang on linux.

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release

To build with gcc using release options

    cmake ../../ --preset gcc-release
    cmake --build . -j <number-of-concurrent-jobs>

To build a portable statically linked binary

    cmake ../../ --preset gcc-release-portable
    cmake --build . -j <number-of-concurrent-jobs>

#### Installers
To build .deb package

    cpack -G DEB

To build a generic linux installer

    cpack -G STGZ

For the generic linux installer it is necesary to set the environment variable `DAISYHOME` to the install directory. For example, if daisy was installed in `/home/user-name/daisy`, you would do

    export DAISYHOME=/home/user-name/daisy

TODO: Need to figure out static linking vs specifying depends in the package. The deb package and similar should probably be dynamically linked with depends, and the generic installer should probably be statically linked.
    
### MacOS
Install dependencies

    brew install cmake boost suite-sparse 
    
Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release
    
Build it

    cmake ../../ --preset clang-release
    cmake --build . -j <number-of-concurrent-jobs>

Test it

    ctest

### Windows
Install [MSYS2](https://www.msys2.org/).

Install build environment

    pacman -S git mingw-w64-ucrt-x86_64-ccmake mingw-w64-ucrt-x86_64-gcc mingw-w64-x86_64-ninja

Install daisy dependencies
    
    pacman -S mingw-w64-ucrt-x86_64-suitesparse mingw-w64-ucrt-x86_64-boost
    
Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release
    
Build it (Does not work yet. Issues with linking + wreg32 is not compiled yet)

    cmake ../../ --preset mingw-gcc-release
    cmake --build . -j <number-of-concurrent-jobs>
    
Test it

    ctest


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
