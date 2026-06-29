target_sources(${DAISY_CORE_NAME} PRIVATE
  program.C
  program_GP2D.C
  program_KM2.C
  program_astdump.C
  program_batch.C
  program_cpedata.C
  program_document.C
  program_extract.C
  program_file.C
  program_hmovie.C
  program_nwaps.C
  program_optimize.C
  program_osvaldo.C
  program_post.C
  program_rootmatch.C
  program_sbrdata.C
  program_spawn.C
  program_weather.C
)

# ===== daisy_bmi pybind11 module =====
pybind11_add_module(daisy_bmi api_bindings.cpp bmi.C daisy_bmi.C)
target_link_libraries(daisy_bmi PRIVATE ${DAISY_CORE_NAME})
target_include_directories(daisy_bmi PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)


# ===== Copy Python module and runtime dependencies =====
if (WIN32)
    # Where the Python package lives (src layout)
    set(PYTHON_PACKAGE_DIR "${CMAKE_SOURCE_DIR}/python/src/daisy")

    # Derive the MinGW/MSYS2 bin directory from the compiler path.
    # This avoids any hardcoded path: wherever CMake found the compiler is
    # where the runtime DLLs live (e.g. C:/msys64/ucrt64/bin).
    get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    message(STATUS "daisy_bmi: Python package dir : ${PYTHON_PACKAGE_DIR}")
    message(STATUS "daisy_bmi: MinGW runtime dir  : ${COMPILER_DIR}")

    add_custom_command(TARGET daisy_bmi POST_BUILD

        # Ensure target directory exists
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PYTHON_PACKAGE_DIR}"

        # --- Copy the Python module (.pyd) ---
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:daisy_bmi>"
            "${PYTHON_PACKAGE_DIR}"

        # --- Copy Daisy core DLL ---
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${DAISY_CORE_NAME}>"
            "${PYTHON_PACKAGE_DIR}"

        # --- Copy MinGW/MSYS2 runtime DLLs ---
        # Direct runtime dependencies of libcore.dll:
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/libstdc++-6.dll"
            "${PYTHON_PACKAGE_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/libgcc_s_seh-1.dll"
            "${PYTHON_PACKAGE_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/libwinpthread-1.dll"
            "${PYTHON_PACKAGE_DIR}"

        # Transitive dependencies (libcore -> libcxsparse -> libsuitesparseconfig -> libgomp-1):
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/libcxsparse.dll"
            "${PYTHON_PACKAGE_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/libsuitesparseconfig.dll"
            "${PYTHON_PACKAGE_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COMPILER_DIR}/libgomp-1.dll"
            "${PYTHON_PACKAGE_DIR}"
    )
endif()
