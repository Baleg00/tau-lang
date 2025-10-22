function(tau_add_tests TEST_TARGET_LIST)
  file(GLOB_RECURSE TAU_TEST_SOURCE_FILES ${PROJECT_SOURCE_DIR}/test/*.c)

  foreach (TEST_FILE IN ITEMS ${TAU_TEST_SOURCE_FILES})
    get_filename_component(TEST_TARGET ${TEST_FILE} NAME_WE)

    add_executable(${TEST_TARGET} ${TEST_FILE})
    target_compile_definitions(${TEST_TARGET} PRIVATE TAU_TEST)
    list(APPEND TARGET_LIST ${TEST_TARGET})

    if (TAU_LLVM_SHARED)
      llvm_config(${TEST_TARGET} USE_SHARED ${TAU_LLVM_COMPONENTS})
    else ()
      llvm_config(${TEST_TARGET} ${TAU_LLVM_COMPONENTS})
    endif ()

    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
  endforeach ()

  set(${TEST_TARGET_LIST} ${TARGET_LIST} PARENT_SCOPE)
endfunction()
