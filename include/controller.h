#ifndef CPU_H
#define CPU_H

#include "process.h"
#include "process_queue.h"
#include "schedulers.h"

void execute_one_tick(Scheduler *scheduler, ProcessQueue *io_queue, Process **running_process,
                      int *current_time);
void execute_until_all_done(Scheduler *scheduler, ProcessQueue *io_queue, Process **running_process,
                            int *current_time);

#endif
