cmake_minimum_required(VERSION 3.25)

foreach(_required_var DAISY_EXECUTABLE DAISY_STAGING_DIR DAISY_DYLIB_TARGET_DIR HOMEBREW_PREFIX)
  if(NOT DEFINED ${_required_var} OR "${${_required_var}}" STREQUAL "")
    message(FATAL_ERROR "Missing required variable ${_required_var}")
  endif()
endforeach()

function(_daisy_run_install_name_tool)
  execute_process(
    COMMAND install_name_tool ${ARGN}
    RESULT_VARIABLE _result
    ERROR_VARIABLE _error
  )
  if(NOT _result EQUAL 0)
    string(JOIN " " _command ${ARGN})
    message(FATAL_ERROR "install_name_tool ${_command} failed: ${_error}")
  endif()
endfunction()

function(_daisy_fixup_load_commands target mode)
  execute_process(
    COMMAND otool -L "${target}"
    RESULT_VARIABLE _result
    OUTPUT_VARIABLE _output
    ERROR_VARIABLE _error
  )
  if(NOT _result EQUAL 0)
    message(FATAL_ERROR "otool -L ${target} failed: ${_error}")
  endif()

  string(REPLACE "\n" ";" _lines "${_output}")
  list(REMOVE_AT _lines 0)
  foreach(_line IN LISTS _lines)
    string(STRIP "${_line}" _line)
    if(_line STREQUAL "")
      continue()
    endif()
    string(REGEX REPLACE "^([^ ]+) .*" "\\1" _old_name "${_line}")
    cmake_path(GET _old_name FILENAME _dep_name)

    set(_new_name "")
    if(_dep_name IN_LIST _daisy_staged_lib_names)
      if(mode STREQUAL "EXECUTABLE")
        set(_new_name "@rpath/lib/${_dep_name}")
      else()
        set(_new_name "@loader_path/${_dep_name}")
      endif()
    elseif(DEFINED DAISY_PYTHON_DYLIB_NAME AND NOT "${DAISY_PYTHON_DYLIB_NAME}" STREQUAL "" AND _dep_name STREQUAL "${DAISY_PYTHON_DYLIB_NAME}")
      if(mode STREQUAL "EXECUTABLE")
        set(_new_name "@executable_path/../python/lib/${_dep_name}")
      else()
        set(_new_name "@loader_path/../python/lib/${_dep_name}")
      endif()
    endif()

    if(NOT _new_name STREQUAL "" AND NOT _old_name STREQUAL _new_name)
      _daisy_run_install_name_tool(-change "${_old_name}" "${_new_name}" "${target}")
    endif()
  endforeach()
endfunction()

set(_search_dirs
  "${HOMEBREW_PREFIX}/lib"
  "${HOMEBREW_PREFIX}/opt/libomp/lib"
  "${DAISY_DYLIB_TARGET_DIR}"
)
if(DEFINED DAISY_PYTHON_ROOT_DIR AND NOT "${DAISY_PYTHON_ROOT_DIR}" STREQUAL "")
  list(APPEND _search_dirs "${DAISY_PYTHON_ROOT_DIR}/lib")
endif()
list(REMOVE_DUPLICATES _search_dirs)

set(_tmp_dylib_dir "${DAISY_DYLIB_TARGET_DIR}.tmp")
file(REMOVE_RECURSE "${_tmp_dylib_dir}")
file(MAKE_DIRECTORY "${_tmp_dylib_dir}")

file(GET_RUNTIME_DEPENDENCIES
  RESOLVED_DEPENDENCIES_VAR _resolved_deps
  UNRESOLVED_DEPENDENCIES_VAR _unresolved_deps
  EXECUTABLES "${DAISY_EXECUTABLE}"
  DIRECTORIES ${_search_dirs}
  POST_EXCLUDE_REGEXES
    "^/System/Library/"
    "^/usr/lib/"
)

if(_unresolved_deps)
  list(JOIN _unresolved_deps "\n  " _joined_unresolved)
  message(FATAL_ERROR "Unresolved macOS runtime dependencies:\n  ${_joined_unresolved}")
endif()

set(_deps_to_stage "")
foreach(_dep IN LISTS _resolved_deps)
  if(DEFINED DAISY_PYTHON_ROOT_DIR AND NOT "${DAISY_PYTHON_ROOT_DIR}" STREQUAL "")
    string(FIND "${_dep}" "${DAISY_PYTHON_ROOT_DIR}/" _python_root_prefix_index)
    if(_python_root_prefix_index EQUAL 0)
      continue()
    endif()
  endif()
  list(APPEND _deps_to_stage "${_dep}")
endforeach()
list(REMOVE_DUPLICATES _deps_to_stage)

if(_deps_to_stage)
  file(COPY ${_deps_to_stage}
    DESTINATION "${_tmp_dylib_dir}"
    FOLLOW_SYMLINK_CHAIN
  )
endif()

set(_daisy_staged_lib_names "")
file(GLOB _staged_libs RELATIVE "${_tmp_dylib_dir}" "${_tmp_dylib_dir}/*.dylib")
foreach(_staged_lib IN LISTS _staged_libs)
  list(APPEND _daisy_staged_lib_names "${_staged_lib}")
endforeach()

foreach(_staged_lib_name IN LISTS _daisy_staged_lib_names)
  set(_staged_lib_path "${_tmp_dylib_dir}/${_staged_lib_name}")
  _daisy_run_install_name_tool(-id "@rpath/lib/${_staged_lib_name}" "${_staged_lib_path}")
  _daisy_fixup_load_commands("${_staged_lib_path}" "DYLIB")
endforeach()

_daisy_fixup_load_commands("${DAISY_EXECUTABLE}" "EXECUTABLE")

file(REMOVE_RECURSE "${DAISY_DYLIB_TARGET_DIR}")
file(RENAME "${_tmp_dylib_dir}" "${DAISY_DYLIB_TARGET_DIR}")
