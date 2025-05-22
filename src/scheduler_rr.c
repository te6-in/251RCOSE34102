#include "process_queue.h"
#include "schedulers.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  ProcessQueue *ready_queue;

  Process *last_run_process; // 틱 변화 시 직전에 수행한 프로세스와 같은지 확인하기 위해 필요.
                             // 다르면 time_quantum_left 초기화

  int time_quantum;
  int time_quantum_left;
} RrState;

static void rr_enqueue(Scheduler *scheduler, Process *process) {
  RrState *state = scheduler->state;

  ProcessQueue *queue = state->ready_queue;

  ProcessNode *new_node = malloc(sizeof(ProcessNode));
  new_node->process = process;
  new_node->next = NULL;

  // 큐가 비어있거나, 새로운 프로세스의 arrived_at이 맨 앞 프로세스보다 빠른 경우
  // 맨 앞에 넣기만 하면 됨
  if (!queue->head || process->arrived_at < queue->head->process->arrived_at) {
    new_node->next = queue->head;
    queue->head = new_node;

    if (!queue->tail)
      queue->tail = new_node;

    return;
  }

  ProcessNode *cur = queue->head;
  while (cur->next && process->arrived_at >= cur->next->process->arrived_at) {
    cur = cur->next;
  }

  new_node->next = cur->next;
  cur->next = new_node;

  if (!new_node->next) // 큐의 맨 뒤에 추가된 경우
    queue->tail = new_node;
}

static Process *rr_pick_next(Scheduler *scheduler) {
  RrState *state = scheduler->state;

  Process *process = NULL;

  // enqueue에서 arrived_at 순으로 정렬해서 넣어줬으므로
  // 벌크로 등록되더라도 arrived_at이 가장 작은 프로세스가 맨 앞에 위치
  if (!dequeue(state->ready_queue, &process))
    return NULL;

  state->last_run_process = process;
  state->time_quantum_left = state->time_quantum;

  return process;
}

static void rr_on_tick(Scheduler *scheduler) {
  RrState *state = scheduler->state;

  if (state->last_run_process && state->last_run_process->is_in_io == false &&
      state->time_quantum_left > 0) {
    state->time_quantum_left--;
  }
}

static bool rr_should_preempt(Scheduler *scheduler, Process *running_process) {
  RrState *state = scheduler->state;

  // 직전에 context switch한 경우
  if (running_process != state->last_run_process) {
    // 업데이트, time_quantum_left 초기화, preemption 필요 없음
    state->last_run_process = running_process;
    state->time_quantum_left = state->time_quantum;

    return false;
  }

  // slice 끝났고 기다리는 프로세스 있는 경우
  // 기다리는 프로세스가 없다면 지금 프로세스를 계속 실행해도 되므로 확인
  if (state->time_quantum_left == 0 && state->ready_queue->head) {
    // 지금 NULL이지만 직후 pick_next()에서 채워짐
    state->last_run_process = NULL;
    state->time_quantum_left = state->time_quantum;

    return true;
  }

  return false;
}

static void rr_destroy(Scheduler *scheduler) {
  free(scheduler->state);
  free(scheduler);
}

static void rr_print_state(Scheduler *scheduler) {
  RrState *state = scheduler->state;

  printf("      [레디 큐]\n");

  print_queue(state->ready_queue);
}

static int rr_get_left_process_count(Scheduler *scheduler) {
  RrState *state = scheduler->state;

  return get_queue_size(state->ready_queue);
}

static void rr_on_initialize(Scheduler *scheduler) {
  RrState *state = scheduler->state;

  if (state->time_quantum > 0)
    return;

  state->time_quantum = get_positive_int("\n  Time quantum: ");
}

Scheduler *create_rr_scheduler(int time_quantum) {
  RrState *state = calloc(1, sizeof *state);
  state->ready_queue = create_queue();

  if (time_quantum > 0) {
    state->time_quantum = time_quantum;
  }

  Scheduler *scheduler = malloc(sizeof *scheduler);
  *scheduler = (Scheduler){
      .name = "RR",

      .enqueue = rr_enqueue,
      .pick_next = rr_pick_next,
      .on_tick = rr_on_tick,
      .should_preempt = rr_should_preempt,
      .on_initialize = rr_on_initialize,
      .destroy = rr_destroy,

      .state = state,
      .print_state = rr_print_state,
      .get_left_process_count = rr_get_left_process_count,
  };

  return scheduler;
}
