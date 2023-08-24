if(MSVC)
  add_compile_options(/Wall /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

set(TAU_SOURCES
  ${TAU_SOURCE_DIR}/allocator.c
  ${TAU_SOURCE_DIR}/analzyer.c
  ${TAU_SOURCE_DIR}/arena.c
  ${TAU_SOURCE_DIR}/ast.c
  ${TAU_SOURCE_DIR}/cli.c
  ${TAU_SOURCE_DIR}/compiler.c
  ${TAU_SOURCE_DIR}/crumb.c
  ${TAU_SOURCE_DIR}/diagnostics.c
  ${TAU_SOURCE_DIR}/file.c
  ${TAU_SOURCE_DIR}/hash.c
  ${TAU_SOURCE_DIR}/lexer.c
  ${TAU_SOURCE_DIR}/list.c
  ${TAU_SOURCE_DIR}/location.c
  ${TAU_SOURCE_DIR}/log.c
  ${TAU_SOURCE_DIR}/memtrace.c
  ${TAU_SOURCE_DIR}/op.c
  ${TAU_SOURCE_DIR}/opcode.c
  ${TAU_SOURCE_DIR}/parser.c
  ${TAU_SOURCE_DIR}/queue.c
  ${TAU_SOURCE_DIR}/register.c
  ${TAU_SOURCE_DIR}/set.c
  ${TAU_SOURCE_DIR}/shyd.c
  ${TAU_SOURCE_DIR}/stack.c
  ${TAU_SOURCE_DIR}/str.c
  ${TAU_SOURCE_DIR}/symtable.c
  ${TAU_SOURCE_DIR}/tasm.c
  ${TAU_SOURCE_DIR}/timer.c
  ${TAU_SOURCE_DIR}/token.c
  ${TAU_SOURCE_DIR}/typedesc.c
  ${TAU_SOURCE_DIR}/typetable.c
  ${TAU_SOURCE_DIR}/vector.c
  ${TAU_SOURCE_DIR}/vm.c
)

add_library(${TAU_LIBRARY_NAME} STATIC ${TAU_SOURCES})

add_executable(${TAU_EXECUTABLE_NAME} main.c)
target_link_libraries(${TAU_EXECUTABLE_NAME} PUBLIC ${TAU_LIBRARY_NAME})
