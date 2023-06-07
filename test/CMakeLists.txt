set(TAU_TEST_SOURCES
  ${TAU_TEST_DIR}/list_test.c
  ${TAU_TEST_DIR}/stack_test.c
  ${TAU_TEST_DIR}/queue_test.c
  ${TAU_TEST_DIR}/arena_test.c
  ${TAU_TEST_DIR}/lexer_test.c
)

foreach(TEST_SOURCE IN ITEMS ${TAU_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

  add_executable(${TEST_NAME} ${TEST_SOURCE})
  target_link_libraries(${TEST_NAME} PUBLIC ${TAU_LIBRARY_NAME})
  
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
