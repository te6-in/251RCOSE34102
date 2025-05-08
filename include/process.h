#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
  int pid;
  // int priority;
  int arrival;
  int cpu_burst;
  // int io_burst;
  // int io_request;

  int remaining_cpu_burst;
} Process;

#endif
