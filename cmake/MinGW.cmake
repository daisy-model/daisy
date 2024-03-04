# On Windows we build everything except main as a shared library (core)
# This means that we need to distribute the relevant shared libraries on windows
add_library(core SHARED)
target_link_libraries(core PUBLIC
  cxsparse
  Boost::filesystem
)
target_link_options(core PRIVATE ${LINKER_OPTIONS})
add_library(windows SHARED)
add_subdirectory(src/windows)
target_link_libraries(core PUBLIC
  windows
  ${WIN_LIBS}
)

# On Windows we need to install the dlls that we build and we need to
# install the dlls we depend on.
install(TARGETS core RUNTIME DESTINATION bin)
install(TARGETS windows RUNTIME DESTINATION bin)
install(FILES
  $ENV{MINGW_PREFIX}/bin/libstdc++-6.dll
  $ENV{MINGW_PREFIX}/bin/libwinpthread-1.dll
  $ENV{MINGW_PREFIX}/bin/libgcc_s_seh-1.dll
  $ENV{MINGW_PREFIX}/bin/libcxsparse.dll
  $ENV{MINGW_PREFIX}/bin/libboost_filesystem-mt.dll
  TYPE BIN
)

set(DAISY_SAMPLE_DESTINATION "sample")
set(DAISY_LIB_DESTINATION "lib")
set(DAISY_CORE_NAME core)
