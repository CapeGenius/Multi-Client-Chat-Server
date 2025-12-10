#ifndef FD_LIST_H
#define FD_LIST_H

#include <stdlib.h>

typedef struct fd_node {
  int fd;
  struct fd_node *next;
} fd_node_t;

// Function declarations
fd_node_t *enqueue_fd(fd_node_t **head, int fd);
int remove_fd(fd_node_t **head, int fd);
void print_fd_list(fd_node_t *head); // optional for debugging

#endif
