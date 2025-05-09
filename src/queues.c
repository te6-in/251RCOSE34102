#include "queues.h"
#include "process.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static ProcessNode *ready_head = NULL;
static ProcessNode *ready_tail = NULL;

static ProcessNode *io_head = NULL;
static ProcessNode *io_tail = NULL;

void enqueue_ready(Process *process) {
  ProcessNode *node = malloc(sizeof(ProcessNode));

  if (!node) {
    perror("malloc");
    exit(1);
  }

  node->process = process;
  node->next = NULL;

  if (!ready_tail) {
    ready_head = ready_tail = node;

    return;
  }

  ready_tail->next = node;
  ready_tail = node;
}

void enqueue_io(Process *process) {
  ProcessNode *node = malloc(sizeof(ProcessNode));

  if (!node) {
    perror("malloc");
    exit(1);
  }

  node->process = process;
  node->next = NULL;

  if (!io_tail) {
    io_head = io_tail = node;
    return;
  }

  io_tail->next = node;
  io_tail = node;
}

int dequeue_ready(Process **process) {
  if (!ready_head)
    return 0;

  ProcessNode *tmp = ready_head;

  *process = tmp->process;
  ready_head = ready_head->next;

  if (!ready_head)
    ready_tail = NULL;

  free(tmp);

  return 1;
}

int dequeue_io(Process **process) {
  if (!io_head)
    return 0;

  ProcessNode *tmp = io_head;

  *process = tmp->process;
  io_head = io_head->next;

  if (!io_head)
    io_tail = NULL;

  free(tmp);
  return 1;
}

int is_ready_queue_empty(void) { return ready_head == NULL; }

int is_io_queue_empty(void) { return io_head == NULL; }

void print_ready_queue(void) {
  printf("    [대기 큐]\n");

  if (!ready_head) {
    printf("      (비어 있음)\n");

    return;
  }

  ProcessNode *cur = ready_head;

  while (cur) {
    printf("      P%d (cpu %d 남음)\n", cur->process->pid, cur->process->cpu_burst_remaining);

    cur = cur->next;
  }
}

void print_io_queue(void) {
  printf("    [I/O 큐]\n");

  if (!io_head) {
    printf("      (비어 있음)\n");
    return;
  }

  ProcessNode *cur = io_head;
  while (cur) {
    printf("      P%d (I/O %d 남음)\n", cur->process->pid, cur->process->io_burst_remaining);
    cur = cur->next;
  }
}

bool peek_io_queue(Process **process) {
  if (is_io_queue_empty())
    return false;

  *process = io_head->process;

  return true;
}
