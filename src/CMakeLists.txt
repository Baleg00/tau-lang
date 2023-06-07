if(MSVC)
  add_compile_options(/Wall /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(${TAU_LIBRARY_NAME} STATIC ${TAU_SOURCES})

add_executable(${TAU_EXECUTABLE_NAME} main.c)
target_link_libraries(${TAU_EXECUTABLE_NAME} PUBLIC ${TAU_LIBRARY_NAME})
