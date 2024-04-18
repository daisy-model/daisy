cmake_minimum_required(VERSION 3.25)
project(
  daisy
  VERSION 7.0.0
  DESCRIPTION "Mechanistic simulation of agricultural fields"
  HOMEPAGE_URL https://daisy.ku.dk/
  LANGUAGES CXX C
)
include(cmake/AddCoverageBuildType.cmake)

# Make Daisy version available to version.C
add_definitions( -DDAISY_VERSION="${CMAKE_PROJECT_VERSION}" )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_DOC "Set to ON to build documentation" OFF)
option(BUILD_CXX_TESTS "Set to ON to build C++ tests" OFF)
option(USE_PROFILE "Set to ON to build for profiling" OFF)
option(MAKE_PORTABLE "Set to ON to make a generic build" OFF)

if (NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release
       CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel Coverage."
       FORCE)
endif ()

add_compile_options(
  ${COMPILER_OPTIONS_WARNING}
  ${COMPILER_OPTIONS_NO_WARNING}
 )

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

set(DAISY_BIN_NAME daisy)
add_executable(${DAISY_BIN_NAME})
if (${OS} STREQUAL "mingw")
  include(cmake/MinGW.cmake)
elseif(${OS} STREQUAL "linux")
  include(cmake/Linux.cmake)
elseif(${OS} STREQUAL "macos")
  include(cmake/MacOS.cmake)
endif()

# Sources are added with target_sources in CMakeLists in the source tree
add_subdirectory(src)

# Test
include(CTest)
add_subdirectory(test)

# Packaging
# lib/ and sample/ contain .dai files that define functionality that should be installed
add_subdirectory(lib)
add_subdirectory(sample)

# General packaging stuff
include(cmake/Packaging.cmake)

# Source package
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")

if (${OS} STREQUAL "mingw")
  include(cmake/MinGWPackaging.cmake)
elseif (${OS} STREQUAL "linux")
  include(cmake/DebianPackaging.cmake)
elseif (${OS} STREQUAL "macos")
  include(cmake/MacOSPackaging.cmake)
endif()

include(CPack)

# Documentation
include(cmake/Doc.cmake)
