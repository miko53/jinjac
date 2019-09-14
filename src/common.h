#ifndef _COMMON_H
#define _COMMON_H

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DBG_PRINT
#define TRACE

#ifdef DBG_PRINT
#define dbg_print(...)     fprintf(stdout, __VA_ARGS__)
#else
#define dbg_print(...)
#endif /* DBG_PRINT*/

#ifdef TRACE
#define trace(...)        fprintf(stderr, __VA_ARGS__)
#else
#define trace(...)
#endif

#define error(...)        fprintf(stderr, __VA_ARGS__)

typedef enum { FALSE, TRUE } BOOL;
#define ASSERT      assert
#define STATIC      static
#define NEW(obj)    malloc(sizeof(obj))

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H */
