#ifndef _COMMON_H
#define _COMMON_H

#include <assert.h>
#include <stdlib.h>
#include "trace.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JINJAC_DEBUG
#define dbg_print(...)       fprintf(stdout, __VA_ARGS__)
#define ASSERT(expr)         assert(expr)
#else
#define dbg_print(...)
#define ASSERT(expr)         ((void) 0)
#endif /* JINJAC_DEBUG*/

#ifdef TRACE
#define trace(...)           print_trace(__FILE__, __LINE__, __VA_ARGS__)
#else
#define trace(...)           ((void) (0))
#endif

#define error(level, ...)    print_error(level, __VA_ARGS__)

#define STATIC      static

typedef enum 
{
  J_OK,
  J_ERROR
} J_STATUS;
  
#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H */
