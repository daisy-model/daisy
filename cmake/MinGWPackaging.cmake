# Windows package
# CPack stores the registry key. Here we define the location so it is available in path.C
# The format is taken from
# https://github.com/Kitware/CMake/blob/3f7493de72251f3dac721f960456ea474f15a330/Modules/Internal/CPack/NSIS.template.in#L661C1-L662C103
# where it is defined as
# Software\${CPACK_PACKAGE_VENDOR}\${CPACK_PACKAGE_VENDOR}\${CPACK_PACKAGE_INSTALL_REGISTRY_KEY}
#
# However, CPACK_PACKAGE_INSTALL_REGISTRY_KEY is not defined as this point. So instead we replicate the definition from 
# https://github.com/Kitware/CMake/blob/3f7493de72251f3dac721f960456ea474f15a330/CMakeCPack.cmake#L38
# where it is defined as ${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}
#
# So unless things change upstream, the registry key will be something like
# Software\${CPACK_PACKAGE_VENDOR}\${CPACK_PACKAGE_VENDOR}\${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}
# CPACK_PACKAGE_NAME and CPACK_PACKAGE_VERSION are also not defined at this time, so instead we use
# CMAKE_PROJECT_NAME and CPACK_PACKAGE_VERSION, which should be identical unless someone changed it...
add_definitions( -DWINDOWS_INSTALL_LOCATION_REG_KEY="Software\\\\${CPACK_PACKAGE_VENDOR}\\\\${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_GENERATOR NSIS)