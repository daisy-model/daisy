# Building Daisy
There are presets defined in `CMakePresets.json` for building on different platforms.

To see a list of available presets run

    cmake --list-presets

in the source dirrectory.

## Python support
By default Daisy is built with python support. On Windows python is bundled. On MacOS and Linux this requires a python installation. Not all users want this, so we build both with and without python support. To build without pythn support add

	-DBUILD_PYTHON=OFF

to the cmake configure command, e.g.

	cmake ../.. --preset macos-clang-portable -DBUILD_PYTHON=OFF


## Platform specific instructions

* [Linux](build-instructions-linux.md)
* [MacOS](build-instructions-macos.md)
* [Windows](build-instructions-windows.md)

Documentation: [build-documentation.md](build-documentation.md)
