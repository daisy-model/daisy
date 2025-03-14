if (${OS} STREQUAL "mingw")
  # This needs to be set such that it matches the location of the downloaded python distribution
  set(Python_ROOT_DIR "${CMAKE_SOURCE_DIR}/python/python-3.13.0-embed-amd64")
  install(DIRECTORY ${Python_ROOT_DIR}/ DESTINATION python COMPONENT python)
endif()
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)
link_libraries(pybind11::embed)