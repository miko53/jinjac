#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <stdlib.h>

typedef enum { FALSE, TRUE } BOOL;
typedef enum
{
  J_OK    = 0,
  J_ERROR = -1
} J_STATUS;

#define ASSERT(expr) assert(expr)
#define NEW(obj)    malloc(sizeof(obj))
#define UNUSED(x)   (void) x;



#endif /* COMMON_H */
