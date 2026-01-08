# Build Daisy on MacOS
## Prerequisites

* [brew](https://brew.sh)
* [uv](https://docs.astral.sh/uv/getting-started/installation/)

## Dependencies

Install dependencies with brew

    brew install cmake boost suite-sparse pybind11

Install python with uv

	uv python install 3.13

## Build Daisy
Download the source code and change to to repository root directory

    git clone git@github.com:daisy-model/daisy.git
    cd daisy

### Default build for release
The Makefile defines some convenience targets

    make macos

Will build daisy in `build/macos-clang-portable` and make a zip archive.


    make macos-no-python

Will build a version of daisy without python support in `build/macos-clang-portable-no-python` and make a zip archive.

### Non-standard builds
[CMakePresets.json](CMakePresets.json) define setups for various builds using gcc or clang. For example, to build a native optimized version using gcc

    mkdir -p build/macos-gcc-native
    cd build/macos-gcc-native
    cmake ../../ --preset macos-gcc-native
    cmake --build . -j `nproc`
    cpack -G ZIP

To see a list of presets

    cmake --list-presets
