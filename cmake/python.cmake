if (${OS} STREQUAL "mingw")
  # Use MSYS2 UCRT Python for building; embedded Python is only for packaging/runtime
  # This needs to be set such that it matches the location of the downloaded python distribution
  set(Python_ROOT_DIR "${CMAKE_SOURCE_DIR}/python/python")
  install(DIRECTORY ${Python_ROOT_DIR}/ TYPE BIN COMPONENT python)
  file(INSTALL ${Python_ROOT_DIR}/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  # Path to the embedded Python we ship with releases
  set(DAISY_WIN_PY_EMBED_DIR "${CMAKE_SOURCE_DIR}/python/python")
  install(DIRECTORY ${DAISY_WIN_PY_EMBED_DIR}/ TYPE BIN COMPONENT python)
  file(INSTALL ${DAISY_WIN_PY_EMBED_DIR}/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)
link_libraries(pybind11::embed)
