#include "controller.h"
#include "history.h"
#include "logger.h"
#include "process.h"
#include "process_queue.h"
#include "scheduler_fcfs.h"
#include "schedulers.h"
#include "stats.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_process_status(Process *process) {
  if (!process) {
    logger(LOG_INFO, "현재 실행 중인 프로세스 없음\n");

    return;
  }

  // TODO: usage 필요
  if (process->is_in_io) {
    printf("    [I/O 중] P%d (%d 남음)\n", process->pid, process->io_burst_remaining);

    return;
  }

  // TODO: usage 필요
  if (process->cpu_burst_remaining > 0) {
    printf("    [실행 중] P%d (%d 남음)\n", process->pid, process->cpu_burst_remaining);

    return;
  }
}

void end_simulator(int current_time, Scheduler *scheduler, ProcessQueue *io_queue) {
  logger(LOG_INFO, "시뮬레이터 종료 시각: %d\n", current_time);

  print_stats();

  scheduler->destroy(scheduler);
  destroy_queue(io_queue);

  exit(0);
}

void add_process(Scheduler *scheduler, int *pid_counter, int current_time) {
  int cpu_burst = get_nonnegative_int("    CPU burst (1 이상): ");

  int io_burst = get_nonnegative_int("    I/O burst (0 이상): ");

  int io_request_time = -1;
  while (io_burst > 0) {
    io_request_time = get_nonnegative_int("    I/O request time (0 이상 CPU burst 이하): ");

    if (io_request_time <= cpu_burst)
      break;

    logger(LOG_ERROR, "I/O request time은 CPU burst(%d)보다 작거나 같아야 합니다.\n", cpu_burst);
  }

  int priority = strcmp(scheduler->name, "Priority") == 0
                     ? get_positive_int("    Priority (1 이상, 클수록 높은 priority): ")
                     : -1;

  Process *new_process = malloc(sizeof(Process));

  *new_process = (Process){
      .pid = (*pid_counter)++,
      .arrived_at = current_time,
      .priority = priority,

      .cpu_burst = cpu_burst,
      .cpu_burst_remaining = cpu_burst,

      .io_burst = io_burst,
      .io_burst_remaining = io_burst,
      .io_request_time = io_request_time,
      .is_in_io = false,

      .started_at = -1,
      .last_ready_enqueued_at = current_time,
      .waiting = 0,
  };

  scheduler->enqueue(scheduler, new_process);
}
