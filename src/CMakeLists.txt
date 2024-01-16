if(MSVC)
  add_compile_options(/Wall /WX /FIbase.h)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror -include base.h)
endif()

set(TAU_SOURCES
  ${TAU_SOURCE_DIR}/compiler.c
  ${TAU_SOURCE_DIR}/main.c
  ${TAU_SOURCE_DIR}/stages/analyzer/analyzer.c
  ${TAU_SOURCE_DIR}/stages/analyzer/symtable.c
  ${TAU_SOURCE_DIR}/stages/analyzer/typebuilder.c
  ${TAU_SOURCE_DIR}/stages/analyzer/typedesc.c
  ${TAU_SOURCE_DIR}/stages/analyzer/typetable.c
  ${TAU_SOURCE_DIR}/stages/codegen/generator.c
  ${TAU_SOURCE_DIR}/stages/lexer/lexer.c
  ${TAU_SOURCE_DIR}/stages/lexer/location.c
  ${TAU_SOURCE_DIR}/stages/lexer/token.c
  ${TAU_SOURCE_DIR}/stages/parser/ast/ast.c
  ${TAU_SOURCE_DIR}/stages/parser/op.c
  ${TAU_SOURCE_DIR}/stages/parser/parser.c
  ${TAU_SOURCE_DIR}/stages/parser/shyd.c
  ${TAU_SOURCE_DIR}/utils/collections/list.c
  ${TAU_SOURCE_DIR}/utils/collections/queue.c
  ${TAU_SOURCE_DIR}/utils/collections/set.c
  ${TAU_SOURCE_DIR}/utils/collections/stack.c
  ${TAU_SOURCE_DIR}/utils/collections/vector.c
  ${TAU_SOURCE_DIR}/utils/crumb.c
  ${TAU_SOURCE_DIR}/utils/diagnostics.c
  ${TAU_SOURCE_DIR}/utils/hash.c
  ${TAU_SOURCE_DIR}/utils/io/cli.c
  ${TAU_SOURCE_DIR}/utils/io/file.c
  ${TAU_SOURCE_DIR}/utils/io/log.c
  ${TAU_SOURCE_DIR}/utils/memory/allocator.c
  ${TAU_SOURCE_DIR}/utils/memory/arena.c
  ${TAU_SOURCE_DIR}/utils/memory/memtrace.c
  ${TAU_SOURCE_DIR}/utils/str_view.c
  ${TAU_SOURCE_DIR}/utils/str.c
  ${TAU_SOURCE_DIR}/utils/timer.c
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
