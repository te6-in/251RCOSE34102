#include "queue.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>

static ProcessNode *head = NULL;
static ProcessNode *tail = NULL;

void enqueue(Process p) {
  ProcessNode *node = malloc(sizeof(ProcessNode));

  if (!node) {
    perror("malloc");
    exit(1);
  }

  node->p = p;
  node->next = NULL;

  if (!tail) {
    head = tail = node;

    return;
  }

  tail->next = node;
  tail = node;
}

int dequeue(Process *out) {
  if (!head)
    return 0;

  ProcessNode *tmp = head;

  *out = tmp->p;
  head = head->next;

  if (!head)
    tail = NULL;

  free(tmp);

  return 1;
}

int is_queue_empty(void) { return head == NULL; }

void print_queue(void) {
  printf("    [대기 큐] ");

  if (!head) {
    printf("(비어 있음)\n");

    return;
  }

  ProcessNode *cur = head;

  while (cur) {
    printf("P%d (%d 남음) ", cur->p.pid, cur->p.remaining_cpu_burst);

    cur = cur->next;
  }

  printf("\n");
}
