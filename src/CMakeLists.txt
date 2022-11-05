add_executable(${PROJECT_NAME} 
  main.c
  
  location.c
  token.c
  lexer.c
  op.c
  ast.c
  shyd.c
  parser.c

  file.c
  cli.c
  log.c
  crumb.c

  list.c
  stack.c
  queue.c)
  
add_compile_options(
  $<$<C_COMPILER_ID:MSVC>:/Wall /WX>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)
