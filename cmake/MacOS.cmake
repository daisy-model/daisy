set(DAISY_PACKAGE_INSTALL_DIRECTORY Daisy)
set(DAISY_SAMPLE_DESTINATION "${DAISY_PACKAGE_INSTALL_DIRECTORY}/sample")
set(DAISY_LIB_DESTINATION "${DAISY_PACKAGE_INSTALL_DIRECTORY}/lib")
set(DAISY_CORE_NAME ${DAISY_BIN_NAME})

target_include_directories(${DAISY_BIN_NAME} PUBLIC include)
target_compile_options(${DAISY_BIN_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_options(${DAISY_BIN_NAME} PRIVATE ${LINKER_OPTIONS})
target_link_libraries(${DAISY_BIN_NAME} PUBLIC
  cxsparse
  Boost::filesystem
  Boost::process
)
target_link_directories(${DAISY_BIN_NAME} PRIVATE ${EXTRA_SYSTEM_INCLUDE_DIRECTORIES})

install(TARGETS ${DAISY_BIN_NAME}
  DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}/bin
  COMPONENT runtime
)

# Stage non-target runtime files that will be installed alongside the packaged
# executable. Runtime library fixups are handled by a post-build CMake script.
set(_staging_dir "${CMAKE_CURRENT_BINARY_DIR}/_staging")
set(_dylib_target_dir "${_staging_dir}/bin/lib")

# The packaged executable lives in Daisy/bin and its bundled libraries live in
# Daisy/bin/lib. In the build tree, the post-build fixup script stages those
# libraries under _staging/bin/lib relative to daisy-bin.
set_target_properties(${DAISY_BIN_NAME}
  PROPERTIES
  INSTALL_RPATH "@executable_path"
  BUILD_RPATH "@executable_path/_staging/bin"
)

set(_python_dir "")
set(_python_dylib_name "")
if (${BUILD_PYTHON})
  # We add python version to distribution name, so people can see the version they get
  set(DAISY_PYTHON_VERSION "${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}")

  # Get the old libpython id and filename
  set(_old_lib_id "${Python_LIBRARIES}")
  cmake_path(GET _old_lib_id FILENAME _python_dylib_name)

  # Copy python installation to build tree
  set(_python_dir "${_staging_dir}/python")
  file(COPY ${UV_INSTALLED_PYTHON_ROOT_DIR}/
    DESTINATION ${_python_dir}
    PATTERN "EXTERNALLY-MANAGED" EXCLUDE  # The environment is no longer uv maintained
    PATTERN "include" EXCLUDE             # We dont need header files
  )

  # Replace the id of the python dylib to avoid leaking info about build.
  set(_python_dylib_relpath "${_staging_dir}/python/lib/${_python_dylib_name}")
  add_custom_command(TARGET ${DAISY_BIN_NAME}
    POST_BUILD
    COMMAND "install_name_tool"
    ARGS "-id" "${_python_dylib_name}" "${_python_dylib_relpath}"
  )

  # Install the wrapper script that calls daisy with python
  install(PROGRAMS # Ensure executable permission
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_daisy_macos.sh
    DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}/bin
    RENAME daisy
    COMPONENT runtime
  )
else()
  # Install the wrapper script that calls daisy without python
  install(PROGRAMS # Ensure executable permission
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_daisy_macos_no_python.sh
    DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}/bin
    RENAME daisy
    COMPONENT runtime
  )
endif()

add_custom_command(TARGET ${DAISY_BIN_NAME}
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}"
  ARGS
    -DDAISY_EXECUTABLE=$<TARGET_FILE:${DAISY_BIN_NAME}>
    -DDAISY_STAGING_DIR=${_staging_dir}
    -DDAISY_DYLIB_TARGET_DIR=${_dylib_target_dir}
    -DDAISY_PYTHON_ROOT_DIR=${_python_dir}
    -DDAISY_PYTHON_DYLIB_NAME=${_python_dylib_name}
    -DHOMEBREW_PREFIX=${HOMEBREW_PREFIX}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FixupMacOSDependencies.cmake
)

# Install the staged stuff
install(DIRECTORY ${_staging_dir}/
  DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}
  USE_SOURCE_PERMISSIONS
  COMPONENT runtime
)
