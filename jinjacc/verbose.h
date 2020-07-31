#ifndef VERBOSE_H
#define VERBOSE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void verbose_setLevel(uint32_t level);
extern void verbose_print(uint32_t level, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* VERBOSE_H */

