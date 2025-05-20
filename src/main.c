#include "controller.h"
#include "executor.h"
#include "history.h"
#include "logger.h"
#include "process_queue.h"
#include "scheduler_fcfs.h"
#include "scheduler_psjf.h"
#include "scheduler_sjf.h"
#include "schedulers.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  int current_time = 0;
  int pid_counter = 1;

  Scheduler *scheduler = create_psjf_scheduler();
  ProcessQueue *io_queue = create_queue();
  Process *running_process = NULL;

  logger(LOG_INFO, "%s 스케줄러를 실행할게요", scheduler->name);

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
      execute_one_tick(scheduler, io_queue, &running_process, &current_time);
      continue;

    case 'f':
      execute_until_all_done(scheduler, io_queue, &running_process, &current_time);
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
