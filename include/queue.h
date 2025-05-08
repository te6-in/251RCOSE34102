#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct ProcessNode {
  Process p;
  struct ProcessNode *next;
} ProcessNode;

void enqueue(Process p);
int dequeue(Process *out);
int is_queue_empty(void);
void print_queue(void);

#endif
