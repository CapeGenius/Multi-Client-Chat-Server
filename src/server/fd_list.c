#include "fd_list.h" // exposes fd_node_t and function prototypes
#include <stdio.h>     // printf for debugging helpers

fd_node_t *enqueue_fd(fd_node_t **head, int fd) {
  // Append a new descriptor node to the tail of the list
  fd_node_t *new_node = malloc(sizeof(fd_node_t)); // allocate node on heap
  if (!new_node)
    return NULL; // allocation failed, report to caller
  new_node->fd = fd;       // stash the descriptor value
  new_node->next = NULL;   // new node initially has no successor

  if (*head == NULL) {
    *head = new_node;      // empty list: new node becomes head
  } else {
    fd_node_t *curr = *head; // start at head and walk to tail
    while (curr->next)
      curr = curr->next;   // advance until the last node
    curr->next = new_node; // link tail to the newly allocated node
  }
  return new_node;         // return pointer for optional chaining/inspection
}

int dequeue_fd(fd_node_t **head) {
  // Remove the head node and return its descriptor
  //remove the first node, update the head, free the old node, and return its fd.
  if (*head == NULL)
    return -1;             // nothing to dequeue
  fd_node_t *temp = *head; // remember current head
  int fd = temp->fd;       // capture descriptor value
  *head = temp->next;      // advance head pointer to next node
  free(temp);              // release old head
  return fd;               // hand descriptor back to caller
}

fd_node_t *find_fd(fd_node_t *head, int fd) {
  // Linear search for a descriptor node with matching fd
  while (head) {
    if (head->fd == fd)
      return head;    // found exact descriptor, return node pointer
    head = head->next; // otherwise continue traversing
  }
  return NULL;        // descriptor not present in list
}

int remove_fd(fd_node_t **head, int fd) {
  // Remove a specific descriptor from the list irrespective of position
  fd_node_t *curr = *head, *prev = NULL; // track current node and its predecessor
  while (curr) {
    if (curr->fd == fd) {
      if (prev)
        prev->next = curr->next; // bypass current node for middle/end removal
      else //current is the head
        *head = curr->next;      // removing head: update head pointer directly
      free(curr);                // release removed node memory
      return 1;                  // removed successfully
    }
    prev = curr;                // advance both pointers for traversal
    curr = curr->next;
  }
  return 0;                     // not found
}

void print_fd_list(fd_node_t *head) {
  // Debug helper to dump the descriptor list contents
  printf("FD List: ");      // prefix output with label
  while (head) {
    printf("%d ", head->fd); // print each descriptor separated by spaces
    head = head->next;       // walk through the list
  }
  printf("\n");           // terminate line for readability
}
