#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

static const char *level_labels[] = {
    "\x1b[1;41;37m ERROR ",
    "\x1b[1;44;37m INFO ",
};

static const char *color_reset = "\x1b[1;0m";

void logger(LogLevel level, const char *fmt, ...) {
  FILE *out = (level == LOG_ERROR) ? stderr : stdout;
  fprintf(out, "%s%s ", level_labels[level], color_reset);

  va_list args;
  va_start(args, fmt);
  vfprintf(out, fmt, args);
  va_end(args);
}

void print_time(int time) { printf("\n\x1b[1;42m Time %2d %s ", time, color_reset); }

void print_duration(int start, int end) {
  printf("\x1b[1;42m %2d ~ %2d %s ", start, end, color_reset);
}
