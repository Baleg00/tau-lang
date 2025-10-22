macro(tau_find_llvm)
  find_package(LLVM 18.1.0 REQUIRED CONFIG)

  message(VERBOSE "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(VERBOSE "Using LLVMConfig.cmake in: ${LLVM_DIR}")
  message(VERBOSE "LLVM Include Directories: ${LLVM_INCLUDE_DIRS}")
  message(VERBOSE "LLVM Library Directories: ${LLVM_LIBRARY_DIRS}")
  message(VERBOSE "LLVM Definitions: ${LLVM_DEFINITIONS}")
endmacro()

function(tau_target_llvm_configure TARGET LLVM_SHARED)
  target_include_directories(${TARGET} PRIVATE ${LLVM_INCLUDE_DIRS})
  target_link_directories(${TARGET} PRIVATE ${LLVM_LIBRARY_DIRS})
  target_compile_definitions(${TARGET} PRIVATE ${LLVM_DEFINITIONS})

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

  if (LLVM_SHARED)
    llvm_config(${TARGET} USE_SHARED ${TAU_LLVM_COMPONENTS})
  else ()
    llvm_config(${TARGET} ${TAU_LLVM_COMPONENTS})
  endif ()
endfunction()
