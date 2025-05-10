#include "executor.h"
#include "history.h"
#include "logger.h"
#include "process.h"
#include "process_queue.h"
#include "scheduler_fcfs.h"
#include "schedulers.h"
#include "stats.h"
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int current_time = 0;
static Scheduler *scheduler = NULL;
static ProcessQueue *io_queue = NULL;

static void print_process_status(Process *process) {
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

static void end_simulator(int current_time) {
  logger(LOG_INFO, "시뮬레이터 종료 시각: %d\n", current_time);

  print_stats();

  scheduler->destroy(scheduler);
  destroy_queue(io_queue);

  exit(0);
}

static void add_processes(Scheduler *scheduler, int *pid_counter, int current_time) {
  while (1) {
    int cpu_burst = get_nonnegative_int("    CPU burst (1 이상, 0 입력하여 추가 완료): ");
    if (cpu_burst == 0)
      return;

    int io_burst = get_nonnegative_int("    I/O burst (0 이상): ");

    int io_request_time = -1;
    while (io_burst > 0) {
      io_request_time = get_nonnegative_int("    I/O request time (0 이상 CPU burst 이하): ");

      if (io_request_time <= cpu_burst)
        break;

      logger(LOG_ERROR, "I/O request time은 CPU burst(%d)보다 작거나 같아야 합니다.\n", cpu_burst);
    }

    Process *new_process = malloc(sizeof(Process));

    *new_process = (Process){
        .pid = (*pid_counter)++,
        .arrived_at = current_time,

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
}

int main(void) {
  int pid_counter = 1;
  Process *running_process = NULL;
  io_queue = create_queue();

  scheduler = create_fcfs_scheduler();
  logger(LOG_INFO, "%s 스케줄러를 실행할게요", scheduler->name);

  while (1) {
    char input[16];
    char choice;

    while (1) {
      print_time(current_time);
      printf("무엇을 할까요? (add/tick/finish/status/history/gantt/quit): ");

      if (!fgets(input, sizeof(input), stdin)) // EOF
        end_simulator(current_time);

      choice = tolower(input[0]);

      if (strchr("atfshgq", choice))
        break;
    }

    switch (choice) {
    case 'a':
      add_processes(scheduler, &pid_counter, current_time);
      continue;

    case 't':
      execute_one_tick(scheduler, io_queue, &running_process, &current_time);
      continue;

    case 'f':
      execute_until_all_done(scheduler, io_queue, &running_process, &current_time);
      continue;

    case 's':
      print_process_status(running_process);

      scheduler->print_state(scheduler);

      printf("\n    I/O 상태:\n");
      printf("      [대기 큐]\n");
      print_queue(io_queue);
      continue;

    case 'h':
      print_history();
      continue;

    case 'g':
      print_block_gantt_chart();
      print_inline_gantt_chart();
      continue;

    case 'q':
      end_simulator(current_time);
      break;
    }
  }

  end_simulator(current_time);
}
