cmake_minimum_required(VERSION 3.0.0)

project(tau
  VERSION 0.1.0
  DESCRIPTION "A strictly typed, compiled, general purpose programming language."
  LANGUAGES C CXX)

include(CTest)
enable_testing()

add_compile_definitions(
  _CRT_SECURE_NO_WARNINGS
  TAU_DIRECTORY="${CMAKE_CURRENT_LIST_DIR}"
  TAU_VERSION="${PROJECT_VERSION}")

include_directories(./inc)
include_directories(./src)

add_subdirectory(./src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
