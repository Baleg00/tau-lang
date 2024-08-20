# Find the LLVM package and ensure that it is found.
# This is required for building the project (using CONFIG mode to locate the package).
find_package(LLVM 18.1.0 REQUIRED CONFIG)

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

# Map LLVM components to corresponding library names and store them in the _LLVM_LIBRARIES variable.
llvm_map_components_to_libnames(_LLVM_LIBRARIES
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

# Store the list of LLVM libraries in a global property for later use.
set_property(GLOBAL PROPERTY LLVM_LIBRARIES_PROPERTY ${_LLVM_LIBRARIES})
message(VERBOSE "LLVM Libraries: ${_LLVM_LIBRARIES}")
