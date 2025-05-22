#include "logger.h"
#include "process.h"
#include "schedulers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256

void add_processes_from_tsv(Scheduler *scheduler, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    logger(LOG_INFO, "%s 파일이 없거나 열 수 없어서 미리 등록하지 않았어요.", filename);

    return;
  }

  int loaded = 0;

  char line[MAX_LINE_LEN];

  while (fgets(line, sizeof(line), file)) {

    if (line[0] == '#') // comment
      continue;

    Process temp = {0}; // 모든 필드 0으로 초기화
    int scanned = sscanf(line, "%d\t%d\t%d\t%d\t%d\t%d", &temp.pid, &temp.arrived_at,
                         &temp.priority, &temp.cpu_burst, &temp.io_burst, &temp.io_request_time);

    if (scanned != 6) // invalid
      continue;

    if (temp.arrived_at < 0 || temp.pid < 0 || temp.cpu_burst <= 0 || temp.io_burst < 0 ||
        temp.io_request_time < 0 || temp.io_request_time > temp.cpu_burst || temp.priority <= 0) {
      logger(LOG_ERROR, "이 프로세스는 불러올 수 없어요: %s", line);

      continue;
    }

    Process *process = malloc(sizeof(Process));

    *process = temp;
    process->cpu_burst_remaining = process->cpu_burst;
    process->io_burst_remaining = process->io_burst;
    process->is_in_io = false;
    process->started_at = -1;
    process->last_ready_enqueued_at = -1;
    process->waiting = 0;

    scheduler->enqueue(scheduler, process);

    loaded++;
  }

  logger(LOG_INFO, "%s 파일에서 %d개의 프로세스를 불러왔어요.", filename, loaded);

  fclose(file);
}
