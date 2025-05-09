#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

typedef struct {
  int pid;

  // int priority;

  int arrival;

  int cpu_burst;
  int cpu_burst_remaining;

  int io_burst;
  int io_burst_remaining;
  int io_request_time;
  bool is_in_io;
} Process;

extern const Process NULL_PROCESS;

#endif
