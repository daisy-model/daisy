SET(COMPILER_OPTIONS_COVERAGE CACHE STRING "Flags used by the CXX compiler during COVERAGE builds")
SET(LINKER_OPTIONS_COVERAGE CACHE STRING "Flags used by the linker during COVERAGE builds")
MARK_AS_ADVANCED(
  COMPILER_OPTIONS_COVERAGE,
  LINKER_OPTIONS_COVERAGE
)
