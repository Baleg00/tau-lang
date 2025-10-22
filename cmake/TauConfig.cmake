function(tau_target_configure TARGET)
  if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set(TAU_COMPILE_OPTIONS
      /FIutils/suppress_warnings.h # Force include `suppress_warnings.h` before processing other headers.

      $<$<CONFIG:Debug>:/Wall> # Enable all warnings.
      $<$<CONFIG:Debug>:/WX> # Treat all warnings as errors.
      $<$<CONFIG:Debug>:/Od> # Disable optimizations.
      $<$<CONFIG:Debug,RelWithDebInfo>:/Zi> # Generate separate PDB file for debugging information.
      $<$<CONFIG:Debug>:/RTCcsu> # Enable runtime error checks.

      $<$<CONFIG:MinSizeRel>:/O1> # Optimize for size.
      $<$<CONFIG:Release,RelWithDebInfo>:/O2> # Optimize for speed.
      $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:/DNDEBUG> # Disable debug checks.
    )

    set(TAU_LINK_OPTIONS
      $<$<CONFIG:Debug,RelWithDebInfo>:/DEBUG> # Generate separate PDB file for debugging information.
    )
  elseif (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(TAU_COMPILE_OPTIONS
      -include utils/suppress_warnings.h # Force include `suppress_warnings.h` before processing other headers.

      $<$<CONFIG:Debug>:-Wall> # Enable all warnings.
      $<$<CONFIG:Debug>:-Wextra> # Enable extra warnings.
      $<$<CONFIG:Debug>:-Wpedantic> # Enable pedantic warnings.
      $<$<CONFIG:Debug>:-Werror> # Treat all warnings as errors.
      $<$<CONFIG:Debug>:-Og> # Optimize for debugging experience.
      $<$<CONFIG:Debug,RelWithDebInfo>:-g> # Generate debugging information.

      $<$<CONFIG:MinSizeRel>:-Os> # Optimize for size.
      $<$<CONFIG:RelWithDebInfo>:-O2> # Enable level 2 optimizations.
      $<$<CONFIG:Release>:-O3> # Enable level 3 optimizations.
      $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-DNDEBUG> # Disable debug checks.
    )

    set(TAU_LINK_OPTIONS "")
  else ()
    message(SEND_ERROR "Unknown C Compiler ID: ${CMAKE_C_COMPILER_ID}")
  endif ()

  set(TAU_COMPILE_DEFINITIONS
    TAU_VERSION="${CMAKE_PROJECT_VERSION}" # Project version as string.

    $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS> # Disable security warnings on MSVC.

    TAU_RELEASE=$<CONFIG:Release,RelWithDebInfo,MinSizeRel> # Define TAU_RELEASE as 1 on release builds, 0 otherwise.
    TAU_DEBUG=$<CONFIG:Debug> # Define TAU_DEBUG as 1 on debug build, 0 otherwise.
  )

  target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/inc)
  target_compile_options(${TARGET} PRIVATE ${TAU_COMPILE_OPTIONS})
  target_link_options(${TARGET} PRIVATE ${TAU_LINK_OPTIONS})
  target_compile_definitions(${TARGET} PRIVATE ${TAU_COMPILE_DEFINITIONS})
endfunction()

function(tau_add_library TARGET)
  file(GLOB_RECURSE TAU_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
  list(REMOVE_ITEM TAU_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/main.c)

  add_library(${TARGET} STATIC EXCLUDE_FROM_ALL ${TAU_SOURCE_FILES})
endfunction()

function(tau_add_executable TARGET)
  add_executable(${TARGET} ${PROJECT_SOURCE_DIR}/src/main.c)
endfunction()
