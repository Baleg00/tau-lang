# Find the LLVM package and ensure that it is found.
# This is required for building the project (using CONFIG mode to locate the package).
find_package(LLVM 19.1.7 REQUIRED CONFIG)

# Output details about the LLVM package for verbose builds.
message(VERBOSE "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(VERBOSE "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(VERBOSE "LLVM Include Directories: ${LLVM_INCLUDE_DIRS}")
message(VERBOSE "LLVM Library Directories: ${LLVM_LIBRARY_DIRS}")
message(VERBOSE "LLVM Definitions: ${LLVM_DEFINITIONS}")

# Add LLVM include directories, library directories and definitions.
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# List of required LLVM components.
set(TAU_LLVM_COMPONENTS
    support
    core
    irreader
    executionengine
    jitlink
    mcjit
    exegesisx86
    x86targetmca
    x86codegen
    x86disassembler
    x86asmparser
    x86desc
    x86info
    passes
)
set_property(GLOBAL PROPERTY TAU_LLVM_COMPONENTS_PROPERTY ${TAU_LLVM_COMPONENTS})

# If TAU_LLVM_SHARED is ON, LLVM shared library will be used for linking.
# Otherwise, static libraries will be used.
option(TAU_LLVM_SHARED "Link to LLVM shared library." OFF)
message(VERBOSE "Use LLVM shared library: ${TAU_LLVM_SHARED}")
