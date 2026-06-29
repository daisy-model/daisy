daisy_version := $(shell scripts/get_version_from_cmake.sh)
daisy_tag := v$(daisy_version)
current_dir := $(shell pwd)
has_gcovr := $(shell command -v gcovr 2> /dev/null)
python_version = 3.13
python_root = $(shell scripts/find_python_root_dir.sh ${python_version})
nproc := 6

# Python
.PHONY: uv-python
uv-python:
	uv python install $(python_version)


# Release
.PHONY: release-tag
release-tag:
	@git fetch --no-tags origin main
	@if [ "$$( git rev-parse --abbrev-ref HEAD )" != "main" ]; then \
		echo "Refusing to tag: current branch is not main"; \
		exit 1; \
	fi
	@if [ "$$( git rev-parse HEAD )" != "$$( git rev-parse origin/main )" ]; then \
		echo "Refusing to tag: local main is not at origin/main HEAD"; \
		exit 1; \
	fi
	@if git rev-parse "$(daisy_tag)" >/dev/null 2>&1; then \
		echo "Refusing to tag: $(daisy_tag) already exists"; \
		exit 1; \
	fi
	git tag -a "$(daisy_tag)" -m "Release $(daisy_tag)"
	@echo "Created tag $(daisy_tag)"


# Windows
## Windows: Default build installer and zip
windows_build_dir=build/mingw-gcc-portable
.PHONY: windows
windows: windows-nsis windows-zip

## Windows: Standard build
.PHONY: windows-build
windows-build: uv-python
	mkdir -p ${windows_build_dir}
	cmake .	-B ${windows_build_dir} --preset mingw-gcc-portable -DUV_INSTALLED_PYTHON_ROOT_DIR=${python_root}
	cmake --build ${windows_build_dir}

## Windows: Installer
.PHONY: windows-nsis
windows-nsis: windows-build
	cd ${windows_build_dir} && cpack -G NSIS

## Windows: Zip
.PHONY: windows-zip
windows-zip: windows-build
	cd ${windows_build_dir} && cpack -G ZIP

## Windows: Test
.PHONY: windows-test
windows-test: windows-zip
	cd ${windows_build_dir} && \
	uv venv --allow-existing && \
	uv pip install git+https://github.com/daisy-model/daisypy-test && \
	unzip -qo `ls | grep -e "daisy-.*-Windows-python.*zip"` && \
	ctest --output-on-failure

# Linux
## Linux: Default build both debian and flatpak
linux_build_dir=build/linux-gcc-portable
.PHONY: linux
linux: debian flatpak

## Linux: Standard build
.PHONY: linux-build
linux-build:
	mkdir -p ${linux_build_dir}
	cmake . -B ${linux_build_dir} --preset linux-gcc-portable
	cmake --build ${linux_build_dir} -j ${nproc}

## Linux: Coverage build
linux_coverage_build_dir=build/linux-gcc-coverage
.PHONY: linux-coverage-build
linux-coverage-build:
	mkdir -p ${linux_coverage_build_dir}
	cmake . -B ${linux_coverage_build_dir} --preset linux-gcc-coverage
	cmake --build ${linux_coverage_build_dir} -j ${nproc}

## Linux: Debian package
.PHONY: debian
debian: linux-build
	cd ${linux_build_dir} && cpack -G DEB

## Linux: Flatpak package
.PHONY: flatpak
flatpak:
	cd flatpak && \
	flatpak-builder --force-clean --install-deps-from=flathub --repo=repo build dk.ku.daisy.yml && \
	flatpak build-bundle repo daisy-$(daisy_version).flatpak dk.ku.daisy


## Linux test using standard build
$(linux_build_dir)/daisy: linux-build

.PHONY: linux-test
linux-test: $(linux_build_dir)/daisy
	cd $(linux_build_dir) && \
	uv venv --allow-existing && \
	uv pip install git+https://github.com/daisy-model/daisypy-test && \
	ctest --output-on-failure

## Linux test using coverage build
$(linux_coverage_build_dir)/daisy: linux-coverage-build

.PHONY: linux-coverage
linux-coverage: $(linux_coverage_build_dir)/daisy
# The test suite will most likely fail some cases so we ignore the output
	- cd $(linux_coverage_build_dir) && \
	uv venv --allow-existing && \
	uv pip install git+https://github.com/daisy-model/daisypy-test && \
	ctest -j 20
ifndef has_gcovr
	@echo "\ngcovr is not available, no coverage report generated\nInstall gcovr with\n  pip install gcovr"
else
	cd $(linux_coverage_build_dir) && gcovr -r ../../ . --html ../../test/coverage.html
	@echo Coverage report: file://$(current_dir)/test/coverage.html
endif


## Linux: Documentation
.PHONY: linux-doc
linux-doc:
	cmake -B $(linux_build_dir) --preset linux-gcc-portable -DBUILD_DOC=ON
	cmake --build $(linux_build_dir) --target docs


# MacOS
## MacOS: Standard build with python support
macos_build_dir=build/macos-clang-portable
.PHONY: macos
macos: uv-python
	mkdir -p ${macos_build_dir} && \
	cd ${macos_build_dir} && \
	rm -f daisy-bin && \
	rm -rf _staging && \
	cmake ../../ --preset macos-clang-portable -DUV_INSTALLED_PYTHON_ROOT_DIR=${python_root} && \
	cmake --build .  -j ${nproc} && \
	otool -L daisy-bin && \
	otool -L _staging/bin/lib/libsuitesparseconfig.7.dylib && \
	cpack

## MacOS: Build without python support
macos_no_python_build_dir=build/macos-clang-portable-no-python
.PHONY: macos-no-python
macos-no-python:
	mkdir -p ${macos_no_python_build_dir} && \
	cd ${macos_no_python_build_dir} && \
	rm -f daisy-bin && \
	rm -rf _staging && \
	cmake ../.. --preset macos-clang-portable -DBUILD_PYTHON=OFF && \
	cmake --build . -j ${nproc} && \
	otool -L daisy-bin && \
	otool -L _staging/bin/lib/libsuitesparseconfig.7.dylib && \
	cpack

## MacOS test. We only use version with python
.PHONY: macos-test
macos-test: macos
	cd ${macos_build_dir} && \
	unzip -qo `ls | grep -e "daisy.*Darwin-python.*zip"` && \
	uv venv --allow-existing && \
	uv pip install git+https://github.com/daisy-model/daisypy-test && \
	ctest --output-on-failure
