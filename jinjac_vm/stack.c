
#include "stack.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "trace.h"

static J_STATUS stack_reallocate(vm_stack* pStack);

J_STATUS stack_initialize(vm_stack* pStack, uint32_t initialAllocation)
{
  J_STATUS rc;
  rc = J_ERROR;

  pStack->stack = calloc(initialAllocation, sizeof(j_object*));
  if (pStack->stack != NULL)
  {
    rc = J_OK;
    pStack->capacity = initialAllocation;
    pStack->growCapacity = initialAllocation;
    pStack->top = 0;
  }

  return rc;
}

void stack_destroy(vm_stack* pStack)
{
  free(pStack->stack);
}


J_STATUS stack_push(vm_stack* pStack, j_object* jValue)
{
  J_STATUS rc;
  rc = J_OK;

  if (pStack->top >= pStack->capacity)
  {
    rc = stack_reallocate(pStack);
  }

  if (rc == 0)
  {
    pStack->stack[pStack->top] = jValue;
    pStack->top++;
  }
  else
  {
    trace("stack: no more memory\n");
    rc = J_ERROR;
  }

  return rc;
}

static J_STATUS stack_reallocate(vm_stack* pStack)
{
  J_STATUS rc;
  j_object** pNewArea;
  pNewArea = realloc(pStack->stack, (pStack->capacity + pStack->growCapacity) * sizeof(j_object*));
  if (pNewArea != NULL)
  {
    pStack->stack = pNewArea;
    pStack->capacity = pStack->capacity + pStack->growCapacity;
    rc = J_OK;
  }
  else
  {
    rc = J_ERROR;
  }

  return rc;
}


j_object* stack_top(vm_stack* pStack)
{
  j_object* pv;

  pv = NULL;
  if (pStack->top != 0)
  {
    pv = pStack->stack[pStack->top - 1];
  }

  return pv;
}

j_object* stack_top_n(vm_stack* pStack, int32_t offset)
{
  j_object* pv;

  pv = NULL;
  if ((pStack->top != 0) && ((((int32_t) pStack->top - 1 + offset) >= 0)))
  {
    pv = pStack->stack[pStack->top - 1 + offset];
  }
  else
  {
    //assert(FALSE);
  }

  return pv;
}


void stack_pop(vm_stack* pStack)
{
  if (pStack->top != 0)
  {
    pStack->top--;
  }
  else
  {
    assert(FALSE);
  }
}

