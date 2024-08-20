cmake_minimum_required(VERSION 3.13.0)

project(Tau
  VERSION 0.1.0
  DESCRIPTION "A strictly typed, compiled, general purpose programming language."
  LANGUAGES C
)

# Include the CTest module and enable testing for the project.
include(CTest)
enable_testing()

# Specify the C standard to be used (C17) and enforce it as a requirement.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include LLVM configuration file.
include(LLVMConfig.cmake)

# Include Tau configuration file.
include(TauConfig.cmake)

# Add the `src` directory, which contains the source files for the project.
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

# Add the `test` directory, which contains the unit tests.
add_subdirectory(${PROJECT_SOURCE_DIR}/test)
