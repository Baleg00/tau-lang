# Retrieve compile options and apply them to later targets.
get_property(TAU_COMPILE_OPTIONS GLOBAL PROPERTY TAU_COMPILE_OPTIONS_PROPERTY)
add_compile_options(${TAU_COMPILE_OPTIONS})

# Retrieve compile definitions and apply them to later targets.
get_property(TAU_COMPILE_DEFINITIONS GLOBAL PROPERTY TAU_COMPILE_DEFINITIONS_PROPERTY)
add_compile_definitions(TAU_TEST ${TAU_COMPILE_DEFINITIONS})

# Link Tau static library to later targets.
link_libraries(TAU_LIB)

file(GLOB_RECURSE TAU_TEST_SOURCE_FILES  ${PROJECT_SOURCE_DIR}/test/*.c)

# Retrieve required LLVM components.
get_property(TAU_LLVM_COMPONENTS GLOBAL PROPERTY TAU_LLVM_COMPONENTS_PROPERTY)

# Iterate over each source file to create an executable and a corresponding test.
foreach(TEST_FILE IN ITEMS ${TAU_TEST_SOURCE_FILES})
  get_filename_component(TARGET ${TEST_FILE} NAME_WE)

  # Define an executable target, excluding it from the default build target and register it as a test.
  add_executable(${TARGET} ${TEST_FILE})

  if(TAU_LLVM_SHARED)
    llvm_config(${TARGET} USE_SHARED ${TAU_LLVM_COMPONENTS})
  else()
    llvm_config(${TARGET} ${TAU_LLVM_COMPONENTS})
  endif()

  add_test(NAME ${TARGET} COMMAND ${TARGET})
endforeach()
