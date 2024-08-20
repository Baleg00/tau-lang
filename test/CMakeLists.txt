# Retrieve compile options and apply them to later targets.
get_property(TAU_COMPILE_OPTIONS GLOBAL PROPERTY TAU_COMPILE_OPTIONS_PROPERTY)
add_compile_options(${TAU_COMPILE_OPTIONS})

# Retrieve compile definitions and apply them to later targets.
get_property(TAU_COMPILE_DEFINITIONS GLOBAL PROPERTY TAU_COMPILE_DEFINITIONS_PROPERTY)
add_compile_definitions(TAU_TEST ${TAU_COMPILE_DEFINITIONS})

# Retrieve Tau and LLVM libraries and link them to later targets.
get_property(TAU_LIBRARIES GLOBAL PROPERTY TAU_LIBRARIES_PROPERTY)
get_property(LLVM_LIBRARIES GLOBAL PROPERTY LLVM_LIBRARIES_PROPERTY)
link_libraries(${TAU_LIBRARIES} ${LLVM_LIBRARIES})

# Define test names corresponding to modules/components to be tested.
set(TAU_TEST_NAMES
  ARENA
  HASH
  LIST
  QUEUE
  SET
  STACK
  STR
  STR_VIEW
  TYPEDESC
  VECTOR
  LEXER
)

# Iterate over each name in `TAU_TEST_NAMES` to create an executable and a corresponding test.
foreach(NAME IN ITEMS ${TAU_TEST_NAMES})
  string(TOLOWER ${NAME} NAME_LOWER)
  set(TARGET TAU_TEST_${NAME})

  # Define an executable target, excluding it from the default build target and register it as a test.
  # The source file for each test is assumed to be located in the `test` directory.
  add_executable(${TARGET} ${PROJECT_SOURCE_DIR}/test/${NAME_LOWER}_test.c)
  add_test(NAME ${TARGET} COMMAND ${TARGET})
endforeach()
