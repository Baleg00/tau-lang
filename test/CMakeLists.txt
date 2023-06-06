add_executable(list_test
  list_test.c

  ../src/list.c
  ../src/log.c
  ../src/memtrace.c
)

add_test(NAME list_test COMMAND list_test)

add_executable(stack_test
  stack_test.c

  ../src/stack.c
  ../src/list.c
  ../src/log.c
  ../src/memtrace.c
)

add_test(NAME stack_test COMMAND stack_test)

add_executable(queue_test
  queue_test.c

  ../src/queue.c
  ../src/list.c
  ../src/log.c
  ../src/memtrace.c
)

add_test(NAME queue_test COMMAND queue_test)
