#ifndef STACK_H
#define STACK_H

#include "j_value.h"
#include "j_object.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
  
typedef struct
{
  j_object** stack;
  uint32_t top;
  uint32_t capacity;
  uint32_t growCapacity;
} vm_stack;

extern J_STATUS stack_initialize(vm_stack* pStack, uint32_t initialAllocation);
extern J_STATUS stack_push(vm_stack* pStack, j_object* jValue);
extern j_object* stack_top(vm_stack* pStack);
extern j_object* stack_top_n(vm_stack* pStack, int32_t offset);
extern void stack_pop(vm_stack* pStack);

extern void stack_destroy(vm_stack* pStack);

#ifdef __cplusplus
}
#endif

#endif /* STACK_H */
