#ifndef LOG_H
#define LOG_H

typedef enum { LOG_ERROR, LOG_INFO } LogLevel;

void logger(LogLevel level, const char *fmt, ...);
void print_time(int time);
void print_duration(int start, int end);

#endif
