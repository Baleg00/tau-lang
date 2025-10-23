function(tau_add_code_coverage_target)
  if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    find_program(GCOV_EXECUTABLE NAMES gcov)
    find_program(LCOV_EXECUTABLE NAMES lcov)
    find_program(GENHTML_EXECUTABLE NAMES genhtml)

    if (NOT GCOV_EXECUTABLE)
      message(WARNING "gcov not found - coverage data will not be generated.")
    endif ()

    if (NOT LCOV_EXECUTABLE)
      message(WARNING "lcov not found - coverage data will not be generated.")
    endif ()

    if (NOT GENHTML_EXECUTABLE)
      message(WARNING "genhtml not found - HTML coverage report will not be generated.")
    endif ()

    add_custom_target(code_coverage
      COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
      COMMAND ${CMAKE_COMMAND} -E make_directory ${TAU_CODE_COVERAGE_DIR}
      COMMAND ${LCOV_EXECUTABLE} --ignore-errors gcov,mismatch -c -d ${CMAKE_BINARY_DIR} -o ${TAU_CODE_COVERAGE_DIR}/coverage.info -j 16
      COMMAND ${GENHTML_EXECUTABLE} -o ${TAU_CODE_COVERAGE_DIR} --dark-mode -j 16 ${TAU_CODE_COVERAGE_DIR}/coverage.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Running tests and generating coverage report..."
      VERBATIM
    )
  else ()
    message(WARNING "Unsupported compiler for coverage: ${CMAKE_C_COMPILER_ID}")
  endif ()
endfunction()

function(tau_target_code_coverage_configure TARGET)
  if (NOT CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    message(WARNING
      "Code coverage is only supported with Debug or RelWithDebInfo builds. "
      "Current build type: ${CMAKE_BUILD_TYPE}"
    )
  endif ()

  if (NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    message(WARNING "Unsupported compiler for coverage: ${CMAKE_C_COMPILER_ID}")
  endif ()

  set(TAU_COVERAGE_COMPILE_OPTIONS
    $<$<C_COMPILER_ID:GNU,Clang>:-Og> # Optimize for debugging experience.
    $<$<C_COMPILER_ID:GNU,Clang>:--coverage> # Generate code instrumented for coverage analysis.
  )

  set(TAU_COVERAGE_LINK_OPTIONS
    $<$<C_COMPILER_ID:GNU,Clang>:-lgcov> # Link against gcov library.
    $<$<C_COMPILER_ID:GNU,Clang>:--coverage> # Generate code instrumented for coverage analysis.
  )

  target_compile_options(${TARGET} PRIVATE ${TAU_COVERAGE_COMPILE_OPTIONS})
  target_link_options(${TARGET} PRIVATE ${TAU_COVERAGE_LINK_OPTIONS})
endfunction()
