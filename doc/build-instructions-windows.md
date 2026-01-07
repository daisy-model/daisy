# Build Daisy on Windows

The [Makefile](../Makefile) provides convenience targets for building, packing and testing.

## Prerequisites

* [MSYS2](https://www.msys2.org/).

All commands below are executed from an MSYS2 shell.

## Dependencies
Install build environment

    pacman -S git mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-nsis unzip make

Install daisy dependencies

    pacman -S mingw-w64-ucrt-x86_64-suitesparse mingw-w64-ucrt-x86_64-boost mingw-w64-ucrt-x86_64-pybind11 mingw-w64-ucrt-x86_64-uv


Install python with uv

	uv python install 3.13

## Build Daisy
Download the source code

    git clone git@github.com:daisy-model/daisy.git
    cd daisy

### Default build for release
The Makefile defines some convenience targets

    make windows-nsis

Will build daisy in `build/mingw-gcc-portable` and make an installer.


    make windows-zip

Will build daisy in `build/mingw-gcc-portable` and make a zip archive.


    make windows

Will build both installer and zip archive.


    make windows-test

Will build the zip archive and run the test suite.


### Non-standard builds
[CMakePresets.json](CMakePresets.json) define setups for various builds using gcc or clang. For example, to build a native optimized version using clang

    mkdir -p build/mingw-clang-native
    cmake . -B build/mingw-clang-native --preset mingw-clang-native -DUV_INSTALLED_PYTHON_ROOT_DIR=$( scripts/find_python_root_dir.sh 3.13 )
    cmake --build build/mingw-clang-native
    cd build/mingw-clang-native
    cpack -G ZIP

To see a list of presets

    cmake --list-presets
