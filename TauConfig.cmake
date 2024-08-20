# Include project header files located in the `inc` directory.
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Set compiler options based on the compiler ID.
# For MSVC (Microsoft Visual C++):
if (CMAKE_C_COMPILER_ID MATCHES MSVC)
  set(_TAU_COMPILE_OPTIONS
    /FIbase.h                 # Force include `base.h` before processing other headers.
    $<$<CONFIG:Debug>:/Wall>  # Enable all warnings in Debug mode.
    $<$<CONFIG:Debug>:/WX>    # Treat all warnings as errors in Debug mode.
    $<$<CONFIG:Debug>:/Od>    # Disable optimizations in Debug mode.
    $<$<CONFIG:Release>:/Ot>  # Optimize for speed in Release mode.
  )
# For Clang or GNU compilers:
elseif (CMAKE_C_COMPILER_ID MATCHES Clang OR CMAKE_C_COMPILER_ID MATCHES GNU)
  set(_TAU_COMPILE_OPTIONS
    -include base.h               # Force include `base.h` before processing other headers.
    $<$<CONFIG:Debug>:-Wall>      # Enable all warnings in Debug mode.
    $<$<CONFIG:Debug>:-Wextra>    # Enable extra warnings in Debug mode.
    $<$<CONFIG:Debug>:-Wpedantic> # Enable pedantic warnings in Debug mode.
    $<$<CONFIG:Debug>:-Werror>    # Treat all warnings as errors in Debug mode.
    $<$<CONFIG:Debug>:-Og>        # Optimize for debugging experience in Debug mode.
    $<$<CONFIG:Release>:-O3>      # Optimize for speed in Release mode.
  )
else ()
  message(FATAL_ERROR "Unknown C Compiler ID: ${CMAKE_C_COMPILER_ID}")
endif ()

# Store the compile options in a global property for later use.
set_property(GLOBAL PROPERTY TAU_COMPILE_OPTIONS_PROPERTY ${_TAU_COMPILE_OPTIONS})
message(VERBOSE "Tau Options: ${_TAU_COMPILE_OPTIONS}")

# Set compile definitions, including versioning and conditional flags.
set(_TAU_COMPILE_DEFINITIONS
  TAU_VERSION="${CMAKE_PROJECT_VERSION}"            # Define the Tau version using the project's version.
  $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>  # Disable security warnings on MSVC.
  $<$<CONFIG:Debug>:TAU_DEBUG>                      # Define TAU_DEBUG in Debug mode.
  $<$<CONFIG:Release>:TAU_RELEASE>                  # Define TAU_RELEASE in Release mode.
)

# Store the compile definitions in a global property for later use.
set_property(GLOBAL PROPERTY TAU_COMPILE_DEFINITIONS_PROPERTY ${_TAU_COMPILE_DEFINITIONS})
message(VERBOSE "Tau Definitions: ${_TAU_COMPILE_DEFINITIONS}")
