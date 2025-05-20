#include "process.h"

const Process NULL_PROCESS = {
    .pid = -1,

    .arrived_at = -1,

    .cpu_burst = -1,
    .cpu_burst_remaining = -1,

    .io_burst = -1,
    .io_burst_remaining = -1,
    .io_request_time = -1,
    .is_in_io = false,

    .started_at = -1,
    .last_ready_enqueued_at = -1,
    .waiting = -1,
};

bool has_shorter_cpu_burst_remaining(Process *a, Process *b) {
  if (a->cpu_burst_remaining != b->cpu_burst_remaining)
    return a->cpu_burst_remaining < b->cpu_burst_remaining;

  // 같은 경우 arrived_at 참조
  return a->arrived_at < b->arrived_at;
}
