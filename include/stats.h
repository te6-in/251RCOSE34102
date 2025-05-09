#ifndef STATS_H
#define STATS_H

typedef struct {
  int turnaround;
  int waiting;

  int finished_processes;
} Stats;

extern Stats stats;

void add_to_stats(int turnaround, int waiting);
void print_stats(void);

#endif
