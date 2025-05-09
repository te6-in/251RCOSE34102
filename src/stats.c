#include "stats.h"
#include <stdio.h>

Stats stats = {
    .turnaround = 0,
    .waiting = 0,
    .finished_processes = 0,
};

void add_to_stats(int turnaround, int waiting) {
  stats.turnaround += turnaround;
  stats.waiting += waiting;
  stats.finished_processes++;
}

void print_stats(void) {
  printf("    [통계]\n");

  if (stats.finished_processes == 0) {
    printf("      아직 종료된 프로세스가 없습니다.\n");

    return;
  }

  printf("      [완료된 프로세스] %d\n", stats.finished_processes);
  printf("      [평균 Turnaround] %.2f\n", (double)stats.turnaround / stats.finished_processes);
  printf("      [평균 Waiting]    %.2f\n", (double)stats.waiting / stats.finished_processes);
}
