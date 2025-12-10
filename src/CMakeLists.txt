add_library(chat_server
  client_helpers.c
  client_helpers.h
  fd_list.c
  fd_list.h
  logger.c
  logger.h
  router.c
  server_setup.c
  server_setup.h
)

add_executable(server_run server.c)
target_link_libraries(server_run chat_server)

add_executable(client_run client.c)
target_link_libraries(client_run chat_server)
