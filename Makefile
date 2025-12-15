uv_python_root = <path-to-python-root>

macos: macos-python macos-no-python

macos-python:
	mkdir -p build/portable-python
	cd build/portable-python && \
	cmake ../.. --preset macos-clang-portable -DUV_INSTALLED_PYTHON_ROOT_DIR=$(uv_python_root) && \
	cmake --build . -j 6 && \
	cpack

macos-no-python:
	mkdir -p build/portable-no-python
	cd build/portable-no-python &&  \
	cmake ../.. --preset macos-clang-portable -DBUILD_PYTHON=OFF && \
	cmake --build . -j 6 && \
	cpack
