#include "trace.h"
#include <stdarg.h>
#include <stdio.h>
#include <libgen.h>


void print_trace(char* fileName, int32_t line, char* fmt, ...)
{
  va_list list;
  va_start(list, fmt);
  fprintf(stderr, "%s_%d: ", basename(fileName), line);
  vfprintf(stderr, fmt, list);
  va_end(list);
}



