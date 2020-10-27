#include "verbose.h"
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

static uint32_t verbose_level = 0;

void verbose_setLevel(uint32_t level)
{
  verbose_level = level;
}


void verbose_print(uint32_t level, const char* fmt, ...)
{
  if (level <= verbose_level)
  {
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
  }
}
