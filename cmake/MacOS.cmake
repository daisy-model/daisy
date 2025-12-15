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
)
target_link_directories(${DAISY_BIN_NAME} PRIVATE ${EXTRA_SYSTEM_INCLUDE_DIRECTORIES})

install(TARGETS ${DAISY_BIN_NAME}
  DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}/bin
  COMPONENT runtime
)

# When making an installer we want to be able to redistribute the dylibs and find them.
#
# Copy dylibs so we can redistribute. First copy to a directory in the build tree. CMake will
# handle symlinks. Then install all the files we just copied.
# We put them in bin/lib, then we dont need to update the rpath of the shared library files
# because they look in @loader_path/../lib, which becomes lib/
set(_staging_dir "${CMAKE_CURRENT_BINARY_DIR}/_staging")
set(_boost_path_prefix "")
set(_dylib_target_dir "${_staging_dir}/bin/lib")
file(INSTALL
  "${HOMEBREW_PREFIX}/lib/libcxsparse.4.dylib"
  "${HOMEBREW_PREFIX}/lib/libsuitesparseconfig.7.dylib"
  "${HOMEBREW_PREFIX}/${_boost_path_prefix}lib/libboost_filesystem.dylib"
  "${HOMEBREW_PREFIX}/${_boost_path_prefix}lib/libboost_system.dylib"
  "${HOMEBREW_PREFIX}/${_boost_path_prefix}lib/libboost_atomic.dylib"
  "${HOMEBREW_PREFIX}/opt/libomp/lib/libomp.dylib"
  DESTINATION ${_dylib_target_dir}
  FOLLOW_SYMLINK_CHAIN
)

# Update daisy binary so it knows to look in @executable_path for dylibs
# First update the rpath.
# For the installed binary we use @executable_path directly because it is installed to
#  <prefix/bin
# which contain lib/ with shared libraries.
# For the build binary we use @executable_path/bin because the binary is not moved to the bin dir
set_target_properties(${DAISY_BIN_NAME}
  PROPERTIES
  INSTALL_RPATH "@executable_path"
  BUILD_RPATH "@executable_path/bin"
)

# Then update the id of dylibs
# This is brittle. Would be nice to get the dir path dynamically.
set(_boost_id_prefix "boost/")
set(_dylibs_rel_path
  "suite-sparse/lib/libcxsparse.4.dylib"
  "${_boost_id_prefix}lib/libboost_filesystem.dylib"
  "${_boost_id_prefix}lib/libboost_system.dylib"
  "${_boost_id_prefix}lib/libboost_atomic.dylib"
  "${_boost_id_prefix}lib/libboost_process.dylib"
)
foreach(_dylib_rel_path ${_dylibs_rel_path})
  set(_old_lib_id "${HOMEBREW_PREFIX}/opt/${_dylib_rel_path}")
  cmake_path(GET _old_lib_id FILENAME _dylib)
  set(_new_lib_id "@rpath/lib/${_dylib}")

  message("-- In ${DAISY_BIN_NAME}: Change ${_old_lib_id} -> ${_new_lib_id}")
  add_custom_command(TARGET ${DAISY_BIN_NAME}
    POST_BUILD
    COMMAND "install_name_tool"
    ARGS "-change" "${_old_lib_id}" "${_new_lib_id}"
    "${DAISY_BIN_NAME}"
  )
endforeach()

# We also need to update the path of libomp in libsuitesparseconfig
set(_old_lib_id "${HOMEBREW_PREFIX}/opt/libomp/lib/libomp.dylib")
set(_new_lib_id "@rpath/libomp.dylib")
set(_suitesparseconfig "${_dylib_target_dir}/libsuitesparseconfig.7.dylib")
message("-- In ${_suitesparseconfig}: Change ${_old_lib_id} -> ${_new_lib_id}")
add_custom_command(TARGET ${DAISY_BIN_NAME}
  POST_BUILD
  COMMAND "install_name_tool"
  ARGS "-change" "${_old_lib_id}" "${_new_lib_id}"
  "${_suitesparseconfig}"
)

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

  # Replace the id of the python dylib to avoid leaking info about build
  # Not necesary for running, because we will never link new objects against
  # the dylib.
  set(_python_dylib_relpath "${_staging_dir}/python/lib/${_python_dylib_name}")
  message("-- In ${_python_dylib_relpath}: Change id to ${_python_dylib_name}")
  add_custom_command(TARGET ${DAISY_BIN_NAME}
    POST_BUILD
    COMMAND "install_name_tool"
    ARGS "-id" "${_python_dylib_name}" "${_python_dylib_relpath}"
  )

  # Update the python dylib path in daisy binary
  set(_new_lib_id "@executable_path/../python/lib/${_python_dylib_name}")
  message("-- In ${DAISY_BIN_NAME}: Change ${_old_lib_id} -> ${_new_lib_id}")
  add_custom_command(TARGET ${DAISY_BIN_NAME}
    POST_BUILD
    COMMAND "install_name_tool"
    ARGS "-change" "${_old_lib_id}" "${_new_lib_id}"
    "${DAISY_BIN_NAME}"
  )

  # Make a symlink to pip so it is easy to install new packages
  file(CREATE_LINK "../python/bin/pip" "${_staging_dir}/bin/pip" SYMBOLIC)

  # Make a symlink to python so it is easy to test things in the specific interpreter
  file(CREATE_LINK "../python/bin/python" "${_staging_dir}/bin/python" SYMBOLIC)

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


# Install the staged stuff
install(DIRECTORY ${_staging_dir}/
  DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}
  USE_SOURCE_PERMISSIONS
  COMPONENT runtime
)
