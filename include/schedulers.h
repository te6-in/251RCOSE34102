#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include "process.h"

typedef struct Scheduler {
  const char *name;

  void (*enqueue)(struct Scheduler *, Process *);
  Process *(*pick_next)(struct Scheduler *);
  void (*on_tick)(struct Scheduler *);
  bool (*should_preempt)(struct Scheduler *, Process *);
  void (*destroy)(struct Scheduler *);

  void *state;
  void (*print_state)(struct Scheduler *);

  int (*get_left_process_count)(struct Scheduler *);
} Scheduler;

#endif
