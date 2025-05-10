#include "process_queue.h"
#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  ProcessQueue *ready_queue;
} FcfsState;

static void fcfs_enqueue(Scheduler *scheduler, Process *process) {
  FcfsState *state = scheduler->state;

  ProcessQueue *queue = state->ready_queue;

  ProcessNode *new_node = malloc(sizeof(ProcessNode));
  new_node->process = process;
  new_node->next = NULL;

  // 큐가 비어있거나, 새로운 프로세스의 arrived_at이 맨 앞 프로세스보다 빠른 경우
  // 맨 앞에 넣기만 하면 됨
  if (!queue->head || process->arrived_at < queue->head->process->arrived_at) {
    new_node->next = queue->head;
    queue->head = new_node;

    if (!queue->tail) // 큐가 비어있었던 경우
      queue->tail = new_node;

    return;
  }

  ProcessNode *current = queue->head;
  while (current->next && current->next->process->arrived_at <= process->arrived_at) {
    current = current->next;
  }

  new_node->next = current->next;
  current->next = new_node;

  if (!new_node->next) // 큐의 맨 뒤에 추가된 경우
    queue->tail = new_node;
}

static Process *fcfs_pick_next(Scheduler *scheduler) {
  FcfsState *state = scheduler->state;

  Process *process = NULL;

  // enqueue에서 arrived_at 순으로 정렬해서 넣어줬으므로
  // 벌크로 등록되더라도 arrived_at이 가장 작은 프로세스가 맨 앞에 위치
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

  printf("      [레디 큐]\n");

  print_queue(state->ready_queue);
}

static int fcfs_get_left_process_count(Scheduler *scheduler) {
  FcfsState *state = scheduler->state;

  return get_queue_size(state->ready_queue);
}

Scheduler *create_fcfs_scheduler(void) {
  FcfsState *state = calloc(1, sizeof *state); // state.dummy = 0으로 초기화
  state->ready_queue = create_queue();

  Scheduler *scheduler = malloc(sizeof *scheduler);
  *scheduler = (Scheduler){
      .name = "FCFS",

      .enqueue = fcfs_enqueue,
      .pick_next = fcfs_pick_next,
      .on_tick = fcfs_on_tick,
      .destroy = fcfs_destroy,

      .state = state,
      .print_state = fcfs_print_state,

      .get_left_process_count = fcfs_get_left_process_count,
  };

  return scheduler;
}
