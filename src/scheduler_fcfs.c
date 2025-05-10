#include "process_queue.h"
#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  ProcessQueue *ready_queue;
} FcfsState;

static void fcfs_enqueue(Scheduler *scheduler, Process *process) {
  FcfsState *state = scheduler->state;
  enqueue(state->ready_queue, process);
}

static Process *fcfs_pick_next(Scheduler *scheduler) {
  FcfsState *state = scheduler->state;

  Process *process = NULL;

  if (!dequeue(state->ready_queue, &process))
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

static void fcfs_print_state(Scheduler *scheduler) {
  FcfsState *state = scheduler->state;

  printf("\n    FCFS 스케줄러 상태:\n");
  printf("      [레디 큐]\n");

  print_queue(state->ready_queue);
}

Scheduler *scheduler_fcfs(void) {
  FcfsState *state = calloc(1, sizeof *state); // state.dummy = 0으로 초기화
  state->ready_queue = create_queue();

  Scheduler *sched = malloc(sizeof *sched);
  *sched = (Scheduler){
      .name = "FCFS",

      .enqueue = fcfs_enqueue,
      .pick_next = fcfs_pick_next,
      .on_tick = fcfs_on_tick,
      .destroy = fcfs_destroy,

      .state = state,
      .print_state = fcfs_print_state,
  };

  return sched;
}
