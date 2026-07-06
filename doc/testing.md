# Testing
## Install test dependencies
We use a set of python tools to check Daisy output against a baseline. These are available from [daispy-test](https://github.com/daisy-model/daisypy-test) and can be installed with

    pip install daisypy-test@git+https://github.com/daisy-model/daisypy-test

## Recommended local entrypoints
For local development, the recommended way to run the test suites is via the
platform-specific Makefile targets:

    make linux-test
    make macos-test
    make windows-test

These targets build Daisy, create or reuse the local `uv` environment in the
build directory, install `daisypy-test`, and then run `ctest`.

In GitHub Actions, we keep the build, test-environment setup, and test steps
explicit rather than routing CI through the Makefile targets.

Test scenarios are in [test/dai_test_files](test/dai_test_files).
When adding a test you need to add it as a `dai_test_case` in [test/CMakeLists.txt](test/CMakeLists.txt).

Use [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html) directly if
you need lower-level control from a configured build directory. This assumes the
build directory already has a `uv` environment and `daisypy-test` installed.

    ctest

If `ctest` fails because `check_daisy` or `test_daisy` cannot be found, either
use the Makefile target for your platform or install the test harness in the
configured build directory first:

    uv venv --allow-existing
    uv pip install git+https://github.com/daisy-model/daisypy-test

### Windows
If you use `MSYS2` you need to install `python`, `pip` and `pandas`. Installing `pandas` with `pip` fails, so it needs to be installed before `daisypy-test`

    pacman -S mingw-w64-ucrt-x86_64-python
    pacman -S mingw-w64-ucrt-x86_64-python-pip
    pacman -S mingw-w64-ucrt-x86_64-python-pandas

Then you need to setup a virtual environment.

    python -m venv ~/.venvs/daisy --system-site-packages

and activate it

    source ~/.venvs/daisy/bin/activate

Then you can use `pip` to install `daispy-test`.

NOTE: This will most likely fail if you have already built daisy and try to setup the test environment from the build directory. The reason is that python binaries are copied to the build direcotry. The solution is to run the commands from another directory or specify the full path to the msys installed python binary.
