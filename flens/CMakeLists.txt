install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${INCLUDE_INSTALL_DIR}
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.tcc"
  PATTERN "*.cxx"
  PATTERN "examples" EXCLUDE
  PATTERN "test" EXCLUDE
  PATTERN "blas/interface" EXCLUDE
  PATTERN "lapack/interface" EXCLUDE
  )
