uv_python_root = $(shell scripts/find_python_root_dir.sh)
script_dir = $(shell pwd)/scripts

# MacOS
MACOS_BUILD_DIR=build/macos-portable
MACOS_NO_PYTHON_BUILD_DIR=build/macos-portable-no-python
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

macos-no-python:
	mkdir -p ${MACOS_NO_BUILD_DIR} && \
	cd ${MACOS_NO_PYTHON_BUILD_DIR} && \
	rm -f daisy-bin && \
	rm -r _staging && \
	cmake ../.. --preset macos-clang-portable -DBUILD_PYTHON=OFF && \
	cmake --build . -j 6 && \
	otool -L daisy-bin && \
	otool -L _staging/bin/lib/libsuitesparseconfig.7.dylib && \
	cpack

macos-test:
	cd ${MACOS_BUILD_DIR} && \
	unzip -qq `ls | grep -e "daisy.*Darwin-python.*zip"` && \
	ctest --output-on-failure
