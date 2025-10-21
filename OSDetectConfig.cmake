# Detect target operating system based on CMAKE_SYSTEM_NAME
# and define corresponding preprocessor macros for use in C code.

# Set TAU_OS_WINDOWS if targeting Windows
if(CMAKE_SYSTEM_NAME MATCHES Windows)
  set(TAU_OS_WINDOWS 1)
endif()

# Set TAU_OS_DARWIN if targeting macOS
if(CMAKE_SYSTEM_NAME MATCHES Darwin)
  set(TAU_OS_DARWIN 1)
endif()

# Set TAU_OS_LINUX if targeting Linux
if(CMAKE_SYSTEM_NAME MATCHES Linux)
  set(TAU_OS_LINUX 1)
endif()

# Send error if target operating system is not recognized
if(NOT DEFINED TAU_OS_WINDOWS AND
   NOT DEFINED TAU_OS_DARWIN AND
   NOT DEFINED TAU_OS_LINUX)
  message(SEND_ERROR "Unknown target OS: ${CMAKE_SYSTEM_NAME}")
endif()

# Generate os_detect.h from template (os_detect.h.in)
# The generated header will contain the appropriate macros defined above.
configure_file(${PROJECT_SOURCE_DIR}/inc/utils/os_detect.h.in
  ${PROJECT_SOURCE_DIR}/inc/utils/os_detect.h)
