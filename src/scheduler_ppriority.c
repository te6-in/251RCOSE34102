#include "process.h"
#include "process_queue.h"
#include "schedulers.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  ProcessQueue *ready_queue;
} PPriorityState;

static void ppriority_enqueue(Scheduler *scheduler, Process *process) {
  PPriorityState *state = scheduler->state;

  ProcessQueue *queue = state->ready_queue;

  ProcessNode *new_node = malloc(sizeof(ProcessNode));
  new_node->process = process;
  new_node->next = NULL;

  // 큐가 비어있거나, 새로운 프로세스의 priority가 맨 앞 프로세스보다 큰 경우
  // 맨 앞에 넣기만 하면 됨
  if (!queue->head || has_higher_priority(process, queue->head->process)) {
    new_node->next = queue->head;
    queue->head = new_node;

    if (!queue->tail)
      queue->tail = new_node;

    return;
  }

  ProcessNode *cur = queue->head;
  while (cur->next && !has_higher_priority(process, cur->next->process)) {
    cur = cur->next;
  }

  new_node->next = cur->next;
  cur->next = new_node;

  if (!new_node->next) // 큐의 맨 뒤에 추가된 경우
    queue->tail = new_node;
}

static Process *ppriority_pick_next(Scheduler *scheduler) {
  PPriorityState *state = scheduler->state;

  Process *process = NULL;

  // enqueue에서 priority 순으로 정렬해서 넣어줬으므로
  // 벌크로 등록되더라도 priority가 가장 큰 프로세스가 맨 앞에 위치
  if (!dequeue(state->ready_queue, &process))
    return NULL;

  return process;
}

static void ppriority_on_tick(Scheduler *_scheduler) {
  (void)_scheduler;

  // no-op
}

static bool ppriority_should_preempt(Scheduler *scheduler, Process *running_process) {
  PPriorityState *state = scheduler->state;

  if (!state->ready_queue->head)
    return false;

  Process *front = state->ready_queue->head->process;
  return has_higher_priority(front, running_process);
}

static void ppriority_destroy(Scheduler *scheduler) {
  free(scheduler->state);
  free(scheduler);
}

static void ppriority_print_state(Scheduler *scheduler) {
  PPriorityState *state = scheduler->state;

  printf("      [레디 큐]\n");

  print_queue(state->ready_queue);
}

static int ppriority_get_left_process_count(Scheduler *scheduler) {
  PPriorityState *state = scheduler->state;

  return get_queue_size(state->ready_queue);
}

Scheduler *create_ppriority_scheduler(void) {
  PPriorityState *state = calloc(1, sizeof *state);
  state->ready_queue = create_queue();

  Scheduler *scheduler = malloc(sizeof *scheduler);
  *scheduler = (Scheduler){
      .name = "Preemptive Priority",

      .enqueue = ppriority_enqueue,
      .pick_next = ppriority_pick_next,
      .on_tick = ppriority_on_tick,
      .should_preempt = ppriority_should_preempt,
      .destroy = ppriority_destroy,

      .state = state,
      .print_state = ppriority_print_state,
      .get_left_process_count = ppriority_get_left_process_count,
  };

  return scheduler;
}
