# Build Daisy on Windows

The [Makefile](../Makefile) provides convenience targets for building, packing and testing.

## Prerequisites

* [MSYS2](https://www.msys2.org/).

All commands below are executed from an MSYS2 shell.

## Dependencies
Install build environment

    pacman -S git mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja unzip

Install daisy dependencies

    pacman -S mingw-w64-ucrt-x86_64-suitesparse mingw-w64-ucrt-x86_64-boost mingw-w64-ucrt-x86_64-pybind11 mingw-w64-ucrt-x86_64-uv


Install python with uv

	uv python install 3.13

Find the path to the installed python.

	uv python list

Should produce something similar to

```{bash}
... <snip>
cpython-3.13.11-windows-x86_64-none                  C:\Users\<user-name>\AppData\Roaming\uv\python\cpython-3.13.11-windows-x86_64-none\python.exe
... <snip>
```

Record the path to your python 3.13 installation directory. We will refer to this as `<python-dir>` in the following. In this case
```
C:/Users/<user-name>/AppData/Roaming/uv/python/cpython-3.13.11-windows-x86_64-none
```
where `\` have been replaced with `/`.

## Build Daisy

Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/mingw-gcc-portable
    cd daisy/build/mingw-gcc-portable


Configure with cmake. Remember to substitute your `<python-dir>`.

    cmake ../../ --preset mingw-gcc-portable -DUV_INSTALLED_PYTHON_ROOT_DIR=<python-dir>

If everything went well, build with cmake

    cmake --build .


## Make an installer
Install dependencies

    pacman -S mingw-w64-ucrt-x86_64-nsis

Make the installer

	cpack -G NSIS

Make a zip file that does not require installation

    cpack -G ZIP
