
#include "executor.h"
#include "history.h"
#include "logger.h"
#include "process.h"
#include "process_queue.h"
#include "schedulers.h"
#include "stats.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

static void tick(Scheduler *scheduler, ProcessQueue *io_queue, int *current_time) {
  // 1. 타이머 증가
  (*current_time)++;

  // 2. I/O 큐에서 끝나는 프로세스가 있는지 먼저 확인
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
  process->last_ready_enqueued_at = *current_time;

  print_time(*current_time);
  printf("프로세스 %d I/O 완료 (I/O %d 남음)\n", process->pid, process->io_burst_remaining);

  // on_tick
  scheduler->on_tick(scheduler);
}

void execute_one_tick(Scheduler *scheduler, ProcessQueue *io_queue, Process **running_process,
                      int *current_time) {
  // 현재 실행 중인 프로세스가 I/O 들어가야 하는 경우
  if (*running_process && ((*running_process)->is_in_io) == false &&
      (*running_process)->io_burst_remaining > 0 &&
      ((*running_process)->cpu_burst - (*running_process)->cpu_burst_remaining ==
       (*running_process)->io_request_time)) {
    print_time(*current_time);
    printf("프로세스 %d I/O 요청 시작 (burst %d 남음, I/O %d 예정)\n", (*running_process)->pid,
           (*running_process)->cpu_burst_remaining, (*running_process)->io_burst_remaining);

    (*running_process)->is_in_io = true;
    enqueue(io_queue, *running_process);
    *running_process = NULL; // running_process가 I/O 큐에 들어가면 running_process는 NULL
  }

  // 현재 실행 중인 프로세스가 원래 없었거나, I/O 큐에 들어가서 NULL이 되었을 수 있음
  if (!*running_process) {
    // 새로 꺼내기
    *running_process = scheduler->pick_next(scheduler);

    // 스케줄러가 줄 수 있는 프로세스가 없는 경우
    if (!*running_process) {
      print_duration(*current_time, *current_time + 1);
      printf("IDLE\n");

      record_idle_entry();
      tick(scheduler, io_queue, current_time);

      // IDLE 완료, 틱 종료
      return;
    }

    // 성공적으로 꺼낸 경우
    (*running_process)->waiting += *current_time - (*running_process)->last_ready_enqueued_at;
  }

  // 처음 실행하는 프로세스
  if ((*running_process)->cpu_burst_remaining == (*running_process)->cpu_burst) {
    (*running_process)->started_at = *current_time;

    print_time(*current_time);
    printf("프로세스 %d 실행 시작 (burst %d 예정)\n", (*running_process)->pid,
           (*running_process)->cpu_burst_remaining);
  }

  (*running_process)->cpu_burst_remaining--;

  print_duration(*current_time, *current_time + 1);
  printf("프로세스 %d 실행  (burst %d 남음)\n", (*running_process)->pid,
         (*running_process)->cpu_burst_remaining);

  record_history_entry(*running_process);

  tick(scheduler, io_queue, current_time);

  // 이번 틱에 프로세스 종료된 경우 알림
  if ((*running_process)->cpu_burst_remaining == 0) {
    int turnaround = *current_time - (*running_process)->arrived_at;
    print_time(*current_time);
    printf("프로세스 %d 종료 (burst %d 완료, turnaround %d, wait %d)\n", (*running_process)->pid,
           (*running_process)->cpu_burst, turnaround, (*running_process)->waiting);

    add_to_stats(turnaround, (*running_process)->waiting);

    free(*running_process);
    *running_process = NULL;
  }
}

void execute_until_all_done(Scheduler *scheduler, ProcessQueue *io_queue, Process **running_process,
                            int *current_time) {
  while (1) {
    if (scheduler->get_left_process_count(scheduler) == 0 && is_empty(io_queue) &&
        !*running_process) {
      logger(LOG_INFO, "모든 프로세스의 스케줄링과 I/O가 완료되었습니다.\n");

      break;
    }

    execute_one_tick(scheduler, io_queue, running_process, current_time);
  }
}
