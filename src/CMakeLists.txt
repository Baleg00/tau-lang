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

file(GLOB_RECURSE TAU_SOURCES ${TAU_SOURCE_DIR}/*.c)

add_library(${TAU_LIBRARY_NAME} STATIC ${TAU_SOURCES})

add_executable(${TAU_EXECUTABLE_NAME} ${TAU_SOURCE_DIR}/main.c)

target_link_libraries(${TAU_EXECUTABLE_NAME} PUBLIC ${LLVM_LIBS} ${TAU_LIBRARY_NAME})

if(MSVC)
  target_compile_options(${TAU_LIBRARY_NAME} PUBLIC /Wall /WX /FIbase.h)
  target_compile_options(${TAU_EXECUTABLE_NAME} PUBLIC /Wall /WX /FIbase.h)
else()
  target_compile_options(${TAU_LIBRARY_NAME} PUBLIC -Wall -Wextra -Wpedantic -Werror -include base.h)
  target_compile_options(${TAU_EXECUTABLE_NAME} PUBLIC -Wall -Wextra -Wpedantic -Werror -include base.h)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  target_compile_definitions(${TAU_LIBRARY_NAME} PUBLIC TAU_DEBUG _CRT_SECURE_NO_WARNINGS)
  target_compile_definitions(${TAU_EXECUTABLE_NAME} PUBLIC TAU_DEBUG _CRT_SECURE_NO_WARNINGS)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  target_compile_definitions(${TAU_LIBRARY_NAME} PUBLIC TAU_RELEASE)
  target_compile_definitions(${TAU_EXECUTABLE_NAME} PUBLIC TAU_RELEASE)
endif()

target_compile_definitions(${TAU_LIBRARY_NAME} PUBLIC TAU_VERSION="${PROJECT_VERSION}")
target_compile_definitions(${TAU_EXECUTABLE_NAME} PUBLIC TAU_VERSION="${PROJECT_VERSION}")

get_target_property(TAU_LINK_LIBRARIES ${TAU_EXECUTABLE_NAME} LINK_LIBRARIES)
get_target_property(TAU_COMPILE_OPTIONS ${TAU_EXECUTABLE_NAME} COMPILE_OPTIONS)
get_target_property(TAU_COMPILE_DEFINITIONS ${TAU_EXECUTABLE_NAME} COMPILE_DEFINITIONS)

message(STATUS "Tau Link Libraries: ${TAU_LINK_LIBRARIES}")
message(STATUS "Tau Compile Options: ${TAU_COMPILE_OPTIONS}")
message(STATUS "Tau Compile Definitions: ${TAU_COMPILE_DEFINITIONS}")

install(TARGETS ${TAU_EXECUTABLE_NAME} CONFIGURATIONS "Release")
