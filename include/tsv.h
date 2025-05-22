#ifndef TSV_H
#define TSV_H

#include "process.h"

Process *load_processes_from_tsv(const char *filename, int *process_count);

#endif
