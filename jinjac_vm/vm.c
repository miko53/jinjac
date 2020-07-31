
#include "vm.h"
#include "loader.h"
#include "vm_opcode.h"
#include "assert.h"
#include "endian.h"
#include "stack.h"
#include "j_value.h"
#include <stdlib.h>
#include <string.h>
#include "buildin.h"
#include "j_array.h"
#include "param.h"
#include "trace.h"
#include "fatal.h"

static vm_desc vm_state;

static J_STATUS vm_execute_copy_string(uint8_t* pStr, FILE* fOutputStream);
static J_STATUS vm_execute_push_string(vm_desc* pVM, uint32_t offset);
static J_STATUS vm_execute_eval(vm_desc* pVM, FILE* pOutput);
static J_STATUS vm_execute_load_symbol(vm_desc* pVM, uint32_t offset);
static J_STATUS vm_execute_load_integer(vm_desc* pVM, uint32_t value);
static J_STATUS vm_execute_load_double(vm_desc* pVM, double value);
static J_STATUS vm_execute_computation(vm_desc* pVM, char c);
static J_STATUS vm_execute_condition(vm_desc* pVM, j_condition c);
static J_STATUS vm_execute_logical_condition(vm_desc* pVM, j_logical_condition c);
static J_STATUS vm_execute_array_get_at(vm_desc* pVM);
static J_STATUS vm_execute_create_symbol(vm_desc* pVM, uint32_t offset);
static J_STATUS vm_execute_delete_symbol(vm_desc* pVM, uint32_t offset);
static J_STATUS vm_execute_set_symbol(vm_desc* pVM, uint32_t offset);
static J_STATUS vm_execute_create_iterator(vm_desc* pVM);
static J_STATUS vm_execute_iterator_load_first(vm_desc* pVM);
static J_STATUS vm_execute_iterator_load_next(vm_desc* pVM);
static J_STATUS vm_execute_iterator_end_reached(vm_desc* pVM);
static J_STATUS vm_execute_iterator_increment(vm_desc* pVM);
static J_STATUS vm_execute_branch_if_true(vm_desc* pVM, uint32_t offset);
static J_STATUS vm_execute_branch_if_false(vm_desc* pVM, uint32_t offset);
static J_STATUS vm_execute_create_array(vm_desc* pVM, BOOL isTuple);

J_STATUS vm_exe(FILE* fOutputStream)
{
  J_STATUS rc;
  J_STATUS status;
  uint32_t pc;
  BOOL bStopped;

  VM_BYTE_CODE instr;
  rc = J_ERROR;

  status = J_OK;
  bStopped = FALSE;
  vm_state.SP = 0;
  vm_state.PC = 0;

  vm_state.pCode = jinjac_loader_getCodeSection();
  vm_state.pData = jinjac_loader_getDataSection();

  stack_initialize(&vm_state.stack, 20);

  while ((status == J_OK) && (!bStopped) &&
         ((vm_state.PC + 1) <= vm_state.pCode->size))
  {
    instr = vm_state.pCode->pSection[vm_state.PC];
    pc = vm_state.PC++;
    switch (instr)
    {
      case OP_COPY_STRING:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t offset = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          offset = be32toh(offset);
          vm_state.PC += 4;
          trace("%.4d: OP_COPY_STRING %d\n", pc, offset);
          if (offset < vm_state.pData->size)
          {
            status = vm_execute_copy_string(&vm_state.pData->pSection[offset], fOutputStream);
          }
          else
          {
            trace("offset out of range (%d)\n", offset);
            status = J_ERROR;
          }
        }
        else
        {
          status = J_ERROR;
          trace("error in PC = %d, file too short\n", vm_state.PC);
        }
        break;

      case OP_NOP:
        trace("%.4d: OP_NOP\n", pc);
        break;

      case OP_HALT:
        trace("%.4d: OP_HALT\n", pc);
        trace("Stack = %d\n", vm_state.stack.top);
        for (uint32_t i = 0; i < vm_state.stack.top; i++)
        {
          j_object* p = stack_top(&vm_state.stack);
          j_object_display(p);
          //j_object_delete(p);
        }
        bStopped = TRUE;
        break;

      case OP_PUSH_STRING:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t offset = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          offset = be32toh(offset);
          vm_state.PC += 4;
          trace("%.4d: OP_PUSH_STRING %d\n", pc, offset);
          if (offset < vm_state.pData->size)
          {
            status = vm_execute_push_string(&vm_state, offset);
          }
          else
          {
            trace("offset out of range (%d)\n", offset);
            status = J_ERROR;
          }
        }
        break;

      case OP_PUSH_NIL:
        {
          j_object_integer* pObject;
          trace("%.4d: OP_PUSH_NIL\n", pc);
          pObject = j_object_integer_new(0);
          status = stack_push(&vm_state.stack, (j_object*) pObject);
        }
        break;

      case OP_EVAL:
        trace("%.4d: OP_EVAL\n", pc);
        status = vm_execute_eval(&vm_state, fOutputStream);
        break;

      case OP_CALL_BUILDIN_FCT:
        if ((vm_state.PC + 8) <= vm_state.pCode->size)
        {
          uint32_t fctID = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          fctID = be32toh(fctID);
          vm_state.PC += 4;
          uint32_t nbArgs = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          nbArgs = be32toh(nbArgs);
          vm_state.PC += 4;
          trace("%.4d: OP_CALL_BUILDIN_FCT %d (args = %d)\n", pc, fctID, nbArgs);
          status = buildin_execute(&vm_state, (buildin_fct_id) fctID, nbArgs);
        }
        break;

      case OP_LOAD_SYMBOL:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t identiferOffset = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          identiferOffset = be32toh(identiferOffset);
          vm_state.PC += 4;
          trace("%.4d: OP_LOAD_SYMBOL %d\n", pc, identiferOffset);
          status = vm_execute_load_symbol(&vm_state, identiferOffset);
        }
        break;

      case OP_LOAD_INTEGER:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t value = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          value = be32toh(value);
          vm_state.PC += 4;
          trace("%.4d: OP_LOAD_INTEGER %d\n", pc, value);
          status = vm_execute_load_integer(&vm_state, value);
        }
        break;

      case OP_LOAD_DOUBLE:
        if ((vm_state.PC + 8) <= vm_state.pCode->size)
        {
          uint64_t value = (*( (uint64_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          value = be64toh(value);
          double ddl;
          ddl = *((double*) &value);
          vm_state.PC += 8;
          trace("%.4d: OP_LOAD_DOUBLE %f\n", pc, ddl);
          status = vm_execute_load_double(&vm_state, ddl);
        }
        break;

      case OP_ADD:
        trace("%.4d: OP_ADD \n", pc);
        status = vm_execute_computation(&vm_state, '+');
        break;

      case OP_SUB:
        trace("%.4d: OP_SUB \n", pc);
        status = vm_execute_computation(&vm_state, '-');
        break;

      case OP_MUL:
        trace("%.4d: OP_MUL \n", pc);
        status = vm_execute_computation(&vm_state, '*');
        break;

      case OP_DIV:
        trace("%.4d: OP_DIV \n", pc);
        status = vm_execute_computation(&vm_state, '/');
        break;

      case OP_GET_AT:
        trace("%.4d: OP_GET_AT \n", pc);
        status = vm_execute_array_get_at(&vm_state);
        break;

      case OP_CREATE_SYMBOL:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t identiferOffset = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          identiferOffset = be32toh(identiferOffset);
          vm_state.PC += 4;
          trace("%.4d: OP_CREATE_SYMBOL %d\n", pc, identiferOffset);
          status = vm_execute_create_symbol(&vm_state, identiferOffset);
        }
        break;

      case OP_DELETE_SYMBOL:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t identiferOffset = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          identiferOffset = be32toh(identiferOffset);
          vm_state.PC += 4;
          trace("%.4d: OP_DELETE_SYMBOL %d\n", pc, identiferOffset);
          status = vm_execute_delete_symbol(&vm_state, identiferOffset);
        }
        break;

      case OP_SET_SYMBOL:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t identiferOffset = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          identiferOffset = be32toh(identiferOffset);
          vm_state.PC += 4;
          trace("%.4d: OP_SET_SYMBOL %d\n", pc, identiferOffset);
          status = vm_execute_set_symbol(&vm_state, identiferOffset);
        }
        break;

      case OP_CREATE_ITERATOR:
        trace("%.4d: OP_CREATE_ITERATOR\n", pc);
        status = vm_execute_create_iterator(&vm_state);
        break;

      case OP_ITERATOR_LOAD_FIRST:
        trace("%.4d: OP_ITERATOR_LOAD_FIRST\n", pc);
        status = vm_execute_iterator_load_first(&vm_state);
        break;

      case OP_POP:
        trace("%.4d: OP_POP\n", pc);
        {
          j_object* p = stack_top(&vm_state.stack);
          stack_pop(&vm_state.stack);
          j_object_delete(p);
        }
        break;

      case OP_ITERATOR_TEST_END_REACHED:
        trace("%.4d: OP_ITERATOR_TEST_END_REACHED\n", pc);
        status = vm_execute_iterator_end_reached(&vm_state);
        break;

      case OP_ITERATOR_INCR:
        trace("%.4d: OP_ITERATOR_INCR\n", pc);
        status = vm_execute_iterator_increment(&vm_state);
        break;

      case OP_B_TRUE:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t offsetPC = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          offsetPC = be32toh(offsetPC);
          vm_state.PC += 4;
          trace("%.4d: OP_B_TRUE %d\n", pc, offsetPC);
          if (offsetPC <= vm_state.pCode->size)
          {
            status = vm_execute_branch_if_true(&vm_state, offsetPC);
          }
          else
          {
            status = J_ERROR;
          }
        }
        break;

      case OP_B_FALSE:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t offsetPC = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          offsetPC = be32toh(offsetPC);
          vm_state.PC += 4;
          trace("%.4d: OP_B_FALSE %d\n", pc, offsetPC);
          if (offsetPC <= vm_state.pCode->size)
          {
            status = vm_execute_branch_if_false(&vm_state, offsetPC);
          }
          else
          {
            status = J_ERROR;
          }
        }
        break;

      case OP_JUMP:
        if ((vm_state.PC + 4) <= vm_state.pCode->size)
        {
          uint32_t offsetPC = (*( (uint32_t*) (&vm_state.pCode->pSection[vm_state.PC])));
          offsetPC = be32toh(offsetPC);
          vm_state.PC += 4;
          trace("%.4d: OP_JUMP %d\n", pc, offsetPC);
          if (offsetPC <= vm_state.pCode->size)
          {
            status = 0;
            vm_state.PC = offsetPC;
          }
          else
          {
            status = J_ERROR;
          }
        }
        break;

      case OP_ITERATOR_LOAD_NEXT:
        trace("%.4d: OP_ITERATOR_LOAD_NEXT\n", pc);
        status = vm_execute_iterator_load_next(&vm_state);
        break;

      case OP_TEST_EQ:
        trace("%.4d: OP_TEST_EQ\n", pc);
        status = vm_execute_condition(&vm_state, COND_EQUAL);
        break;

      case OP_TEST_NEQ:
        trace("%.4d: OP_TEST_NEQ\n", pc);
        status = vm_execute_condition(&vm_state, COND_DIFFERENT);
        break;

      case OP_TEST_GE:
        trace("%.4d: OP_TEST_GE\n", pc);
        status = vm_execute_condition(&vm_state, COND_HIGH_AND_EQUAL_THAN);
        break;

      case OP_TEST_GT:
        trace("%.4d: OP_TEST_GT\n", pc);
        status = vm_execute_condition(&vm_state, COND_HIGH_THAN);
        break;

      case OP_TEST_LE:
        trace("%.4d: OP_TEST_LE\n", pc);
        status = vm_execute_condition(&vm_state, COND_LOWER_AND_EQUAL_THAN);
        break;

      case OP_TEST_LT:
        trace("%.4d: OP_TEST_LT\n", pc);
        status = vm_execute_condition(&vm_state, COND_LOWER_THAN);
        break;

      case OP_TEST_OR:
        trace("%.4d: OP_TEST_OR\n", pc);
        status = vm_execute_logical_condition(&vm_state, COND_LOG_OR);
        break;

      case OP_TEST_AND:
        trace("%.4d: OP_TEST_AND\n", pc);
        status = vm_execute_logical_condition(&vm_state, COND_LOG_AND);
        break;

      case OP_TEST_NOT:
        trace("%.4d: OP_TEST_NOT\n", pc);
        status = vm_execute_logical_condition(&vm_state, COND_LOG_NOT);
        break;

      case OP_TEST_IS:
        trace("%.4d: OP_TEST_IS\n", pc);
        assert(FALSE);
        break;

      case OP_CREATE_TUPLE:
        trace("%.4d: OP_CREATE_TUPLE\n", pc);
        status = vm_execute_create_array(&vm_state, TRUE);
        break;

      case OP_CREATE_ARRAY:
        trace("%.4d: OP_CREATE_ARRAY\n", pc);
        status = vm_execute_create_array(&vm_state, FALSE);
        break;

      default:
        assert(FALSE);
        break;
    }
  }

  if ((bStopped == TRUE) && (status == J_OK))
  {
    rc = J_OK;
  }
  else
  {
    fatal("vm executable failed at PC = %d\n", vm_state.PC);
  }

  stack_destroy(&vm_state.stack);
  return rc;
}

static J_STATUS vm_execute_copy_string(uint8_t* pStr, FILE* pOutput)
{
  J_STATUS rc;
  rc = fprintf(pOutput, "%s", pStr);
  if (rc > 0)
  {
    rc = J_OK;
  }
  return rc;
}

static J_STATUS vm_execute_push_string(vm_desc* pVM, uint32_t offset)
{
  J_STATUS rc;
  char* p;
  j_object_string* value;

  p = strdup((char*) &pVM->pData->pSection[offset]);
  value = j_object_string_new(p);
  rc = stack_push(&pVM->stack, (j_object*) value);
  return rc;
}

J_STATUS vm_execute_eval(vm_desc* pVM, FILE* pOutput)
{
  J_STATUS rc;
  j_object* o = stack_top(&pVM->stack);
  if (o == NULL)
  {
    rc = J_ERROR;
  }
  else
  {
    char* pStr;
    pStr = j_object_toString(o);
    if (pStr != NULL)
    {
      rc = fprintf(pOutput, "%s", pStr);
      if (rc > 0)
      {
        rc = J_OK;
      }
      free(pStr);
    }
    else
    {
      rc = J_ERROR;
      trace("unable to convert object at PC = %d\n", pVM->PC);
    }
    j_object_delete(o);
    stack_pop(&pVM->stack);
  }

  return rc;
}


J_STATUS vm_execute_load_symbol(vm_desc* pVM, uint32_t offset)
{
  J_STATUS rc;
  char* p;
  rc = J_ERROR;
  j_object_identifier* pIdentifier;

  p = strdup((char*) &pVM->pData->pSection[offset]);
  pIdentifier = j_object_identifier_new(p);
  rc = stack_push(&pVM->stack, (j_object*) pIdentifier);
  return rc;
}

J_STATUS vm_execute_load_integer(vm_desc* pVM, uint32_t value)
{
  J_STATUS rc;
  rc = J_ERROR;
  j_object_integer* pInteger;

  pInteger = j_object_integer_new(value);
  rc = stack_push(&pVM->stack, (j_object*) pInteger);
  return rc;
}

J_STATUS vm_execute_load_double(vm_desc* pVM, double value)
{
  J_STATUS rc;
  rc = J_ERROR;
  j_object_double* pDouble;

  pDouble = j_object_double_new(value);
  rc = stack_push(&pVM->stack, (j_object*) pDouble);
  return rc;
}

J_STATUS vm_execute_computation(vm_desc* pVM, char c)
{
  J_STATUS rc;
  rc = J_ERROR;

  j_object* pLeft;
  j_object* pRight;
  j_object* pResult;

  pRight = stack_top_n(&pVM->stack, 0);
  pLeft  = stack_top_n(&pVM->stack, -1);

  pResult = j_object_doOperation(pLeft, pRight, c);
  if (pResult != NULL)
  {
    rc = J_OK;

    j_object_delete(pLeft);
    j_object_delete(pRight);

    stack_pop(&pVM->stack);
    stack_pop(&pVM->stack);

    stack_push(&pVM->stack, pResult);
  }

  return rc;
}


J_STATUS vm_execute_array_get_at(vm_desc* pVM)
{
  J_STATUS rc;
  j_object* pIdentifier;
  j_object* pOffset;
  j_object* pResult;
  j_value vOffset;
  BOOL bOk;

  rc = J_ERROR;
  pResult = NULL;

  pIdentifier = stack_top_n(&pVM->stack, 0);
  pOffset = stack_top_n(&pVM->stack, -1);

  bOk = j_object_getValue(pOffset, &vOffset);
  if ((bOk == TRUE) && (vOffset.type == J_INT))
  {
    pResult = j_object_getAtIndex(pIdentifier, vOffset.value.type_int);
  }

  j_object_delete(pIdentifier);
  j_object_delete(pOffset);
  j_value_destroy(&vOffset);

  stack_pop(&pVM->stack);
  stack_pop(&pVM->stack);
  if (pResult != NULL)
  {
    rc = J_OK;
    stack_push(&pVM->stack, pResult);
  }

  return rc;
}


J_STATUS vm_execute_create_symbol(vm_desc* pVM, uint32_t offset)
{
  J_STATUS rc;
  char* IdentifierName;
  rc = J_ERROR;
  IdentifierName = (char*) &pVM->pData->pSection[offset];

  rc = param_insert(IdentifierName, J_INT, 0);
  return rc;
}


J_STATUS vm_execute_delete_symbol(vm_desc* pVM, uint32_t offset)
{
  char* IdentifierName;
  IdentifierName = (char*) &pVM->pData->pSection[offset];
  param_delete(IdentifierName);
  return J_OK;
}

//  object in stack for a FOR statement :
//  --------
//  |      | IDENTIFIER (index param)
//  --------
//  |      | SEQUENCED_OBJ
//  --------
//  |      | ITERATOR
//  --------  <--- top stack

J_STATUS vm_execute_create_iterator(vm_desc* pVM)
{
  J_STATUS rc;
  j_object* p;
  j_object* pSequenced;
  rc = J_ERROR;

  pSequenced = stack_top(&pVM->stack);
  p = (j_object*) j_object_iterator_new(pSequenced);
  if (p != NULL)
  {
    rc = stack_push(&pVM->stack, p);
  }

  return rc;
}

J_STATUS vm_execute_iterator_load_first(vm_desc* pVM)
{
  J_STATUS rc;
  rc = J_ERROR;
  j_object* pResult;

  j_object_iterator* pIterator = (j_object_iterator*) stack_top(&pVM->stack);

  pResult = j_object_iterator_get_first(pIterator);
  if (pResult != NULL)
  {
    rc = J_OK;
    stack_push(&pVM->stack, pResult);
  }

  return rc;
}

J_STATUS vm_execute_iterator_load_next(vm_desc* pVM)
{
  J_STATUS rc;
  rc = J_ERROR;
  j_object* pResult;

  j_object_iterator* pIterator = (j_object_iterator*) stack_top(&pVM->stack);

  pResult = j_object_iterator_get_next(pIterator);
  if (pResult != NULL)
  {
    rc = J_OK;
    stack_push(&pVM->stack, pResult);
  }

  return rc;
}



J_STATUS vm_execute_set_symbol(vm_desc* pVM, uint32_t offset)
{
  J_STATUS rc;
  char* identifierName;
  identifierName = (char*) &pVM->pData->pSection[offset];
  rc = J_ERROR;

  j_object* pTop = stack_top(&pVM->stack);
  if (pTop != NULL)
  {
    j_value v;
    j_object_getValue(pTop, &v);
    rc = param_update(identifierName, &v);
    j_value_destroy(&v);
  }

  return rc;
}


J_STATUS vm_execute_iterator_end_reached(vm_desc* pVM)
{
  J_STATUS rc;
  rc = J_ERROR;
  j_object* pResult;
  BOOL b;

  j_object_iterator* pIterator = (j_object_iterator*) stack_top(&pVM->stack);

  b = j_object_iterator_isFinished(pIterator);
  if (b == TRUE)
  {
    pResult = (j_object*) j_object_boolean_new(TRUE);
  }
  else
  {
    pResult = (j_object*) j_object_boolean_new(FALSE);
  }

  if (pResult != NULL)
  {
    rc = J_OK;
    stack_push(&pVM->stack, pResult);
  }

  return rc;
}

J_STATUS vm_execute_iterator_increment(vm_desc* pVM)
{
  j_object_iterator* pIterator = (j_object_iterator*) stack_top(&pVM->stack);

  j_object_iterator_increment(pIterator);
  return 0;
}


J_STATUS vm_execute_branch_if_true(vm_desc* pVM, uint32_t offset)
{
  j_object* top = stack_top(&pVM->stack);

  BOOL b = j_object_toBoolean(top);
  if (b == TRUE)
  {
    pVM->PC = offset;
  }

  j_object_delete(top);
  stack_pop(&pVM->stack);

  return 0;
}

J_STATUS vm_execute_branch_if_false(vm_desc* pVM, uint32_t offset)
{
  j_object* top = stack_top(&pVM->stack);

  BOOL b = j_object_toBoolean(top);
  if (b == FALSE)
  {
    pVM->PC = offset;
  }

  j_object_delete(top);
  stack_pop(&pVM->stack);

  return 0;
}


J_STATUS vm_execute_condition(vm_desc* pVM, j_condition c)
{
  J_STATUS rc;
  rc = J_ERROR;

  j_object* pLeft;
  j_object* pRight;
  j_object* pResult;

  pRight = stack_top_n(&pVM->stack, 0);
  pLeft  = stack_top_n(&pVM->stack, -1);

  pResult = j_object_doCondition(pLeft, pRight, c);
  if (pResult != NULL)
  {
    rc = J_OK;

    j_object_delete(pLeft);
    j_object_delete(pRight);

    stack_pop(&pVM->stack);
    stack_pop(&pVM->stack);

    stack_push(&pVM->stack, pResult);
  }

  return rc;
}

J_STATUS vm_execute_logical_condition(vm_desc* pVM, j_logical_condition c)
{
  J_STATUS rc;
  rc = J_ERROR;

  j_object* pLeft;
  j_object* pRight;
  j_object* pResult;
  BOOL bResult;
  BOOL bLeft;
  BOOL bRight;

  switch (c)
  {
    case COND_LOG_NOT:
      pLeft  = stack_top_n(&pVM->stack, 0);
      bResult = !j_object_toBoolean(pLeft);
      j_object_delete(pLeft);
      stack_pop(&pVM->stack);
      rc = J_OK;
      break;

    case COND_LOG_IS:
      pLeft  = stack_top_n(&pVM->stack, 0);
      bResult = FALSE; //TODO
      j_object_delete(pLeft);
      stack_pop(&pVM->stack);
      break;

    case COND_LOG_AND:
      pRight = stack_top_n(&pVM->stack, 0);
      pLeft  = stack_top_n(&pVM->stack, -1);

      bLeft = j_object_toBoolean(pLeft);
      bRight = j_object_toBoolean(pRight);

      bResult = bLeft && bRight;

      j_object_delete(pLeft);
      j_object_delete(pRight);

      stack_pop(&pVM->stack);
      stack_pop(&pVM->stack);
      rc = J_OK;
      break;

    case COND_LOG_OR:
      pRight = stack_top_n(&pVM->stack, 0);
      pLeft  = stack_top_n(&pVM->stack, -1);

      bLeft = j_object_toBoolean(pLeft);
      bRight = j_object_toBoolean(pRight);

      bResult = bLeft || bRight;

      j_object_delete(pLeft);
      j_object_delete(pRight);

      stack_pop(&pVM->stack);
      stack_pop(&pVM->stack);
      rc = J_OK;
      break;

    default:
      bResult = FALSE;
      break;
  }

  pResult = (j_object*) j_object_boolean_new(bResult);
  stack_push(&pVM->stack, pResult);
  return rc;
}


J_STATUS vm_execute_create_array(vm_desc* pVM, BOOL isTuple)
{
  J_STATUS rc;
  int32_t i;
  int32_t nbItems;
  j_object* pObject;
  rc = J_OK;

  j_object_array* pArray = j_object_array_new(isTuple);

  pObject = stack_top(&pVM->stack);
  nbItems = j_object_toInteger(pObject);
  j_object_delete(pObject);
  stack_pop(&pVM->stack);

  for (i = 0; ((i < nbItems) && (rc == 0)); i++)
  {
    pObject = stack_top(&pVM->stack);
    rc = j_object_array_insert(pArray, pObject);
    stack_pop(&pVM->stack);
  }

  stack_push(&pVM->stack, (j_object*) pArray);

  return rc;
}
