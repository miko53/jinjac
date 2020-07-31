#ifndef VM_OPCODE_H
#define VM_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
#define BUILDIN(x, r) FCT_## x,
#include "buildin.h"
#undef BUILDIN
} buildin_fct_id;

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
