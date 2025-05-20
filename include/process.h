#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

typedef struct {
  // meta
  int pid;
  int arrived_at;
  int priority;

  // cpu
  int cpu_burst, cpu_burst_remaining;

  // io
  int io_burst, io_burst_remaining, io_request_time;
  bool is_in_io;

  // stats
  int started_at;
  int last_ready_enqueued_at;
  int waiting;
} Process;

extern const Process NULL_PROCESS;

bool has_shorter_cpu_burst_remaining(Process *a, Process *b);

#endif
