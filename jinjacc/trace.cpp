#include <cstdio>
#include <cstdarg>
#include <libgen.h>
#include "trace.h"

void print_trace(char const* fileName, int32_t line, char const* fmt, ...)
{
  va_list list;
  va_start(list, fmt);
  fprintf(stderr, "%s_%d: ", basename( (char*) fileName), line);
  vfprintf(stderr, fmt, list);
  va_end(list);
}

void print_error(error_level level, char const* fmt, ...)
{
  va_list list;
  va_start(list, fmt);

  char const * levelStr[] = { "error", "warning" };

  fprintf(stderr, "%s: ", levelStr[level]);
  vfprintf(stderr, fmt, list);
  va_end(list);
}

