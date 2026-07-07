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
  "${HOMEBREW_PREFIX}/${_boost_path_prefix}lib/libboost_atomic.dylib"
  "${HOMEBREW_PREFIX}/${_boost_path_prefix}lib/libboost_process.dylib"
  "${HOMEBREW_PREFIX}/${_boost_path_prefix}lib/libboost_context.dylib"
  "${HOMEBREW_PREFIX}/${_boost_path_prefix}lib/libboost_date_time.dylib"
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
  "${_boost_id_prefix}lib/libboost_atomic.dylib"
  "${_boost_id_prefix}lib/libboost_process.dylib"
  "${_boost_id_prefix}lib/libboost_context.dylib"
  "${_boost_id_prefix}lib/libboost_date_time.dylib"
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

# Update copied dylib-to-dylib references so the staged libraries can find
# each other from Daisy/bin/lib without relying on rpaths from the main binary.
file(GLOB _staged_cxsparse_files "${_dylib_target_dir}/libcxsparse*.dylib")
foreach(_staged_cxsparse ${_staged_cxsparse_files})
  set(_new_lib_id "@loader_path/libsuitesparseconfig.7.dylib")
  message("-- In ${_staged_cxsparse}: Change libsuitesparseconfig dependency -> ${_new_lib_id}")
  add_custom_command(TARGET ${DAISY_BIN_NAME}
    POST_BUILD
    COMMAND "codesign"
    ARGS "--remove-signature" "${_staged_cxsparse}"
    COMMAND "install_name_tool"
    ARGS "-change" "@rpath/libsuitesparseconfig.7.dylib" "${_new_lib_id}"
    "${_staged_cxsparse}"
    COMMAND "install_name_tool"
    ARGS "-change" "${HOMEBREW_PREFIX}/lib/libsuitesparseconfig.7.dylib" "${_new_lib_id}"
    "${_staged_cxsparse}"
    COMMAND "codesign"
    ARGS "--force" "--sign" "-" "${_staged_cxsparse}"
  )
endforeach()

# We also need to update the path of libomp in libsuitesparseconfig.
file(GLOB _staged_suitesparseconfig_files "${_dylib_target_dir}/libsuitesparseconfig*.dylib")
foreach(_suitesparseconfig ${_staged_suitesparseconfig_files})
  set(_new_lib_id "@loader_path/libomp.dylib")
  message("-- In ${_suitesparseconfig}: Change libomp dependency -> ${_new_lib_id}")
  add_custom_command(TARGET ${DAISY_BIN_NAME}
    POST_BUILD
    COMMAND "codesign"
    ARGS "--remove-signature" "${_suitesparseconfig}"
    COMMAND "install_name_tool"
    ARGS "-change" "@rpath/libomp.dylib" "${_new_lib_id}"
    "${_suitesparseconfig}"
    COMMAND "install_name_tool"
    ARGS "-change" "${HOMEBREW_PREFIX}/opt/libomp/lib/libomp.dylib" "${_new_lib_id}"
    "${_suitesparseconfig}"
    COMMAND "codesign"
    ARGS "--force" "--sign" "-" "${_suitesparseconfig}"
  )
endforeach()

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

  # We keep the copied python dylib as-is. Rewriting its install name is not
  # necessary for running Daisy, and some managed Python builds cannot be
  # processed safely by install_name_tool after copying.
  set(_python_dylib_relpath "${_staging_dir}/python/lib/${_python_dylib_name}")

  # Update the python dylib path in daisy binary
  set(_new_lib_id "@executable_path/../python/lib/${_python_dylib_name}")
  message("-- In ${DAISY_BIN_NAME}: Change ${_old_lib_id} -> ${_new_lib_id}")
  add_custom_command(TARGET ${DAISY_BIN_NAME}
    POST_BUILD
    COMMAND "install_name_tool"
    ARGS "-change" "${_old_lib_id}" "${_new_lib_id}"
    "${DAISY_BIN_NAME}"
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


# Install the staged stuff
install(DIRECTORY ${_staging_dir}/
  DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}
  USE_SOURCE_PERMISSIONS
  COMPONENT runtime
)
