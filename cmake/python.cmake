if (${OS} STREQUAL "mingw")
  # This needs to be set such that it matches the location of the downloaded python distribution
  set(Python_ROOT_DIR "${CMAKE_SOURCE_DIR}/python/python")
  install(DIRECTORY ${Python_ROOT_DIR}/ TYPE BIN COMPONENT python)
  file(INSTALL ${Python_ROOT_DIR}/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)
link_libraries(pybind11::embed)
