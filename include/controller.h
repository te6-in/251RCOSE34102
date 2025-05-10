#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "process.h"
#include "process_queue.h"
#include "schedulers.h"

void print_process_status(Process *process);
void end_simulator(int current_time, Scheduler *scheduler, ProcessQueue *io_queue);
void add_processes(Scheduler *scheduler, int *pid_counter, int current_time);

#endif
