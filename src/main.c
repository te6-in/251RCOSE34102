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

void tick_io_queue(void) {
  Process *process;

  if (!peek(io_queue, &process))
    return;

  // I/O 큐에 프로세스 있는 경우 head의 remaining -1
  (process->io_burst_remaining)--;

  if (process->io_burst_remaining > 0)
    return;

  // -1 했더니 0인 경우 I/O 큐에서 프로세스 제거 후 레디 큐 맨 뒤로 추가
  process->is_in_io = false;
  dequeue(io_queue, &process);
  scheduler->enqueue(scheduler, process);
  process->last_ready_enqueued_at = current_time;

  print_time(current_time);
  printf("프로세스 %d I/O 완료 (I/O %d 남음)\n", process->pid, process->io_burst_remaining);
}

static void end_simulator(int current_time) {
  logger(LOG_INFO, "시뮬레이터 종료 시각: %d\n", current_time);

  print_stats();

  exit(0);
}

static void tick(void) {
  current_time++;

  tick_io_queue();

  scheduler->on_tick(scheduler);
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

    // 현재 실행 중인 프로세스가 I/O 들어가야 하는 경우
    if (running_process && running_process->is_in_io == false &&
        running_process->io_burst_remaining > 0 &&
        (running_process->cpu_burst - running_process->cpu_burst_remaining ==
         running_process->io_request_time)) {
      print_time(current_time);
      printf("프로세스 %d I/O 요청 시작 (burst %d 남음, I/O %d 예정)\n", running_process->pid,
             running_process->cpu_burst_remaining, running_process->io_burst_remaining);

      running_process->is_in_io = true;
      enqueue(io_queue, running_process);

      // running_process가 I/O 큐에 들어가면 running_process는 NULL
      running_process = NULL;
    }

    // 현재 실행 중인 프로세스가 원래 없었거나, I/O 큐에 들어가서 NULL이 되었을 수 있음
    if (!running_process) {
      // 새로 꺼내기
      running_process = scheduler->pick_next(scheduler);

      // 스케줄러가 줄 수 있는 프로세스가 없는 경우
      if (!running_process) {
        print_duration(current_time, current_time + 1);
        printf("IDLE\n");

        record_idle_entry();
        tick();

        continue;
      }

      // 꺼내짐
      running_process->waiting += current_time - running_process->last_ready_enqueued_at;
    }

    // 처음 실행하는 프로세스
    if (running_process->cpu_burst_remaining == running_process->cpu_burst) {
      running_process->started_at = current_time;

      print_time(current_time);
      printf("프로세스 %d 실행 시작 (burst %d 예정)\n", running_process->pid,
             running_process->cpu_burst_remaining);
    }

    running_process->cpu_burst_remaining--;

    print_duration(current_time, current_time + 1);
    printf("프로세스 %d 실행  (burst %d 남음)\n", running_process->pid,
           running_process->cpu_burst_remaining);

    record_history_entry(running_process);
    tick();

    // 프로세스 종료
    if (running_process->cpu_burst_remaining == 0) {
      int turnaround = current_time - running_process->arrived_at;

      print_time(current_time);
      printf("프로세스 %d 종료 (burst %d 완료, turnaround %d, wait %d)\n", running_process->pid,
             running_process->cpu_burst, turnaround, running_process->waiting);

      add_to_stats(turnaround, running_process->waiting);

      free(running_process);
      running_process = NULL;
    }
  }

  end_simulator(current_time);
}
