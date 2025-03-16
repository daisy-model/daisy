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
* g++
* pybind11

Using apt

    apt install g++ cmake libsuitesparse-dev libboost-filesystem-dev libboost-system-dev python3-pybind11

[CMakePresets.json](CMakePresets.json) define setups for building with gcc and clang on linux.

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release

To build with gcc using release options

    cmake ../../ --preset linux-gcc-native
    cmake --build . -j <number-of-concurrent-jobs>

#### Build for distribution
To build a portable binary for distribution

    cmake ../../ --preset linux-gcc-portable
    cmake --build . -j <number-of-concurrent-jobs>


To build .deb package

    cpack -G DEB

To build a generic linux installer

    cpack -G STGZ

For the generic linux installer it is necesary to set the environment variable `DAISYHOME` to the install directory. For example, if daisy was installed in `/home/user-name/daisy`, you would do

    export DAISYHOME=/home/user-name/daisy

TODO: Need to figure out static linking vs specifying depends in the package. The deb package and similar should probably be dynamically linked with depends, and the generic installer should probably be statically linked.

### MacOS
#### Note on python suppport
At the moment we rely on users manually installing python from python.org. The current version is python 3.13.2, which should be downloaded from https://www.python.org/downloads/release/python-3132/ and installed before building. Other dependencies are installed with brew. The reason we do not use brew to install python, is because it would then require that users also used brew to install python and we prefer the on-click installer from python.org.


Install dependencies

    brew install cmake boost suite-sparse pybind11

Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release

Build it

    cmake ../../ --preset macos-clang-native
    cmake --build . -j <number-of-concurrent-jobs>

#### Build for distribution
Build a portable binary

    cmake ../../ --preset macos-clang-portable
    cmake --build . -j <number-of-concurrent-jobs>

Make an installer

    cpack

### Windows
Install [MSYS2](https://www.msys2.org/).

Install build environment

    pacman -S git mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja

Install daisy dependencies
    
    pacman -S mingw-w64-ucrt-x86_64-suitesparse mingw-w64-ucrt-x86_64-boost mingw-w64-ucrt-x86_64-pybind11 mingw-w64-ucrt-x86_64-python
	
   
Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/release
    cd daisy/build/release


Download a python distribution from https://www.python.org/downloads/windows/ and unpack to `daisy/python/<python-release-name>`. Then update `Python_ROOT_DIR` in `cmake/python.cmake` to point to the directory you just unpacked.
    
Build it

    cmake ../../ --preset mingw-gcc-native
    cmake --build .

At time of writing, it was not possible to use the latest python (3.13.2) because the development module was not found. Version 3.12.9 works fine.

For some reason Daisy ends up looking for `libpython3.12.dll`, but the embeddable python distribution contains `python312.dll`. A workaround for now is to make a copy of `python312.dll` called `libpython3.12.dll` and place it in the `daisy/python/<python-release-name>` directory. We cannot just rename, because "Find_Python" looks for "python312.dll`. 


#### Build for distribution
Build a portable binary

    cmake ../../ --preset mingw-gcc-portable
    cmake --build .

Install dependencies

    pacman -S mingw-w64-ucrt-x86_64-nsis

Make an installer

	cpack

## Testing
### Install test dependencies
We use a set of python tools to check Daisy output against a baseline. These are available from [daispy-test](https://github.com/daisy-model/daisypy-test) and can be installed with

    pip install daisypy-test@git+https://github.com/daisy-model/daisypy-test

Test scenarios are in [test/dai_test_files](test/dai_test_files).
When adding a test you need to add it as a `dai_test_case` in [test/CMakeLists.txt](test/CMakeLists.txt).

Use [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) to run tests.

    ctest

#### Windows
If you use `MSYS2` you need to install `python`, `pip` and `pandas`. Installing `pandas` with `pip` fails, so it needs to be installed before `daisypy-test`

    pacman -S mingw-w64-ucrt-x86_64-python
    pacman -S mingw-w64-ucrt-x86_64-python-pip
    pacman -S mingw-w64-ucrt-x86_64-python-pandas

Then you need to setup a virtual environment.

    python -m venv ~/.venvs/daisy --system-site-packages

and activate it

    source ~/.venvs/daisy/bin/activate

Then you can use `pip` to install `daispy-test`


## Code coverage
To get code coverage you need to set the build type to `Coverage`. This will disable some optimizations.
There is a coverage preset defined for gcc.

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/coverage 
    cd daisy/build/coverage
    cmake ../.. --preset linux-gcc-coverage
    make -j 8
    ctest

Use https://gcovr.com/en/stable/ to summarize coverage

    pip install gcovr
    cd <coverage-build-dir>
    gcovr -r ../../ . --html ../../test/coverage.html

## Documentation
Documentation can be built by running

    cmake ../../ --preset <preset-name -DBUILD_DOC=ON
    cmake --build . --target docs

This will generate `exercises.pdf`, `reference.pdf` and `tutorial.pdf` in the directory `doc` under the build directory.
