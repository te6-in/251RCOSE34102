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
