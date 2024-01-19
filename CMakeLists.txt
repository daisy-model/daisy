cmake_minimum_required(VERSION 3.25)
project(
  daisy
  VERSION 6.44
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_PROFILE "Set to ON to build for profiling" OFF)
option(MAKE_PORTABLE "Set to ON to make a generic build" OFF)

SET(CMAKE_CXX_FLAGS_COVERAGE
  "--coverage -O1"
  CACHE STRING "Flags used by the CXX compiler during COVERAGE builds"
)
SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE
  "--coverage"
  CACHE STRING "Flags used by the linker during COVERAGE builds"
)
MARK_AS_ADVANCED(
  CMAKE_CXX_FLAGS_COVERAGE,
  CMAKE_EXE_LINKER_FLAGS_COVERAGE
)

IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release
       CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel Coverage."
       FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

add_compile_options(${WARNING_COMPILE_OPTIONS})

find_package(Boost 1.74 REQUIRED CONFIG COMPONENTS filesystem system)
message(STATUS "Boost version: ${Boost_VERSION}")

add_executable(daisy)

# Sources are added with target_sources in CMakeLists in the source tree
add_subdirectory(src)

if (USE_PROFILE)
  target_compile_options(daisy PRIVATE ${PROFILE_COMPILE_OPTIONS})
endif()

if (MAKE_PORTABLE)
  target_compile_options(daisy PRIVATE ${PORTABLE_COMPILE_OPTIONS})
else()
  target_compile_options(daisy PRIVATE ${NATIVE_COMPILE_OPTIONS})
endif()

target_include_directories(daisy PUBLIC include)
target_link_directories(daisy INTERFACE Boost::Boost)
target_link_libraries(daisy PUBLIC
  stdc++
  ${Boost_LIBRARIES}
  cxsparse
  dl
  pthread
)

target_compile_options(daisy PRIVATE
  "$<$<CONFIG:RELEASE>:${RELEASE_COMPILE_OPTIONS}>"
)

install(TARGETS daisy DESTINATION bin)

# Contains .dai files that define functionality
add_subdirectory(lib)

include(CTest)
add_subdirectory(test)
