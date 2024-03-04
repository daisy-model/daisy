# Windows package
# CPack stores the registry key. Here we define the location so it is available in path.C
add_definitions( -DWINDOWS_INSTALL_LOCATION_REG_KEY="Software\\\\${CPACK_PACKAGE_VENDOR}\\\\daisy ${CMAKE_PROJECT_VERSION}" )
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_GENERATOR NSIS)
