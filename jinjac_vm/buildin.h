#ifndef BUILDIN_H
#define BUILDIN_H

#include <stdint.h>
#include "vm.h"
#include "vm_opcode.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t buildin_execute(vm_desc* pVM, buildin_fct_id fctId, uint32_t nbArgs);

#ifdef __cplusplus
}
#endif

#endif /* BUILDIN_H */
