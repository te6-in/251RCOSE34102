#include "process.h"
#include "queue.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_HISTORY 1000

static const Process IDLE_PROCESS = {
    .pid = -1,
    .arrival = -1,
    .cpu_burst = -1,
    .remaining_cpu_burst = -1,
};

Process history[MAX_HISTORY];
int history_count = 0;

int is_history_full(void) { return history_count >= MAX_HISTORY; }

void record_history_entry(const Process *process) {
  if (is_history_full())
    return;

  history[history_count++] = *process;
}

void record_idle_entry(void) {
  if (is_history_full())
    return;

  history[history_count++] = IDLE_PROCESS;
}

void print_history(void) {
  printf("\n[히스토리]\n");

  for (int i = 0; i < history_count; i++) {
    Process *p = &history[i];

    if (p->pid == IDLE_PROCESS.pid) {
      printf("Time %d~%d — CPU idle\n", i, i + 1);

      continue;
    }

    printf("Time %d~%d — 프로세스 %d (%d/%d)\n", i, i + 1, p->pid,
           p->cpu_burst - p->remaining_cpu_burst, p->cpu_burst);
  }
}

static void print_state(Process *running_process) {
  if (running_process->remaining_cpu_burst > 0) {
    printf("    [실행 중] P%d (%d 남음)\n", running_process->pid,
           running_process->remaining_cpu_burst);
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
  Process running_process;

  printf("FCFS\n");

  while (1) {
    char input[8];
    char choice;

    while (1) {
      printf("\nTime %d   — 프로세스를 추가할까요? (yes/no/state/history/quit): ", current_time);

      if (!fgets(input, sizeof(input), stdin)) // EOF
        end_simulator(current_time);

      choice = input[0];

      if (choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N' || choice == 'q' ||
          choice == 'Q' || choice == 's' || choice == 'S' || choice == 'h' || choice == 'H') {
        break;
      }
    }

    switch (choice) {
    case 'q':
    case 'Q':
      end_simulator(current_time);
      break;

    case 'h':
    case 'H':
      print_history();
      continue;

    case 's':
    case 'S':
      print_state(&running_process);
      continue;

    case 'y':
    case 'Y':
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
        printf("Time %d~%d — CPU idle\n", current_time, current_time + 1);

        record_idle_entry();

        current_time++;

        continue;
      }

      dequeue(&running_process);
    }

    // 실행할 프로세스 있음

    if (running_process.remaining_cpu_burst == running_process.cpu_burst) {
      printf("Time %d   — 프로세스 %d 실행 시작 (burst %d 예정)\n", current_time,
             running_process.pid, running_process.remaining_cpu_burst);
    }

    running_process.remaining_cpu_burst--;

    printf("Time %d~%d — 프로세스 %d 실행  (burst %d 남음)\n", current_time, current_time + 1,
           running_process.pid, running_process.remaining_cpu_burst);

    record_history_entry(&running_process);

    if (running_process.remaining_cpu_burst == 0) {
      printf("Time   %d — 프로세스 %d 종료 (burst %d 완료)\n", current_time + 1,
             running_process.pid, running_process.cpu_burst);
    }

    current_time++;
  }

  end_simulator(current_time);
}
