#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <stdbool.h>

typedef struct ProcessQueue ProcessQueue;

ProcessQueue *create_queue(void);
void destroy_queue(ProcessQueue *queue);
void enqueue(ProcessQueue *queue, Process *process);
bool dequeue(ProcessQueue *queue, Process **process);
bool peek(ProcessQueue *queue, Process **process);
void print_queue(ProcessQueue *queue);

#endif
