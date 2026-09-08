# Building Daisy
The [Makefile](../Makefile) contains targets for building Daisy on multiple platforms. There are also targets for building and running tests (including coverage reports), for building documentation, for building the test bundle and for tagging a release.

For example, to build and run test on windows you would do
```
make windows-test
```

## Platform specific instructions

* [Linux](build-instructions-linux.md)
* [MacOS](build-instructions-macos.md)
* [Windows](build-instructions-windows.md)
* [Making a release](making-a-release.md)

Documentation: [build-documentation.md](build-documentation.md)

## Custom builds
If you want to customize you build of Daisy, for example to generate an optimized build you should use the Makefile targets as starting point.

There are also build presets defined in `CMakePresets.json` that can be used to adapt the build. To see a list of available presets run

    cmake --list-presets

in the source dirrectory.

## Python support
By default Daisy is built with python support. Python is bundled on Windows and MacOS. On Linux this requires a python installation. If for some reason you do not want python support, you can build without python support by adding

	-DBUILD_PYTHON=OFF

to the cmake configure command, e.g.

	cmake ../.. --preset macos-clang-portable -DBUILD_PYTHON=OFF
