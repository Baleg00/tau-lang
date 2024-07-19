cmake_minimum_required(VERSION 3.13.0)

project(tau
  VERSION 0.1.0
  DESCRIPTION "A strictly typed, compiled, general purpose programming language."
  LANGUAGES C)

find_package(LLVM 18.1.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM Include Directories: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM Library Directories: ${LLVM_LIBRARY_DIRS}")
message(STATUS "LLVM Definitions: ${LLVM_DEFINITIONS}")

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include(CTest)
enable_testing()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(TAU_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(TAU_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/inc)
set(TAU_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)

set(TAU_EXECUTABLE_NAME tauc)
set(TAU_LIBRARY_NAME tau_lib)

include_directories(${TAU_SOURCE_DIR} ${TAU_INCLUDE_DIR})

add_subdirectory(${TAU_SOURCE_DIR})
add_subdirectory(${TAU_TEST_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
