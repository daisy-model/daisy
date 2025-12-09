# Code coverage
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
