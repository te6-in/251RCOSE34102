#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct ProcessNode {
  Process process;
  struct ProcessNode *next;
} ProcessNode;

void enqueue(Process process);
int dequeue(Process *process);
int is_queue_empty(void);
void print_queue(void);

#endif
