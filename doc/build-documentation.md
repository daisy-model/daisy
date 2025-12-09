# Building documentation
TODO: Add prerequisites and dependencies

Documentation can be built by running

    cmake ../../ --preset <preset-name -DBUILD_DOC=ON
    cmake --build . --target docs

This will generate `exercises.pdf`, `reference.pdf` and `tutorial.pdf` in the directory `doc` under the build directory.
