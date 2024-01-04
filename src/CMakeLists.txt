if(MSVC)
  add_compile_options(/Wall /WX /FIbase.h)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror -include base.h)
endif()

set(TAU_SOURCES
  ${TAU_SOURCE_DIR}/allocator.c
  ${TAU_SOURCE_DIR}/analyzer.c
  ${TAU_SOURCE_DIR}/arena.c
  ${TAU_SOURCE_DIR}/ast.c
  ${TAU_SOURCE_DIR}/cli.c
  ${TAU_SOURCE_DIR}/compiler.c
  ${TAU_SOURCE_DIR}/crumb.c
  ${TAU_SOURCE_DIR}/diagnostics.c
  ${TAU_SOURCE_DIR}/file.c
  ${TAU_SOURCE_DIR}/generator.c
  ${TAU_SOURCE_DIR}/hash.c
  ${TAU_SOURCE_DIR}/lexer.c
  ${TAU_SOURCE_DIR}/list.c
  ${TAU_SOURCE_DIR}/location.c
  ${TAU_SOURCE_DIR}/log.c
  ${TAU_SOURCE_DIR}/memtrace.c
  ${TAU_SOURCE_DIR}/op.c
  ${TAU_SOURCE_DIR}/parser.c
  ${TAU_SOURCE_DIR}/queue.c
  ${TAU_SOURCE_DIR}/set.c
  ${TAU_SOURCE_DIR}/shyd.c
  ${TAU_SOURCE_DIR}/stack.c
  ${TAU_SOURCE_DIR}/str_view.c
  ${TAU_SOURCE_DIR}/str.c
  ${TAU_SOURCE_DIR}/symtable.c
  ${TAU_SOURCE_DIR}/timer.c
  ${TAU_SOURCE_DIR}/token.c
  ${TAU_SOURCE_DIR}/typebuilder.c
  ${TAU_SOURCE_DIR}/typedesc.c
  ${TAU_SOURCE_DIR}/typeset.c
  ${TAU_SOURCE_DIR}/typetable.c
  ${TAU_SOURCE_DIR}/vector.c
)

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
