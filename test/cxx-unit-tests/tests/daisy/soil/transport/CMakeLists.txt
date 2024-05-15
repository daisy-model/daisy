cxx_unit_test(ut_geometry1d
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/geometry1d.C
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/geometry.C
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/geometry_vert.C
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/volume.C
)

cxx_unit_test(ut_geometry_rect
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/geometry_rect.C
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/geometry.C
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/geometry_vert.C
  ${CMAKE_SOURCE_DIR}/src/daisy/soil/transport/volume.C
)
