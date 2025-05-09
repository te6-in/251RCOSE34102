#include "queue.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>

static ProcessNode *head = NULL;
static ProcessNode *tail = NULL;

void enqueue(Process process) {
  ProcessNode *node = malloc(sizeof(ProcessNode));

  if (!node) {
    perror("malloc");
    exit(1);
  }

  node->process = process;
  node->next = NULL;

  if (!tail) {
    head = tail = node;

    return;
  }

  tail->next = node;
  tail = node;
}

int dequeue(Process *process) {
  if (!head)
    return 0;

  ProcessNode *tmp = head;

  *process = tmp->process;
  head = head->next;

  if (!head)
    tail = NULL;

  free(tmp);

  return 1;
}

int is_queue_empty(void) { return head == NULL; }

void print_queue(void) {
  printf("    [대기 큐]\n");

  if (!head) {
    printf("      (비어 있음)\n");

    return;
  }

  ProcessNode *cur = head;

  while (cur) {
    printf("      P%d (%d 남음)\n", cur->process.pid, cur->process.remaining_cpu_burst);

    cur = cur->next;
  }
}
