#include "fd_list.h"
#include <stdio.h>

fd_node_t *enqueue_fd(fd_node_t **head, int fd) {
  fd_node_t *new_node = malloc(sizeof(fd_node_t));
  if (!new_node)
    return NULL;
  new_node->fd = fd;
  new_node->next = NULL;

  if (*head == NULL) {
    *head = new_node;
  } else {
    fd_node_t *curr = *head;
    while (curr->next)
      curr = curr->next;
    curr->next = new_node;
  }
  return new_node;
}

int dequeue_fd(fd_node_t **head) {
  if (*head == NULL)
    return -1;
  fd_node_t *temp = *head;
  int fd = temp->fd;
  *head = temp->next;
  free(temp);
  return fd;
}

fd_node_t *find_fd(fd_node_t *head, int fd) {
  while (head) {
    if (head->fd == fd)
      return head;
    head = head->next;
  }
  return NULL;
}

int remove_fd(fd_node_t **head, int fd) {
  fd_node_t *curr = *head, *prev = NULL;
  while (curr) {
    if (curr->fd == fd) {
      if (prev)
        prev->next = curr->next;
      else
        *head = curr->next;
      free(curr);
      return 1; // removed successfully
    }
    prev = curr;
    curr = curr->next;
  }
  return 0; // not found
}

void print_fd_list(fd_node_t *head) {
  printf("FD List: ");
  while (head) {
    printf("%d ", head->fd);
    head = head->next;
  }
  printf("\n");
}
