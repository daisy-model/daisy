set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libsuitesparse-dev, libboost-filesystem-dev, libboost-system-dev"
)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/cmake/debian/postrm;"
)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Silas Ørting<silas@di.ku.dk>")
set(CPACK_GENERATOR DEB)
