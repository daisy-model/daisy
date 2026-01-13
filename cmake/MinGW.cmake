set(DAISY_SAMPLE_DESTINATION "sample")
set(DAISY_LIB_DESTINATION "lib")
set(DAISY_CORE_NAME core)

set(_staging_dir "${CMAKE_CURRENT_BINARY_DIR}/_staging")

target_include_directories(${DAISY_BIN_NAME} PUBLIC include)

# On Windows we build everything except main as a shared library (core)
add_library(${DAISY_CORE_NAME} SHARED)
target_include_directories(${DAISY_CORE_NAME} PUBLIC include)
target_link_libraries(${DAISY_CORE_NAME} PUBLIC
  cxsparse
  Boost::filesystem
  Boost::process
)
target_link_options(${DAISY_CORE_NAME} PRIVATE ${LINKER_OPTIONS})

add_library(windows SHARED)
target_include_directories(windows PUBLIC include)
target_link_options(windows PRIVATE ${LINKER_OPTIONS})
add_subdirectory(src/windows)
target_link_libraries(${DAISY_CORE_NAME} PUBLIC
  windows
  ${WIN_LIBS}
)
target_link_libraries(${DAISY_BIN_NAME} PUBLIC ${DAISY_CORE_NAME})

# On Windows we need to install the dlls that we build and we need to
# install the dlls we depend on.
install(TARGETS ${DAISY_BIN_NAME} RUNTIME DESTINATION bin COMPONENT runtime)
install(TARGETS ${DAISY_CORE_NAME} RUNTIME DESTINATION bin COMPONENT runtime)
install(TARGETS windows RUNTIME DESTINATION bin COMPONENT runtime)
install(FILES
  $ENV{MINGW_PREFIX}/bin/libstdc++-6.dll
  $ENV{MINGW_PREFIX}/bin/libwinpthread-1.dll
  $ENV{MINGW_PREFIX}/bin/libgcc_s_seh-1.dll
  $ENV{MINGW_PREFIX}/bin/libgomp-1.dll
  $ENV{MINGW_PREFIX}/bin/libcxsparse.dll
  $ENV{MINGW_PREFIX}/bin/libsuitesparseconfig.dll
  $ENV{MINGW_PREFIX}/bin/libboost_filesystem-mt.dll
  TYPE BIN
  COMPONENT runtime
)

# We add python version to distribution name, so people can see the version they get
set(DAISY_PYTHON_VERSION "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")

# Copy python installation to build tree
set(_python_dir "${_staging_dir}/python")
file(COPY ${UV_INSTALLED_PYTHON_ROOT_DIR}/
  DESTINATION ${_python_dir}
  PATTERN "EXTERNALLY-MANAGED" EXCLUDE  # The environment is no longer uv maintained
  PATTERN "include" EXCLUDE             # We dont need header files
  PATTERN "Scripts" EXCLUDE
  PATTERN "tcl" EXCLUDE
  PATTERN "LICENSE" EXCLUDE
  PATTERN "BUILD" EXCLUDE
  PATTERN "pythonw.exe" EXCLUDE
)
# and install it
install(DIRECTORY ${_python_dir}/ TYPE BIN COMPONENT runtime)
