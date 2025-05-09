#include "logger.h"
#include "process.h"
#include <stdio.h>

#define MAX_HISTORY 1000

static const Process IDLE_PROCESS = {
    .pid = -1,
    .arrival = -1,
    .cpu_burst = -1,
    .remaining_cpu_burst = -1,
};

static Process history[MAX_HISTORY];
static int history_count = 0;

int is_history_full(void) { return history_count >= MAX_HISTORY; }

void record_history_entry(const Process *process) {
  if (is_history_full()) {
    logger(LOG_ERROR, "히스토리가 꽉 찼어요. 더 이상 저장하지 않아요.");
    return;
  }

  history[history_count++] = *process;
}

void record_idle_entry(void) {
  if (is_history_full()) {
    logger(LOG_ERROR, "히스토리가 꽉 찼어요. 더 이상 저장하지 않아요.");
    return;
  }

  history[history_count++] = IDLE_PROCESS;
}

void print_history(void) {
  printf("\n  [히스토리]\n");

  if (history_count == 0) {
    printf("    (기록 없음)\n");
    return;
  }

  for (int i = 0; i < history_count; i++) {
    Process *p = &history[i];

    printf("    ");
    print_duration(i, i + 1);

    if (p->pid == IDLE_PROCESS.pid) {
      printf("IDLE\n");

      continue;
    }

    printf("프로세스 %d (%d/%d)\n", p->pid, p->cpu_burst - p->remaining_cpu_burst, p->cpu_burst);
  }
}

void print_block_gantt_chart(void) {
  printf("\n  [Gantt 차트]\n");

  if (history_count == 0) {
    printf("    (기록 없음)");
    return;
  }

  int start = 0;

  for (int i = 1; i <= history_count; i++) {
    if (i == history_count || history[i].pid != history[start].pid) {
      Process *p = &history[start];

      printf("    ");
      print_duration(start, i);

      if (p->pid == IDLE_PROCESS.pid) {
        printf("IDLE\n");
      } else {
        printf("프로세스 %d (%d/%d)\n", p->pid, i - start, p->cpu_burst);
      }

      start = i;
    }
  }
}

void print_inline_gantt_chart(void) {
  printf("\n  [Gantt 차트]\n");

  if (history_count == 0) {
    printf("    (기록 없음)");
    return;
  }

  printf("    ");

  int start = 0;

  for (int i = 1; i <= history_count; i++) {
    if (i == history_count || history[i].pid != history[start].pid) {
      Process *p = &history[start];

      printf("%d - ", start);

      if (p->pid == IDLE_PROCESS.pid) {
        printf("IDLE");
      } else {
        printf("프로세스 %d (%d/%d)", p->pid, i - start, p->cpu_burst);
      }

      printf(" - ");

      if (i == history_count)
        printf("%d", i);

      start = i;
    }
  }
}
