#include "history.h"
#include "logger.h"
#include "process.h"
#include "queue.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_state(Process *running_process) {
  if (running_process->remaining_cpu_burst > 0) {
    printf("    [실행 중] P%d (%d 남음)\n", running_process->pid,
           running_process->remaining_cpu_burst);
  }

  print_queue();
}

static void end_simulator(int current_time) {
  logger(LOG_INFO, "시뮬레이터 종료 시각: %d", current_time);

  exit(0);
}

int main(void) {
  int current_time = 0;
  int pid_counter = 1;
  Process running_process;

  logger(LOG_INFO, "FCFS");

  while (1) {
    char input[8];
    char choice;

    while (1) {
      print_time(current_time);
      printf("무엇을 할까요? (new/continue/state/history/gantt/quit): ");

      if (!fgets(input, sizeof(input), stdin)) // EOF
        end_simulator(current_time);

      choice = tolower(input[0]);

      if (strchr("ncshgq", choice))
        break;
    }

    switch (choice) {
    case 'q':
      end_simulator(current_time);
      break;

    case 's':
      print_state(&running_process);
      continue;

    case 'h':
      print_history();
      continue;

    case 'g':
      print_block_gantt_chart();
      print_inline_gantt_chart();
      continue;

    case 'n':
      int burst = get_positive_int("    CPU burst: ");

      enqueue((Process){
          .pid = pid_counter++,
          .arrival = current_time,
          .cpu_burst = burst,
          .remaining_cpu_burst = burst,
      });

      print_state(&running_process);
    }

    // 현재 실행 중인 프로세스가 없음
    if (running_process.remaining_cpu_burst <= 0) {
      // 대기 큐 없음
      if (is_queue_empty()) {
        print_duration(current_time, current_time + 1);
        printf("IDLE\n");

        record_idle_entry();

        current_time++;

        continue;
      }

      dequeue(&running_process);
    }

    // 실행할 프로세스 있음

    if (running_process.remaining_cpu_burst == running_process.cpu_burst) {
      print_time(current_time);
      printf("프로세스 %d 실행 시작 (burst %d 예정)\n", running_process.pid,
             running_process.remaining_cpu_burst);
    }

    running_process.remaining_cpu_burst--;

    print_duration(current_time, current_time + 1);
    printf("프로세스 %d 실행  (burst %d 남음)\n", running_process.pid,
           running_process.remaining_cpu_burst);

    record_history_entry(&running_process);

    if (running_process.remaining_cpu_burst == 0) {
      print_time(current_time + 1);
      printf("프로세스 %d 종료 (burst %d 완료)\n", running_process.pid, running_process.cpu_burst);
    }

    current_time++;
  }

  end_simulator(current_time);
}
