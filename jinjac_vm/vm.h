#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stdio.h>
#include "stack.h"
#include "loader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  vm_stack stack;
  uint32_t SP;
  uint32_t PC;
  section_desc* pCode;
  section_desc* pData;
} vm_desc;

extern int32_t vm_exe(FILE* fOutputStream);
  
#ifdef __cplusplus
}
#endif

#endif /* VM_H */
