# Retrieve compile options and apply them to later targets.
get_property(TAU_COMPILE_OPTIONS GLOBAL PROPERTY TAU_COMPILE_OPTIONS_PROPERTY)
add_compile_options(${TAU_COMPILE_OPTIONS})

# Retrieve compile definitions and apply them to later targets.
get_property(TAU_COMPILE_DEFINITIONS GLOBAL PROPERTY TAU_COMPILE_DEFINITIONS_PROPERTY)
add_compile_definitions(${TAU_COMPILE_DEFINITIONS})

# Define Tau static library to be reused by tests and other targets.
file(GLOB_RECURSE TAU_SOURCE_FILES  ${PROJECT_SOURCE_DIR}/src/*.c)
list(REMOVE_ITEM TAU_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/main.c)
set_property(GLOBAL PROPERTY TAU_SOURCE_FILES_PROPERTY ${TAU_SOURCE_FILES})
add_library(TAU_LIB STATIC ${TAU_SOURCE_FILES})

# Define the main executable target for the compiler.
add_executable(tauc ${PROJECT_SOURCE_DIR}/src/main.c)

# Link Tau static library to the main executable.
target_link_libraries(tauc PUBLIC TAU_LIB)

# Retrieve required LLVM components.
get_property(TAU_LLVM_COMPONENTS GLOBAL PROPERTY TAU_LLVM_COMPONENTS_PROPERTY)

if(TAU_LLVM_SHARED)
    llvm_config(tauc USE_SHARED ${TAU_LLVM_COMPONENTS})
else()
    llvm_config(tauc ${TAU_LLVM_COMPONENTS})
endif()

# Install the main executable for both Debug and Release configurations.
install(TARGETS tauc CONFIGURATIONS Debug Release)
