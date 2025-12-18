uv_python_root := $(shell scripts/find_python_root_dir.sh)
version := $(shell scripts/get_version_from_cmake.sh)
current_dir := $(shell pwd)
has_gcovr := $(shell command -v gcovr 2> /dev/null)

# Linux
## Linux: Default build both debian and flatpak
LINUX_BUILD_DIR=build/linux-gcc-portable
.PHONY: linux
linux: debian flatpak

## Linux: Standard build
.PHONY: linux-build
linux-build:
	mkdir -p ${LINUX_BUILD_DIR}
	cmake . -B ${LINUX_BUILD_DIR} --preset linux-gcc-portable
	cmake --build ${LINUX_BUILD_DIR} -j 20

## Linux: Coverage build
LINUX_COVERAGE_BUILD_DIR=build/linux-gcc-coverage
.PHONY: linux-coverage-build
linux-coverage-build:
	mkdir -p ${LINUX_COVERAGE_BUILD_DIR}
	cmake . -B ${LINUX_COVERAGE_BUILD_DIR} --preset linux-gcc-coverage
	cmake --build ${LINUX_COVERAGE_BUILD_DIR} -j 20

## Linux: Debian package
.PHONY: debian
debian: linux-build
	cd ${LINUX_BUILD_DIR} && \
	cpack -G DEB

## Linux: Flatpak package
.PHONY: flatpak
flatpak:
	cd flatpak && \
	flatpak-builder --force-clean --install-deps-from=flathub --repo=repo build dk.ku.daisy.yml && \
	flatpak build-bundle repo daisy-$(version).flatpak dk.ku.daisy


## Linux test using standard build
.PHONY: $(LINUX_BUILD_DIR)/daisy
$(LINUX_BUILD_DIR)/daisy: linux-build

linux-test: $(LINUX_BUILD_DIR)/daisy
	cd $(LINUX_BUILD_DIR) && \
	uv venv --allow-existing && \
	uv pip install git+https://github.com/daisy-model/daisypy-test && \
	ctest -j 20

## Linux test using coverage build
.PHONY: $(LINUX_COVERAGE_BUILD_DIR)/daisy
$(LINUX_COVERAGE_BUILD_DIR)/daisy: linux-coverage-build

linux-coverage: $(LINUX_COVERAGE_BUILD_DIR)/daisy
# The test suite will most likely fail some cases so we ignore the output
	- cd $(LINUX_COVERAGE_BUILD_DIR) && \
	uv venv --allow-existing && \
	uv pip install git+https://github.com/daisy-model/daisypy-test && \
	ctest -j 20
ifndef has_gcovr
	@echo "\ngcovr is not available, no coverage report generated\nInstall gcovr with\n  pip install gcovr"
else
	cd $(LINUX_COVERAGE_BUILD_DIR) && gcovr -r ../../ . --html ../../test/coverage.html
	@echo Coverage report: file://$(current_dir)/test/coverage.html
endif



# MacOS
## MacOS: Standard build with python support
MACOS_BUILD_DIR=build/macos-portable
macos:
	mkdir -p ${MACOS_BUILD_DIR} && \
	cd ${MACOS_BUILD_DIR} && \
	rm -f daisy-bin && \
	rm -rf _staging && \
	cmake ../../ --preset macos-clang-portable -DUV_INSTALLED_PYTHON_ROOT_DIR=$(uv_python_root) && \
	cmake --build .  -j 6 && \
	otool -L daisy-bin && \
	otool -L _staging/bin/lib/libsuitesparseconfig.7.dylib && \
	cpack

## MacOS: Build without python support
MACOS_NO_PYTHON_BUILD_DIR=build/macos-portable-no-python
macos-no-python:
	mkdir -p ${MACOS_NO_PYTHON_BUILD_DIR} && \
	cd ${MACOS_NO_PYTHON_BUILD_DIR} && \
	rm -f daisy-bin && \
	rm -r _staging && \
	cmake ../.. --preset macos-clang-portable -DBUILD_PYTHON=OFF && \
	cmake --build . -j 6 && \
	otool -L daisy-bin && \
	otool -L _staging/bin/lib/libsuitesparseconfig.7.dylib && \
	cpack

## MacOS test. We only use version with python
macos-test:
	cd ${MACOS_BUILD_DIR} && \
	unzip -qq `ls | grep -e "daisy.*Darwin-python.*zip"` && \
	uv venv --allow-existing && \
	uv pip install git+https://github.com/daisy-model/daisypy-test && \
	ctest --output-on-failure
