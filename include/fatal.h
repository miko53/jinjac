#ifndef FATAL_H
#define FATAL_H

#include <stdlib.h>
#include "trace.h"

#define fatal(...)           do { print_trace(__FILE__, __LINE__, __VA_ARGS__); exit(EXIT_FAILURE); } while (0);

#endif /* FATAL_H */

