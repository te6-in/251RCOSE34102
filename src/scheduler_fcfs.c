#include "queues.h"
#include "schedulers.h"
#include <stdlib.h>

typedef struct {
  int dummy;
} FcfsState;

static void fcfs_enqueue(Scheduler *_scheduler, Process *process) {
  (void)_scheduler;
  enqueue_ready(process);
}

static Process *fcfs_pick_next(Scheduler *_scheduler) {
  (void)_scheduler;

  Process *process = NULL;

  if (!dequeue_ready(&process))
    return NULL;

  return process;
}

static void fcfs_on_tick(Scheduler *_scheduler) {
  (void)_scheduler;

  // no-op
}

static void fcfs_destroy(Scheduler *scheduler) {
  free(scheduler->state);
  free(scheduler);
}

Scheduler *scheduler_fcfs(void) {
  FcfsState *state = calloc(1, sizeof *state); // state.dummy = 0으로 초기화

  Scheduler *sched = malloc(sizeof *sched);
  *sched = (Scheduler){
      .name = "FCFS",

      .enqueue = fcfs_enqueue,
      .pick_next = fcfs_pick_next,
      .on_tick = fcfs_on_tick,
      .destroy = fcfs_destroy,

      .state = state,
  };

  return sched;
}
