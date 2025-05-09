#include "history.h"
#include "logger.h"
#include "process.h"
#include "queue.h"
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_state(Process *running_process) {
  if (running_process && running_process->cpu_burst_remaining > 0) {
    printf("    [실행 중] P%d (%d 남음)\n", running_process->pid,
           running_process->cpu_burst_remaining);
  }

  print_ready_queue();
  print_io_queue();
}

static void end_simulator(int current_time) {
  logger(LOG_INFO, "시뮬레이터 종료 시각: %d", current_time);

  exit(0);
}

int main(void) {
  int current_time = 0;
  int pid_counter = 1;
  Process *running_process = NULL;

  logger(LOG_INFO, "FCFS");

  while (1) {
    char input[16];
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
      print_state(running_process);
      continue;

    case 'h':
      print_history();
      continue;

    case 'g':
      print_block_gantt_chart();
      print_inline_gantt_chart();
      continue;

    case 'n':
      while (1) {
        int cpu_burst = get_nonnegative_int("    CPU burst (0 입력하여 추가 완료): ");
        if (cpu_burst == 0)
          break;

        int io_burst = get_positive_int("    I/O burst: ");
        int io_request_time = get_positive_int("    I/O request time: ");

        Process *new_process = malloc(sizeof(Process));

        *new_process = (Process){
            .pid = pid_counter++,

            .arrival = current_time,

            .cpu_burst = cpu_burst,
            .cpu_burst_remaining = cpu_burst,

            .io_burst = io_burst,
            .io_burst_remaining = io_burst,
            .io_request_time = io_request_time,
            .is_in_io = false,
        };

        enqueue_ready(new_process);
      }

      print_state(running_process);
    }

    // 현재 실행 중인 프로세스가 없음
    if (!running_process || running_process->cpu_burst_remaining <= 0) {
      // 대기 큐 없음
      if (is_ready_queue_empty()) {
        print_duration(current_time, current_time + 1);
        printf("IDLE\n");

        record_idle_entry();

        current_time++;

        continue;
      }

      dequeue_ready(&running_process);
    }

    // 실행할 프로세스 있음

    // I/O 해야 함

    int cpu_time_used = running_process->cpu_burst - running_process->cpu_burst_remaining;
    if (cpu_time_used == running_process->io_request_time) {
      print_time(current_time);
      printf("프로세스 %d I/O 요청 (burst %d 완료)\n", running_process->pid, cpu_time_used);

      running_process->is_in_io = true;
      enqueue_io(running_process);

      running_process = NULL;

      current_time++;

      continue;
    }

    if (running_process->cpu_burst_remaining == running_process->cpu_burst) {
      print_time(current_time);
      printf("프로세스 %d 실행 시작 (burst %d 예정)\n", running_process->pid,
             running_process->cpu_burst_remaining);
    }

    running_process->cpu_burst_remaining--;

    print_duration(current_time, current_time + 1);
    printf("프로세스 %d 실행  (burst %d 남음)\n", running_process->pid,
           running_process->cpu_burst_remaining);

    record_history_entry(running_process);

    if (running_process->cpu_burst_remaining == 0) {
      print_time(current_time + 1);
      printf("프로세스 %d 종료 (burst %d 완료)\n", running_process->pid,
             running_process->cpu_burst);

      free(running_process);
      running_process = NULL;
    }

    current_time++;
  }

  end_simulator(current_time);
}
