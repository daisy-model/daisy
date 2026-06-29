include(GNUInstallDirs)
set(DAISY_SAMPLE_DESTINATION "${CMAKE_INSTALL_DATADIR}/daisy/sample")
set(DAISY_LIB_DESTINATION "${CMAKE_INSTALL_DATADIR}/daisy/lib")
set(DAISY_CORE_NAME ${DAISY_BIN_NAME})

set(DAISY_CXSPARSE_LIBRARY cxsparse)
if (DAISY_FLATPAK)
  find_library(
    DAISY_CXSPARSE_LIBRARY
    NAMES cxsparse
    HINTS /app/lib /app/lib64
    REQUIRED
  )
endif()

target_include_directories(${DAISY_BIN_NAME} PUBLIC include)
target_compile_options(${DAISY_BIN_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_options(${DAISY_BIN_NAME} PRIVATE ${LINKER_OPTIONS})
target_link_libraries(${DAISY_BIN_NAME} PUBLIC
  ${DAISY_CXSPARSE_LIBRARY}
  Boost::filesystem
  $<$<BOOL:${DAISY_FLATPAK}>:Boost::process>
)

install(TARGETS ${DAISY_BIN_NAME} RUNTIME DESTINATION bin)
