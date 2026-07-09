# Building documentation
TODO: Add prerequisites and dependencies

Documentation can be built by running

    cmake ../../ --preset <preset-name> -DBUILD_DOC=ON
    cmake --build . --target docs

This will generate `exercises.pdf`, `reference.pdf` and `tutorial.pdf` in the directory `doc` under the build directory.

Structured model-reference scaffolding lives under `doc/model-reference/`.
These Markdown files are intended to be the source tree for future HTML model
documentation. They are kept in a machine-readable shape with YAML front matter
so they can be rendered later without rewriting the content.

Generated registration-derived scaffolding can be produced with:

    cmake --build . --target model_reference_scaffolding

This writes Markdown scaffolding to `model-reference-generated` in the build
directory using the `DeclareComponent` / `DeclareModel` / `load_frame`
definitions as the primary source.
