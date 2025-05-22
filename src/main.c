#include "controller.h"
#include "executor.h"
#include "history.h"
#include "logger.h"
#include "process_queue.h"
#include "scheduler_fcfs.h"
#include "scheduler_ppriority.h"
#include "scheduler_priority.h"
#include "scheduler_psjf.h"
#include "scheduler_rr.h"
#include "scheduler_sjf.h"
#include "schedulers.h"
#include "tsv.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int current_time = 0;

  Scheduler *scheduler = NULL;

  char *scheduler_flag = NULL;
  char *tsv_flag = "data/processes.tsv"; // default
  int non_interactive_flag = 0;
  int rr_time_quantum_flag = 0;

  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--scheduler=", 12) == 0) {
      scheduler_flag = argv[i] + 12; // 문자열 시작 주소로 설정하여 trim
    } else if (strncmp(argv[i], "--rr-time-quantum=", 18) == 0) {
      rr_time_quantum_flag = atoi(argv[i] + 18); // int로 변환

      if (rr_time_quantum_flag <= 0) {
        logger(LOG_ERROR, "--rr-time-quantum은 0보다 커야 합니다.\n");
        exit(1);
      }
    } else if (strncmp(argv[i], "--tsv=", 6) == 0) {
      tsv_flag = argv[i] + 6;
    } else if (strcmp(argv[i], "--non-interactive") == 0) {
      non_interactive_flag = 1;
    }
  }

  if (scheduler_flag) {
    if (strcmp(scheduler_flag, "fcfs") == 0)
      scheduler = create_fcfs_scheduler();
    else if (strcmp(scheduler_flag, "sjf") == 0)
      scheduler = create_sjf_scheduler();
    else if (strcmp(scheduler_flag, "psjf") == 0)
      scheduler = create_psjf_scheduler();
    else if (strcmp(scheduler_flag, "priority") == 0)
      scheduler = create_priority_scheduler();
    else if (strcmp(scheduler_flag, "ppriority") == 0)
      scheduler = create_ppriority_scheduler();
    else if (strcmp(scheduler_flag, "rr") == 0) {
      scheduler = create_rr_scheduler(rr_time_quantum_flag);
    } else {
      logger(LOG_ERROR, "scheduler flag를 확인해주세요: %s\n", scheduler_flag);
      exit(1);
    }
  }

  while (!scheduler) {
    int choice = get_positive_int("어떤 스케줄러를 사용할까요?\n"
                                  "  1 - FCFS\n"
                                  "  2 - SJF\n"
                                  "  3 - Preemptive SJF\n"
                                  "  4 - Priority\n"
                                  "  5 - Preemptive Priority\n"
                                  "  6 - RR\n"
                                  "입력: ");

    if (choice > 6) {
      logger(LOG_ERROR, "잘못된 입력입니다. 1~6 사이의 숫자를 입력하세요.\n");
      continue;
    }

    switch (choice) {
    case 1:
      scheduler = create_fcfs_scheduler();
      break;
    case 2:
      scheduler = create_sjf_scheduler();
      break;
    case 3:
      scheduler = create_psjf_scheduler();
      break;
    case 4:
      scheduler = create_priority_scheduler();
      break;
    case 5:
      scheduler = create_ppriority_scheduler();
      break;
    case 6:
      scheduler = create_rr_scheduler(rr_time_quantum_flag);
      break;
    }
  }

  ProcessQueue *io_queue = create_queue();
  Process *running_process = NULL;

  printf("\n");
  logger(LOG_INFO, "%s 스케줄러를 실행할게요\n", scheduler->name);
  scheduler->on_initialize(scheduler);

  int pending_process_count = 0;
  Process *pending_processes = load_processes_from_tsv(tsv_flag, &pending_process_count);

  if (non_interactive_flag) {
    execute_until_all_done(scheduler, io_queue, &running_process, &current_time, pending_processes,
                           pending_process_count);

    end_simulator(current_time, scheduler, io_queue);
  }

  int pid_counter = 0;
  for (int i = 0; i < pending_process_count; i++) {
    if (pending_processes[i].pid > pid_counter) {
      pid_counter = pending_processes[i].pid;
    }
  }
  pid_counter++;

  while (1) {
    char input[16];
    char choice;

    while (1) {
      print_time(current_time);
      printf("무엇을 할까요? "
             "(\033[4ma\033[0mdd/\033[4mt\033[0mick/\033[4mf\033[0minish/\033[4ms\033[0mtatus/"
             "\033[4mh\033[0mistory/\033[4mg\033[0mantt/\033[4mq\033[0muit): ");

      if (!fgets(input, sizeof(input), stdin)) // EOF
        end_simulator(current_time, scheduler, io_queue);

      choice = tolower(input[0]);

      if (strchr("atfshgq", choice))
        break;
    }

    switch (choice) {
    case 'a':
      add_process(scheduler, &pid_counter, current_time);
      continue;

    case 't':
      execute_one_tick(scheduler, io_queue, &running_process, &current_time, pending_processes,
                       pending_process_count);
      continue;

    case 'f':
      execute_until_all_done(scheduler, io_queue, &running_process, &current_time,
                             pending_processes, pending_process_count);
      continue;

    case 's':
      print_process_status(running_process);

      printf("\n    %s 스케줄러 상태:\n", scheduler->name);
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
      end_simulator(current_time, scheduler, io_queue);
      break; // unreachable
    }
  }

  return 0; // unreachable
}
