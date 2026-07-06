target_sources(${DAISY_CORE_NAME} PRIVATE
  registration.C
)

add_subdirectory(bioclimate)
add_subdirectory(litter)
add_subdirectory(surface)
add_subdirectory(vegetation)
add_subdirectory(weather)
