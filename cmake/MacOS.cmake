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

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/Daisy.tmbundle
  DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}/TextMate
  COMPONENT TextMateSupport
)

# When making an installer we want to be able to redistribute the dylibs and find them.
#
# Copy dylibs so we can redistribute. First copy to a directory in the build tree. CMake will
# handle symlinks. Then install all the files we just copied.
# We put them in bin/lib, then we dont need to update the rpath of the shared library files
# because they look in @loader_path/../lib, which becomes lib/
set(_boost_path_prefix "")
set(_dylib_target_dir "${CMAKE_CURRENT_BINARY_DIR}/bin/lib")
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
install(DIRECTORY ${_dylib_target_dir}
  DESTINATION ${DAISY_PACKAGE_INSTALL_DIRECTORY}/bin
  COMPONENT runtime
)

# Update daisy binary so it knows to look in @executable_path for dylibs
# First update the rpath.
# For the installed binary we use @executable_path directly because it is installed to
#  <prefix/bin
# which contain lib/ with shared libraries.
# For the build binary we use @executable_path/bin because the binary is not moved to the bin dir
set_target_properties(daisy
  PROPERTIES
  INSTALL_RPATH "@executable_path"
  BUILD_RPATH "@executable_path/bin"
)

# Then update the id of dylibs
# This is brittle. Would be nice to get the dir path dynamically.
set(_boost_id_prefix "boost@1.85/")
set(_dylibs_rel_path
  "suite-sparse/lib/libcxsparse.4.dylib"
  "${_boost_id_prefix}lib/libboost_filesystem-mt.dylib"
  "${_boost_id_prefix}lib/libboost_system-mt.dylib"
  "${_boost_id_prefix}lib/libboost_atomic-mt.dylib"
  "${_boost_id_prefix}lib/libboost_process.dylib"
)
foreach(_dylib_rel_path ${_dylibs_rel_path})
  set(_old_lib_id "${HOMEBREW_PREFIX}/opt/${_dylib_rel_path}")
  cmake_path(GET _old_lib_id FILENAME _dylib)
  set(_new_lib_id "@rpath/lib/${_dylib}")

  message("-- In ${DAISY_BIN_NAME}: Change ${_old_lib_id} -> ${_new_lib_id}")
  add_custom_command(TARGET daisy
    POST_BUILD
    COMMAND "install_name_tool"
    ARGS "-change" "${_old_lib_id}" "${_new_lib_id}"
    "${DAISY_BIN_NAME}"
  )
endforeach()

# We also need to update the path of libomp in libsuitesparseconfig
set(_old_lib_id "${HOMEBREW_PREFIX}/opt/libomp/lib/libomp.dylib")
set(_new_lib_id "@rpath/libomp.dylib")
set(_suitesparseconfig "bin/lib/libsuitesparseconfig.7.dylib")
message("-- In ${_suitesparseconfig}: Change ${_old_lib_id} -> ${_new_lib_id}")
add_custom_command(TARGET daisy
  POST_BUILD
  COMMAND "install_name_tool"
  ARGS "-change" "${_old_lib_id}" "${_new_lib_id}"
  "${_suitesparseconfig}"
)
