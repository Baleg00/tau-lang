cmake_minimum_required(VERSION 3.13.0)

project(Tau
  VERSION 0.1.0
  DESCRIPTION "A strictly typed, compiled, general purpose programming language."
  LANGUAGES C CXX
)

option(TAU_LLVM_SHARED "Link to LLVM shared library" OFF)
option(TAU_CODE_COVERAGE "Enable code coverage support" OFF)
set(TAU_CODE_COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage CACHE STRING "Code coverage ouput directory")

include(CTest)
enable_testing()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(cmake/CodeCoverageConfig.cmake)
include(cmake/LLVMConfig.cmake)
include(cmake/OSDetectConfig.cmake)
include(cmake/TauConfig.cmake)
include(cmake/TestConfig.cmake)

tau_os_detect_configure_file()

tau_find_llvm()

tau_add_library(tau)
tau_target_configure(tau)
tau_target_llvm_configure(tau ${TAU_LLVM_SHARED})

if (TAU_CODE_COVERAGE)
  tau_target_code_coverage_configure(tau)
endif ()

tau_add_tests(TAU_TEST_TARGET_LIST)

foreach (TEST_TARGET IN LISTS TAU_TEST_TARGET_LIST)
  tau_target_configure(${TEST_TARGET})
  tau_target_llvm_configure(${TEST_TARGET} ${TAU_LLVM_SHARED})
  target_link_libraries(${TEST_TARGET} PRIVATE tau)

  if (TAU_CODE_COVERAGE)
    tau_target_code_coverage_configure(${TEST_TARGET})
  endif ()
endforeach ()

tau_add_executable(tauc)
tau_target_configure(tauc)
tau_target_llvm_configure(tauc ${TAU_LLVM_SHARED})
target_link_libraries(tauc PRIVATE tau)

if (TAU_CODE_COVERAGE)
  tau_target_code_coverage_configure(tauc)
  tau_add_code_coverage_target()
endif ()

install(TARGETS tauc CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
