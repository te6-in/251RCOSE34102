#include "process.h"
#include "process_queue.h"
#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  ProcessQueue *ready_queue;
} PriorityState;

static void priority_enqueue(Scheduler *scheduler, Process *process) {
  PriorityState *state = scheduler->state;

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

  ProcessNode *current = queue->head;
  while (current->next && !has_higher_priority(process, current->next->process)) {
    current = current->next;
  }

  new_node->next = current->next;
  current->next = new_node;

  if (!new_node->next) // 큐의 맨 뒤에 추가된 경우
    queue->tail = new_node;
}

static Process *priority_pick_next(Scheduler *scheduler) {
  PriorityState *state = scheduler->state;

  Process *process = NULL;

  // enqueue에서 priority 순으로 정렬해서 넣어줬으므로
  // 벌크로 등록되더라도 priority가 가장 큰 프로세스가 맨 앞에 위치
  if (!dequeue(state->ready_queue, &process))
    return NULL;

  return process;
}

static void priority_on_tick(Scheduler *_scheduler) {
  (void)_scheduler;

  // no-op
}

static bool priority_should_preempt(Scheduler *_scheduler, Process *_running_process) {
  (void)_scheduler;
  (void)_running_process;

  return false;
}

static void priority_destroy(Scheduler *scheduler) {
  free(scheduler->state);
  free(scheduler);
}

static void priority_print_state(Scheduler *scheduler) {
  PriorityState *state = scheduler->state;

  printf("      [레디 큐]\n");

  print_queue(state->ready_queue);
}

static int priority_get_left_process_count(Scheduler *scheduler) {
  PriorityState *state = scheduler->state;

  return get_queue_size(state->ready_queue);
}

Scheduler *create_priority_scheduler(void) {
  PriorityState *state = calloc(1, sizeof *state);
  state->ready_queue = create_queue();

  Scheduler *scheduler = malloc(sizeof *scheduler);
  *scheduler = (Scheduler){
      .name = "Priority",

      .enqueue = priority_enqueue,
      .pick_next = priority_pick_next,
      .on_tick = priority_on_tick,
      .should_preempt = priority_should_preempt,
      .destroy = priority_destroy,

      .state = state,
      .print_state = priority_print_state,
      .get_left_process_count = priority_get_left_process_count,
  };

  return scheduler;
}
