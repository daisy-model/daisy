cmake_minimum_required(VERSION 3.25)
project(
  daisy
  VERSION 6.44
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
  -Wpedantic
  -Wall
  -Wextra
  -Wvariadic-macros 
  -Wvla
  -Wmissing-declarations
  -Wfloat-equal
  -Wcast-qual
  -Woverloaded-virtual
  -Wundef
  -Wpointer-arith
  -Wwrite-strings
  -Wcast-align
  -Wmissing-format-attribute
  -Wold-style-cast
  -Wformat=2
  -Winit-self
  -Wsign-promo
  -Wredundant-decls
  -Wno-unused-parameter
  -Wno-sign-compare
  -Wno-mismatched-tags
)

find_package(Boost 1.74 REQUIRED CONFIG COMPONENTS filesystem system)
message(STATUS "Boost version: ${Boost_VERSION}")

add_executable(daisy)

# Sources are added with target_sources in CMakeLists in the source tree
add_subdirectory(src) 

target_include_directories(daisy PUBLIC include)
target_link_directories(daisy INTERFACE Boost::Boost)
target_link_libraries(daisy PUBLIC
  stdc++
  ${Boost_LIBRARIES}
  cxsparse
  dl
  pthread
)

set(RELEASE_COMPILE_OPTIONS
  -ffast-math
  -fno-finite-math-only
  -mtune=native
  -march=native
) 
target_compile_options(daisy PRIVATE
  "$<$<CONFIG:RELEASE>:${RELEASE_COMPILE_OPTIONS}>"
)
