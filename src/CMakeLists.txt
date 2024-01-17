if(MSVC)
  add_compile_options(/Wall /WX /FIbase.h)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror -include base.h)
endif()

file(GLOB_RECURSE TAU_SOURCES ${TAU_SOURCE_DIR}/*.c)

add_library(${TAU_LIBRARY_NAME} STATIC ${TAU_SOURCES})

llvm_map_components_to_libnames(LLVM_LIBS
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

add_executable(${TAU_EXECUTABLE_NAME} main.c)

target_link_libraries(${TAU_EXECUTABLE_NAME} PUBLIC ${LLVM_LIBS} ${TAU_LIBRARY_NAME})
