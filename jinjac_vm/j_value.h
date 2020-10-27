
#ifndef J_VALUE_H
#define J_VALUE_H

#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  J_STRING,
  J_INT,
  J_DOUBLE,
} j_value_type;

typedef union
{
  char* type_string;
  int32_t type_int;
  double type_double;
} j_value_base;

typedef struct
{
  j_value_type type;
  j_value_base value;
} j_value;

void j_value_destroy(j_value* p);

#ifdef __cplusplus
}
#endif

#endif /* J_VALUE_H */
