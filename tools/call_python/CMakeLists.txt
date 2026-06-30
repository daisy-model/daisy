cmake_minimum_required(VERSION 3.15)
project(call_python_from_daisy LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)

add_executable(call_python_function call_python_function.cpp)
target_link_libraries(call_python_function PRIVATE pybind11::embed)
