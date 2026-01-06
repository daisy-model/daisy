# Build Daisy on MacOS
The guide assumes that Daisy is built with Python support. If this is not the case, then ignore all the python related things.

## Prerequisites

* [brew](https://brew.sh)
* [uv](https://docs.astral.sh/uv/getting-started/installation/)

## Dependencies

Install dependencies with brew

    brew install cmake boost suite-sparse pybind11

Install python with uv

	uv python install 3.13

Find the path to the installed python.

	uv python list

Should produce something similar to

```{bash}
cpython-3.15.0a2-macos-x86_64-none                 <download available>
... <snip>
cpython-3.13.11-macos-x86_64-none                  /Users/<user-name>/.local/share/uv/python/cpython-3.13.11-macos-x86_64-none/bin/python3.13
... <snip>
```

Record the path to your python 3.13 install. We will refer to this as `<python-dir>` in the following


## Build Daisy

Download the source code and setup a build dir

    git clone git@github.com:daisy-model/daisy.git
    mkdir -p daisy/build/portable
	cd daisy/build/portable

Configure with cmake. Remember to substitute your `<python-dir>`

	cmake ../.. --preset macos-clang-portable -DUV_INSTALLED_PYTHON_ROOT_DIR=<python-dir>

If everything went well, build with cmake

    cmake --build . -j <number-of-concurrent-jobs>

Check that dynamic library paths have been updated to relative paths

	otool -L ./daisy

Should yield something similar to

```{bash}
daisy:
	@rpath/lib/libcxsparse.4.dylib (compatibility version 4.0.0, current version 4.4.1)
	@rpath/lib/libboost_filesystem.dylib (compatibility version 0.0.0, current version 0.0.0)
	@rpath/lib/python/lib/libpython3.13.dylib (compatibility version 3.13.0, current version 3.13.0)x
	@rpath/lib/libboost_atomic.dylib (compatibility version 0.0.0, current version 0.0.0)
	@rpath/lib/libboost_system.dylib (compatibility version 0.0.0, current version 0.0.0)
	/usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 1900.180.0)
	/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1351.0.0)
```

The cxsparse, boost and python libraries should start with `@rpath`. If not then you need to update cmake/MacOS.cmake and rerun both cmake steps.


## Make an installer

    cpack
