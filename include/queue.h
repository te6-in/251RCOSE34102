#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <stdbool.h>

typedef struct ProcessNode {
  Process *process;
  struct ProcessNode *next;
} ProcessNode;

void enqueue_ready(Process *process);
int dequeue_ready(Process **process);
int is_ready_queue_empty(void);
void print_ready_queue(void);

void enqueue_io(Process *process);
int dequeue_io(Process **process);
int is_io_queue_empty(void);
void print_io_queue(void);
bool peek_io_queue(Process **process);

#endif
