#ifndef HISTORY_H
#define HISTORY_H

#include "process.h"

int is_history_full(void);
void record_history_entry(const Process *process);
void record_idle_entry(void);
void print_history(void);
void print_block_gantt_chart(void);
void print_inline_gantt_chart(void);

#endif
