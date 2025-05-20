#include "process_queue.h"
#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  ProcessQueue *ready_queue;
} SjfState;

static int has_shorter_cpu_burst(Process *a, Process *b) {
  if (a->cpu_burst_remaining != b->cpu_burst_remaining)
    return a->cpu_burst_remaining < b->cpu_burst_remaining;

  // 같은 경우 arrived_at 참조
  return a->arrived_at < b->arrived_at;
}

static void sjf_enqueue(Scheduler *scheduler, Process *process) {
  SjfState *state = scheduler->state;

  ProcessQueue *queue = state->ready_queue;

  ProcessNode *new_node = malloc(sizeof(ProcessNode));
  new_node->process = process;
  new_node->next = NULL;

  // 큐가 비어있거나, 새로운 프로세스의 remaining burst가 맨 앞 프로세스보다 빠른 경우
  // 맨 앞에 넣기만 하면 됨
  if (!queue->head || has_shorter_cpu_burst(process, queue->head->process)) {
    new_node->next = queue->head;
    queue->head = new_node;

    if (!queue->tail)
      queue->tail = new_node;

    return;
  }

  ProcessNode *current = queue->head;
  while (current->next && !has_shorter_cpu_burst(process, current->next->process)) {
    current = current->next;
  }

  new_node->next = current->next;
  current->next = new_node;

  if (!new_node->next) // 큐의 맨 뒤에 추가된 경우
    queue->tail = new_node;
}

static Process *sjf_pick_next(Scheduler *scheduler) {
  SjfState *state = scheduler->state;

  Process *process = NULL;

  // enqueue에서 remaining burst 순으로 정렬해서 넣어줬으므로
  // 벌크로 등록되더라도 remaining burst가 가장 작은 프로세스가 맨 앞에 위치
  if (!dequeue(state->ready_queue, &process))
    return NULL;

  return process;
}

static void sjf_on_tick(Scheduler *_scheduler) {
  (void)_scheduler;

  // no-op
}

static void sjf_destroy(Scheduler *scheduler) {
  free(scheduler->state);
  free(scheduler);
}

static void sjf_print_state(Scheduler *scheduler) {
  SjfState *state = scheduler->state;

  printf("      [레디 큐]\n");

  print_queue(state->ready_queue);
}

static int sjf_get_left_process_count(Scheduler *scheduler) {
  SjfState *state = scheduler->state;

  return get_queue_size(state->ready_queue);
}

Scheduler *create_sjf_scheduler(void) {
  SjfState *state = calloc(1, sizeof *state);
  state->ready_queue = create_queue();

  Scheduler *scheduler = malloc(sizeof *scheduler);
  *scheduler = (Scheduler){
      .name = "SJF",

      .enqueue = sjf_enqueue,
      .pick_next = sjf_pick_next,
      .on_tick = sjf_on_tick,
      .destroy = sjf_destroy,

      .state = state,
      .print_state = sjf_print_state,
      .get_left_process_count = sjf_get_left_process_count,
  };

  return scheduler;
}
