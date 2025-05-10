#include "process_queue.h"
#include "process.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ProcessNode {
  Process *process;
  struct ProcessNode *next;
} ProcessNode;

typedef struct ProcessQueue {
  ProcessNode *head;
  ProcessNode *tail;
} ProcessQueue;

ProcessQueue *create_queue(void) {
  return calloc(1, sizeof(ProcessQueue)); // head와 tail을 0(NULL)으로 초기화한 queue 반환
}

void destroy_queue(ProcessQueue *queue) {
  ProcessNode *current = queue->head;
  ProcessNode *next;

  while (current) {
    next = current->next;
    free(current);

    current = next;
  }

  free(queue);
}

void enqueue(ProcessQueue *queue, Process *process) {
  ProcessNode *node = malloc(sizeof(ProcessNode));

  if (!node) {
    perror("malloc");
    exit(1);
  }

  node->process = process;
  node->next = NULL;

  if (!queue->tail) {
    queue->head = queue->tail = node;

    return;
  }

  queue->tail->next = node;
  queue->tail = node;
}

bool dequeue(ProcessQueue *queue, Process **process) {
  if (!queue->head)
    return false;

  ProcessNode *tmp = queue->head;

  *process = tmp->process;
  queue->head = queue->head->next;

  if (!queue->head)
    queue->tail = NULL;

  free(tmp);

  return true;
}

bool is_empty(ProcessQueue *queue) { return queue->head == NULL; }

bool peek(ProcessQueue *queue, Process **process) {
  if (is_empty(queue))
    return false;

  *process = queue->head->process;

  return true;
}

void print_queue(ProcessQueue *queue) {
  if (is_empty(queue)) {
    printf("      (비어 있음)\n");

    return;
  }

  ProcessNode *current = queue->head;

  while (current) {
    printf("      P%d\n", current->process->pid);
    current = current->next;
  }
}
