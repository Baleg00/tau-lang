function(tau_os_detect_configure_file)
  set(TAU_OS_WINDOWS 0)
  set(TAU_OS_DARWIN 0)
  set(TAU_OS_LINUX 0)

  if (CMAKE_SYSTEM_NAME MATCHES Windows)
    set(TAU_OS_WINDOWS 1)
  endif ()

  if (CMAKE_SYSTEM_NAME MATCHES Darwin)
    set(TAU_OS_DARWIN 1)
  endif ()

  if (CMAKE_SYSTEM_NAME MATCHES Linux)
    set(TAU_OS_LINUX 1)
  endif ()

  if (NOT CMAKE_SYSTEM_NAME MATCHES "Windows|Darwin|Linux")
    message(SEND_ERROR "Unknown target OS: ${CMAKE_SYSTEM_NAME}")
  endif ()

  configure_file(${PROJECT_SOURCE_DIR}/inc/utils/os_detect.h.in
    ${PROJECT_SOURCE_DIR}/inc/utils/os_detect.h)
endfunction()
