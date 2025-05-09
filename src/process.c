#include "process.h"

const Process NULL_PROCESS = {
    .pid = -1,

    .arrival = -1,

    .cpu_burst = -1,
    .cpu_burst_remaining = -1,

    .io_burst = -1,
    .io_burst_remaining = -1,
    .io_request_time = -1,
    .is_in_io = false,
};
