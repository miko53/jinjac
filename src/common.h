#ifndef COMMON
#define COMMON

#include <assert.h>

#define DBG_PRINT

#ifdef DBG_PRINT
#define dbg_print(...)     fprintf(stderr, __VA_ARGS__)
#else
#define dbg_print(...)
#endif /* DBG_RULES*/


typedef enum { FALSE, TRUE } BOOL;
#define ASSERT      assert
#define STATIC      static


#endif /* COMMON */
