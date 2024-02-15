cmake_minimum_required(VERSION 3.25)
project(
  daisy
  VERSION 7.0.0
  DESCRIPTION "Mechanistic simulation of agricultural fields"
  HOMEPAGE_URL https://daisy.ku.dk/
  LANGUAGES CXX
)
add_definitions( -DDAISY_VERSION="${CMAKE_PROJECT_VERSION}" )

include(cmake/AddCoverageBuildType.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_PROFILE "Set to ON to build for profiling" OFF)
option(MAKE_PORTABLE "Set to ON to make a generic build" OFF)

if (NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release
       CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel Coverage."
       FORCE)
endif (NOT CMAKE_BUILD_TYPE)

add_compile_options(
  ${COMPILER_OPTIONS_WARNING}
  ${COMPILER_OPTIONS_NO_WARNING}
 )

if (MAKE_PORTABLE)
  set(Boost_USE_STATIC_LIBS ON)
endif (MAKE_PORTABLE)
find_package(Boost 1.74 REQUIRED CONFIG COMPONENTS filesystem system)
message(STATUS "Boost version: ${Boost_VERSION}")

SET(COMPILE_OPTIONS
  ${OS_OPTIONS}
  "$<$<CONFIG:RELEASE>:${COMPILER_OPTIONS_RELEASE}>"
  "$<$<CONFIG:COVERAGE>:${COMPILER_OPTIONS_COVERAGE}>"
  "$<$<BOOL:${USE_PROFILE}>:${COMPILER_OPTIONS_PROFILE}>"
  "$<IF:$<BOOL:${MAKE_PORTABLE}>,${COMPILER_OPTIONS_PORTABLE},${COMPILER_OPTIONS_NATIVE}>"
)

SET(LINKER_OPTIONS
  "$<$<CONFIG:COVERAGE>:${LINKER_OPTIONS_COVERAGE}>"
  "$<$<BOOL:${MAKE_PORTABLE}>:${LINKER_OPTIONS_PORTABLE}>"
)

# Shared libraries
add_library(core SHARED)
target_include_directories(core PUBLIC include)
target_compile_options(core PRIVATE ${COMPILE_OPTIONS})

add_library(gnuplot SHARED)
target_include_directories(gnuplot PUBLIC include)
target_compile_options(gnuplot PRIVATE ${COMPILE_OPTIONS})

add_library(object_model SHARED)
target_include_directories(object_model PUBLIC include)
target_compile_options(object_model PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(object_model PUBLIC gnuplot)

add_library(programs SHARED)
target_include_directories(programs PUBLIC include)
target_compile_options(programs PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(programs PUBLIC
  core
  Boost::filesystem
)

add_library(ui SHARED)
target_include_directories(ui PUBLIC include)
target_compile_options(ui PRIVATE ${COMPILE_OPTIONS})

add_library(util SHARED)
target_include_directories(util PUBLIC include)
target_compile_options(util PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(util PUBLIC
  cxsparse
)

# Daisy executable
add_executable(daisy)
target_include_directories(daisy PUBLIC include)
target_compile_options(daisy PRIVATE ${COMPILE_OPTIONS})
target_link_libraries(daisy PUBLIC
  core
  gnuplot
  object_model
  programs
  ui
  util
)
target_link_directories(daisy PRIVATE "${LINKER_DIRS}")
target_link_options(daisy PRIVATE ${LINKER_OPTIONS})

# Sources are added with target_sources in CMakeLists in the source tree
add_subdirectory(src)


# Test
include(CTest)
add_subdirectory(test)


# Packaging
# Contains .dai files that define functionality and should be installed
add_subdirectory(lib)
add_subdirectory(sample)
install(TARGETS daisy RUNTIME DESTINATION bin)

# General packaging stuff
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_SOURCE_IGNORE_FILES
  /.git*
  /cmake
  /.*build.*
)

# Source package
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")

# Debian package
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libsuitesparse-dev, libboost-filesystem-dev, libboost-system-dev"
)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/cmake/debian/postrm;"
)
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Silas Ørting<silas@di.ku.dk>")


# Windows package
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "SetOutPath $INSTDIR")
SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
  "WriteRegStr HKCU 'Software\\\\Daisy' '${CMAKE_PROJECT_VERSION}' 'Install directory' '$INSTDIR'"
)
SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
  "HKCU 'Software\\\\Daisy' '${CMAKE_PROJECT_VERSION}' 'Install directory'"
)

include(CPack)
