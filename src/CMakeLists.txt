# Retrieve compile options and apply them to later targets.
get_property(TAU_COMPILE_OPTIONS GLOBAL PROPERTY TAU_COMPILE_OPTIONS_PROPERTY)
add_compile_options(${TAU_COMPILE_OPTIONS})

# Retrieve compile definitions and apply them to later targets.
get_property(TAU_COMPILE_DEFINITIONS GLOBAL PROPERTY TAU_COMPILE_DEFINITIONS_PROPERTY)
add_compile_definitions(${TAU_COMPILE_DEFINITIONS})

# Define static libraries for various modules/components within the project.
file(GLOB_RECURSE TAU_LIB_AST_FILES       ${PROJECT_SOURCE_DIR}/src/ast/*.c)
file(GLOB_RECURSE TAU_LIB_LEXER_FILES     ${PROJECT_SOURCE_DIR}/src/stages/lexer/*.c)
file(GLOB_RECURSE TAU_LIB_PARSER_FILES    ${PROJECT_SOURCE_DIR}/src/stages/parser/*.c)
file(GLOB_RECURSE TAU_LIB_ANALYSIS_FILES  ${PROJECT_SOURCE_DIR}/src/stages/analysis/*.c)
file(GLOB_RECURSE TAU_LIB_CODEGEN_FILES   ${PROJECT_SOURCE_DIR}/src/stages/codegen/*.c)
file(GLOB_RECURSE TAU_LIB_LINKER_FILES    ${PROJECT_SOURCE_DIR}/src/linker/*.c)

add_library(TAU_LIB_MEMTRACE    STATIC ${PROJECT_SOURCE_DIR}/src/utils/memory/memtrace.c)
add_library(TAU_LIB_ARENA       STATIC ${PROJECT_SOURCE_DIR}/src/utils/memory/arena.c)
add_library(TAU_LIB_LIST        STATIC ${PROJECT_SOURCE_DIR}/src/utils/collections/list.c)
add_library(TAU_LIB_QUEUE       STATIC ${PROJECT_SOURCE_DIR}/src/utils/collections/queue.c)
add_library(TAU_LIB_SET         STATIC ${PROJECT_SOURCE_DIR}/src/utils/collections/set.c)
add_library(TAU_LIB_STACK       STATIC ${PROJECT_SOURCE_DIR}/src/utils/collections/stack.c)
add_library(TAU_LIB_VECTOR      STATIC ${PROJECT_SOURCE_DIR}/src/utils/collections/vector.c)
add_library(TAU_LIB_CLI         STATIC ${PROJECT_SOURCE_DIR}/src/utils/io/cli.c)
add_library(TAU_LIB_COMMAND     STATIC ${PROJECT_SOURCE_DIR}/src/utils/io/command.c)
add_library(TAU_LIB_FILE        STATIC ${PROJECT_SOURCE_DIR}/src/utils/io/file.c)
add_library(TAU_LIB_LOG         STATIC ${PROJECT_SOURCE_DIR}/src/utils/io/log.c)
add_library(TAU_LIB_HASH        STATIC ${PROJECT_SOURCE_DIR}/src/utils/hash.c)
add_library(TAU_LIB_STR         STATIC ${PROJECT_SOURCE_DIR}/src/utils/str.c)
add_library(TAU_LIB_STR_VIEW    STATIC ${PROJECT_SOURCE_DIR}/src/utils/str_view.c)
add_library(TAU_LIB_TIMER       STATIC ${PROJECT_SOURCE_DIR}/src/utils/timer.c)
add_library(TAU_LIB_CRUMB       STATIC ${PROJECT_SOURCE_DIR}/src/utils/crumb.c)
add_library(TAU_LIB_DIAGNOSTICS STATIC ${PROJECT_SOURCE_DIR}/src/utils/diagnostics.c)
add_library(TAU_LIB_COMPILER    STATIC ${PROJECT_SOURCE_DIR}/src/compiler.c)
add_library(TAU_LIB_LLVM        STATIC ${PROJECT_SOURCE_DIR}/src/llvm.c)
add_library(TAU_LIB_AST         STATIC ${TAU_LIB_AST_FILES})
add_library(TAU_LIB_LEXER       STATIC ${TAU_LIB_LEXER_FILES})
add_library(TAU_LIB_PARSER      STATIC ${TAU_LIB_PARSER_FILES})
add_library(TAU_LIB_ANALYSIS    STATIC ${TAU_LIB_ANALYSIS_FILES})
add_library(TAU_LIB_CODEGEN     STATIC ${TAU_LIB_CODEGEN_FILES})
add_library(TAU_LIB_LINKER      STATIC ${TAU_LIB_LINKER_FILES})

# Combine libraries into a single variable for easier linking later.
set(TAU_LIBRARIES
  TAU_LIB_MEMTRACE
  TAU_LIB_ARENA
  TAU_LIB_LIST
  TAU_LIB_QUEUE
  TAU_LIB_SET
  TAU_LIB_STACK
  TAU_LIB_VECTOR
  TAU_LIB_CLI
  TAU_LIB_COMMAND
  TAU_LIB_FILE
  TAU_LIB_LOG
  TAU_LIB_HASH
  TAU_LIB_STR
  TAU_LIB_STR_VIEW
  TAU_LIB_TIMER
  TAU_LIB_CRUMB
  TAU_LIB_DIAGNOSTICS
  TAU_LIB_COMPILER
  TAU_LIB_LLVM
  TAU_LIB_AST
  TAU_LIB_LEXER
  TAU_LIB_PARSER
  TAU_LIB_ANALYSIS
  TAU_LIB_CODEGEN
  TAU_LIB_LINKER
)

# Store libraries in a global property for later use.
set_property(GLOBAL PROPERTY TAU_LIBRARIES_PROPERTY ${TAU_LIBRARIES})

# Define the main executable target for the compiler.
add_executable(tauc ${PROJECT_SOURCE_DIR}/src/main.c)

# Retrieve LLVM libraries and link them to the main executable.
get_property(LLVM_LIBRARIES GLOBAL PROPERTY LLVM_LIBRARIES_PROPERTY)
target_link_libraries(tauc PUBLIC ${TAU_LIBRARIES} ${LLVM_LIBRARIES})

# Install the main executable for both Debug and Release configurations.
install(TARGETS tauc CONFIGURATIONS Debug Release)
