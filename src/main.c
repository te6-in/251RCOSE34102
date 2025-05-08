#include "process.h"
#include "queue.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

static void print_state(Process *running) {
  if (running->remaining_cpu_burst > 0) {
    printf("    [실행 중] P%d (%d 남음)\n", running->pid, running->remaining_cpu_burst);
  }

  print_queue();
}

static void end_simulator(int current_time) {
  printf("\n시뮬레이터 종료 시각: %d\n", current_time);

  exit(0);
}

int main(void) {
  int current_time = 0;
  int pid_counter = 1;
  Process running;

  printf("FCFS\n");

  while (1) {
    char input[8];
    char choice;

    while (1) {
      printf("\nTime %d   — 프로세스를 추가할까요? (y/n/p/q): ", current_time);

      if (!fgets(input, sizeof(input), stdin)) // EOF
        end_simulator(current_time);

      choice = input[0];

      if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N' || choice == 'q' ||
          choice == 'Q' || choice == 'p' || choice == 'P')
        break;
    }

    switch (choice) {
    case 'q':
    case 'Q':
      end_simulator(current_time);
      break;

    case 'p':
    case 'P':
      print_state(&running);
      continue;

    case 'y':
    case 'Y':
      int burst = get_positive_int("    CPU burst: ");

      Process p = {pid_counter++, current_time, burst, burst};
      enqueue(p);

      print_state(&running);
    }

    // 현재 실행 중인 프로세스가 없음
    if (running.remaining_cpu_burst <= 0) {
      // 대기 큐 없음
      if (is_queue_empty()) {
        printf("Time %d~%d — CPU idle\n", current_time, current_time + 1);
        current_time++;

        continue;
      }

      dequeue(&running);
    }

    // 실행할 프로세스 있음

    if (running.remaining_cpu_burst == running.cpu_burst) {
      printf("Time %d   — 프로세스 %d 실행 시작 (burst %d 예정)\n", current_time, running.pid,
             running.remaining_cpu_burst);
    }

    running.remaining_cpu_burst--;

    printf("Time %d~%d — 프로세스 %d 실행  (burst %d 남음)\n", current_time, current_time + 1,
           running.pid, running.remaining_cpu_burst);

    if (running.remaining_cpu_burst == 0) {
      printf("Time   %d — 프로세스 %d 종료 (burst %d 완료)\n", current_time + 1, running.pid,
             running.cpu_burst);
    }

    current_time++;
  }

  end_simulator(current_time);
}
