# Build Daisy on Windows

## Prerequisites

* [MSYS2](https://www.msys2.org/).

## Dependencies
Install build environment

    pacman -S git mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja unzip

Install daisy dependencies

    pacman -S mingw-w64-ucrt-x86_64-suitesparse mingw-w64-ucrt-x86_64-boost mingw-w64-ucrt-x86_64-pybind11 mingw-w64-ucrt-x86_64-python


Download a python distribution from https://www.python.org/downloads/windows/ and unpack to `daisy/python/python`. For example,

```{bash}
wget https://www.python.org/ftp/python/3.12.10/python-3.12.10-embed-amd64.zip -O python/python.zip
unzip python/python.zip -d python/python
```

At time of writing, it was not possible to use the latest python (3.13.2) because the development module was not found. Version 3.12.10 works fine.

For some reason Daisy ends up looking for `libpython3.12.dll`, but the embeddable python distribution contains `python312.dll`. A workaround for now is to make a copy of `python312.dll` called `libpython3.12.dll` and place it in the `daisy/python/python` directory. We cannot just rename, because "Find_Python" looks for "python312.dll`.

## Build Daisy

Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/portable
    cd daisy/build/portable


Convifure with cmake

    cmake ../../ --preset mingw-gcc-portable


## Make an installer
Install dependencies

    pacman -S mingw-w64-ucrt-x86_64-nsis

Make the installer

	cpack

Make a zip file that does not require installation

    cpack -G ZIP
´
