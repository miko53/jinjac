#ifndef VM_OPCODE_H
#define VM_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  FCT_CAPITALIZE,
  FCT_CENTER,
  FCT_FORMAT,
  FCT_JOIN,
  FCT_LOWER,
  FCT_UPPER,
  FCT_RANGE,
  FCT_TITLE,
  FCT_TRIM,
  FCT_TRUNCATE
} buildin_fct_id;

extern const char* vm_buildin_fct[];

typedef enum
{
#define OPCODE(x) OP_ ## x,
#include "ops.h"
#undef OPCODE
} VM_BYTE_CODE;

#ifdef __cplusplus
}
#endif

#endif /* VM_OPCODE_H */
