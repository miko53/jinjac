
#ifndef TRACE_H
#define TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef INSERT_TRACE
#define trace(...)           print_trace(__FILE__, __LINE__, __VA_ARGS__)
#else
#define trace(...)           ((void) (0))
#endif

extern void print_trace(char* fileName, int32_t line, char* fmt, ...);


#ifdef __cplusplus
}
#endif

#endif /* TRACE_H */

