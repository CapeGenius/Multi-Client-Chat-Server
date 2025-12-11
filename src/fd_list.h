#ifndef FD_LIST_H
#define FD_LIST_H

typedef struct fd_node {
  int file_descriptor;
  struct fd_node *next;
} fd_node_t;

// Function declarations
fd_node_t *enqueue_fd(fd_node_t **head, int file_descriptor);
int remove_fd(fd_node_t **head, int file_descriptor);
void print_fd_list(fd_node_t *head); // optional for debugging

#endif
