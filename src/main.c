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

static void print_process_status(Process *process) {
  if (!process) {
    logger(LOG_INFO, "현재 실행 중인 프로세스 없음\n");

    return;
  }

  if (process->is_in_io) {
    printf("    [I/O 중] P%d (%d 남음)\n", process->pid, process->io_burst_remaining);

    return;
  }

  if (process->cpu_burst_remaining > 0) {
    printf("    [실행 중] P%d (%d 남음)\n", process->pid, process->cpu_burst_remaining);

    return;
  }
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
      printf("무엇을 할까요? (new/continue/status/history/gantt/quit): ");

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
      print_process_status(running_process);
      print_ready_queue();
      print_io_queue();
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
        int cpu_burst = get_nonnegative_int("    CPU burst (1 이상, 0 입력하여 추가 완료): ");
        if (cpu_burst == 0)
          break;

        int io_burst = get_nonnegative_int("    I/O burst (0 이상): ");

        int io_request_time = -1;
        while (io_burst > 0) {
          io_request_time = get_nonnegative_int("    I/O request time (0 이상 CPU burst 이하): ");

          if (io_request_time <= cpu_burst)
            break;

          logger(LOG_ERROR, "I/O request time은 CPU burst(%d)보다 작거나 같아야 합니다.\n",
                 cpu_burst);
        }

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
    }

    // dequeue 필요한 경우 1: 현재 실행 중인 프로세스가 없음
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

    // dequeue 필요한 경우 2: 현재 실행 중인 프로세스가 I/O 들어감
    if (running_process && (running_process->cpu_burst - running_process->cpu_burst_remaining ==
                            running_process->io_request_time)) {
      print_time(current_time);
      printf("프로세스 %d I/O 요청 시작 (burst %d 남음, I/O %d 예정)\n", running_process->pid,
             running_process->cpu_burst_remaining, running_process->io_burst_remaining);

      running_process->is_in_io = true;
      enqueue_io(running_process);

      running_process = NULL;

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

    // 처음 실행하는 프로세스
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

    current_time++;

    // 프로세스 종료
    if (running_process->cpu_burst_remaining == 0) {
      print_time(current_time);
      printf("프로세스 %d 종료 (burst %d 완료)\n", running_process->pid,
             running_process->cpu_burst);

      free(running_process);
      running_process = NULL;
    }
  }

  end_simulator(current_time);
}
