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

    apt install g++ cmake libsuitesparse-dev libboost-filesystem-dev libboost-system-dev python3-pybind11

### Build Daisy
[CMakePresets.json](CMakePresets.json) define setups for building with gcc and clang on linux.

Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release

To build with gcc using release options

    cmake ../../ --preset linux-gcc-native
    cmake --build . -j <number-of-concurrent-jobs>-

#### Build for distribution
To build a portable binary for distribution

    cmake ../../ --preset linux-gcc-portable
    cmake --build . -j <number-of-concurrent-jobs>


### Make an installer
To build .deb package

    cpack -G DEB

To build a generic linux installer

    cpack -G STGZ

For the generic linux installer it is necesary to set the environment variable `DAISYHOME` to the install directory. For example, if daisy was installed in `/home/user-name/daisy`, you would do

    export DAISYHOME=/home/user-name/daisy

TODO: Need to figure out static linking vs specifying depends in the package. The deb package and similar should probably be dynamically linked with depends, and the generic installer should probably be statically linked.

## Flatpak
### Dependencies
Install dependencies

* flatpak
* flatpak-builder

Using apt

    # apt install flatpak flatpak-builder
    
### Build
The build is controlled from `flatpak/dk.ku.daisy.yml`.

he build will take some time the first time you do it, because flatpak-builder download and build dependencies. Subsequent builds should be much faster due to caching.

From the root source dir
```
make flatpak
```
This will produce `flatpak/daisy.flatpak` that can be distributed.
    
    
