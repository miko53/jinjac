
#include <stdio.h>
#include <stdlib.h>
#include "convert.h"
#include "common.h"

char* intToStr(int value)
{
  int size = 0;
  char* p = NULL;
  size = snprintf(p, size, "%d", value);
  if (size < 0)
  {
    p = NULL;
  }
  else
  {
    size++;
    p = malloc(size);
    ASSERT(p != NULL);
    snprintf(p, size, "%d", value);
  }

  return p;
}

char* doubleToStr(double value)
{
  int size = 0;
  char* p = NULL;
  size = snprintf(p, size, "%f", value);
  if (size < 0)
  {
    p = NULL;
  }
  else
  {
    size++;
    p = malloc(size);
    ASSERT(p != NULL);
    snprintf(p, size, "%f", value);
  }

  return p;
}


