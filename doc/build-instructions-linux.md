# Build Daisy on Linux

## Dependencies
Install dependencies
* cmake
* boost (filesystem and system)
* suitesparse
* g++
* pybind11

Using apt

    apt install g++ cmake libsuitesparse-dev libboost-filesystem-dev libboost-system-dev python3-pybind11

## Build Daisy
[CMakePresets.json](CMakePresets.json) define setups for building with gcc and clang on linux.

Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release

To build with gcc using release options

    cmake ../../ --preset linux-gcc-native
    cmake --build . -j <number-of-concurrent-jobs>-

### Build for distribution
To build a portable binary for distribution

    cmake ../../ --preset linux-gcc-portable
    cmake --build . -j <number-of-concurrent-jobs>


## Make an installer
To build .deb package

    cpack -G DEB

To build a generic linux installer

    cpack -G STGZ

For the generic linux installer it is necesary to set the environment variable `DAISYHOME` to the install directory. For example, if daisy was installed in `/home/user-name/daisy`, you would do

    export DAISYHOME=/home/user-name/daisy

TODO: Need to figure out static linking vs specifying depends in the package. The deb package and similar should probably be dynamically linked with depends, and the generic installer should probably be statically linked.
