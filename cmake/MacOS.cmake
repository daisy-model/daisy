set(DAISY_SAMPLE_DESTINATION "sample")
set(DAISY_LIB_DESTINATION "lib")
set(DAISY_CORE_NAME ${DAISY_BIN_NAME})

target_include_directories(${DAISY_BIN_NAME} PUBLIC include)
target_compile_options(${DAISY_BIN_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_options(${DAISY_BIN_NAME} PRIVATE ${LINKER_OPTIONS})
target_link_libraries(${DAISY_BIN_NAME} PUBLIC
  cxsparse
  Boost::filesystem
)
target_link_directories(${DAISY_BIN_NAME} PRIVATE ${EXTRA_SYSTEM_INCLUDE_DIRECTORIES})

install(TARGETS ${DAISY_BIN_NAME} RUNTIME DESTINATION bin)