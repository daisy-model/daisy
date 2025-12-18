# Build Daisy on Linux
Daisy can either be built with [CMake](#cmake) or [Flatpak](#flatpak).

## CMake
### Dependencies
Install dependencies
* cmake
* boost (filesystem)
* suitesparse
* g++
* pybind11

Using apt

    apt install g++ cmake libsuitesparse-dev libboost-filesystem-dev python3-pybind11

#### Flatpak dependencies
Install dependencies

* flatpak
* flatpak-builder

Using apt

    apt install flatpak flatpak-builder


### Build Daisy
Download the source code and change to to repository root directory

    git clone git@github.com:daisy-model/daisy.git
    cd daisy

#### Default build for release
The Makefile defines some convenience targets

    make debian

Will build daisy in `build/linux-portable` and make a `deb` package.


    make flatpak

Will build daisy in `./flatpak`  and make a `flatpak` package. Note that this will take some time on first run because dependencies have to be downloaded and built. The flatpak build is controlled from `flatpak/dk.ku.daisy.yml`.


    make linux

Will build both `deb` and `flatpak` packages.


    make linux-test

Will run the test suite.


    make linux-coverage

Will make a build for coverage analysis, run the test suite and, if `gcovr` is installed, create a coverage report in html format.

#### Non-standard builds
[CMakePresets.json](CMakePresets.json) define setups for various builds using gcc or clang. For example, to build a native optimized version using clang

    mkdir -p build/linux-clang-native
    cd build/linux-clang-native
    cmake ../../ --preset linux-clang-native
    cmake --build . -j <number-of-concurrent-jobs>-

To see a list of presets

    cmake --list-presets
