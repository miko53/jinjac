/*
 * Copyright (c) 2019 miko53
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "common.h"
#include "buildin.h"
#include "jobject.h"
#include "jfunction.h"
#include "jlist.h"

#define MAX_OBJECT (50)

typedef struct
{
  BOOL inError;
  char* currentStringValue;
  char* error;
  uint32_t ast_nb_object;
  JObject* ast_list[MAX_OBJECT];
} ast;

STATIC ast ast_root;

STATIC void ast_remove_last(BOOL toDelete);

void ast_removeLastResultItem(void)
{
  ast_remove_last(TRUE);
}

void ast_init()
{
  ast_clean();
}

void ast_clean()
{
  ast_root.inError = FALSE;
  if (ast_root.currentStringValue != NULL)
  {
    free(ast_root.currentStringValue);
    ast_root.currentStringValue = NULL;
  }

  for (uint32_t i = 0; i < ast_root.ast_nb_object; i++)
  {
    JObject_delete(ast_root.ast_list[i]);
    ast_root.ast_list[i] = NULL;
  }
  ast_root.ast_nb_object = 0;
}

void ast_setInError(char* errorString)
{
  ast_root.error = errorString;
  ast_root.inError = TRUE;
}

char* ast_getErrorDetails()
{
  if (ast_root.error != NULL)
  {
    return ast_root.error;
  }

  return "";
}


ast_status ast_getStatus(void)
{
  BOOL inError;
  ast_status status;
  inError = ast_root.inError;

  status = IN_ERROR;
  if (!inError)
  {
    if (ast_root.ast_nb_object >= 1)
    {
      JObject* top = ast_root.ast_list[ast_root.ast_nb_object - 1];
      switch (top->type)
      {
        case J_FOR:
          status = FOR_STATEMENT;
          break;

        case J_END_FOR:
          status = END_FOR_STATEMENT;
          break;

        case J_IF:
          status = IF_STATEMENT;
          break;

        case J_ELSE:
          status = ELSE_STATEMENT;
          break;

        case J_END_IF:
          status = END_IF_STATEMENT;
          break;

        default:
          status = OK_DONE;
          break;
      }
    }
  }

  return status;
}

BOOL ast_setBeginOfForStatement(int64_t offset, int32_t noLine, BOOL bStripWhiteSpace)
{
  BOOL bOk;
  bOk = FALSE;
  if (ast_root.ast_nb_object >= 1)
  {
    JObject* top = ast_root.ast_list[ast_root.ast_nb_object - 1];
    if (top->type == J_FOR)
    {
      JFor_setStartPoint((JFor*) top, offset, noLine, bStripWhiteSpace);
      bOk = TRUE;
    }
  }

  return bOk;
}



J_STATUS ast_insert(JObject* o)
{
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object < MAX_OBJECT)
  {
    ast_root.ast_list[ast_root.ast_nb_object] = o;
    ast_root.ast_nb_object++;
    rc = J_OK;
  }

  return rc;
}

J_STATUS ast_insert_constante(char* name)
{
  JObject* o = JStringConstante_new(name);
  return ast_insert(o);
}

J_STATUS ast_insert_identifier(char* name)
{
  JObject* o = JIdentifier_new(name);
  return ast_insert(o);
}

J_STATUS ast_insert_integer(int32_t i)
{
  JObject* o = JInteger_new(i);
  return ast_insert(o);
}

J_STATUS ast_insert_double(double d)
{
  JObject* o = JDouble_new(d);
  return ast_insert(o);
}

J_STATUS ast_insert_boolean(BOOL b)
{
  JObject* o = JBoolean_new(b);
  return ast_insert(o);
}


J_STATUS ast_insert_function(char* fct)
{
  JObject* top;
  top = NULL;

  if (ast_root.ast_nb_object >= 1)
  {
    top = ast_root.ast_list[ast_root.ast_nb_object - 1];
    if (top->type != J_FUNCTION_ARGS)
    {
      top = NULL;
    }
  }

  JObject* o = JFunction_new(fct);
  if (o != NULL)
  {
    if (top != NULL)
    {
      //it is a argument, put in function first and remove from top
      ast_remove_last(FALSE);
    }
    ((JFunction*) o)->argList = (JArgs*) top;
    return ast_insert(o);
  }

  return J_ERROR;
}

J_STATUS ast_insert_array(char* name, int32_t offset)
{
  J_STATUS rc;
  JObject* o;

  rc = J_ERROR;
  o = JArray_new(name, offset);
  if (o != NULL)
  {
    rc = ast_insert(o);
  }

  return rc;
}

STATIC void ast_remove_last(BOOL toDelete)
{
  if (toDelete)
  {
    JObject_delete(ast_root.ast_list[ast_root.ast_nb_object - 1]);
  }

  ast_root.ast_list[ast_root.ast_nb_object - 1] = NULL;
  ast_root.ast_nb_object--;
}


J_STATUS ast_execute_function()
{
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object >= 1)
  {
    if (ast_root.ast_list[ast_root.ast_nb_object - 1]->type == J_FUNCTION)
    {
      JFunction* f = (JFunction*) ast_root.ast_list[ast_root.ast_nb_object - 1];
      JObject* resultObj;

      resultObj = JFunction_execute(f, NULL);
      if (resultObj != NULL)
      {
        ast_remove_last(TRUE);
        ast_insert(resultObj);
        rc = J_OK;
      }
    }
  }

  return rc;
}


J_STATUS ast_execute_filtered_function(void)
{
  J_STATUS rc;
  JObject* pConcernedObject;

  pConcernedObject = NULL;
  rc = J_ERROR;

  if (ast_root.ast_nb_object >= 2)
  {
    pConcernedObject = ast_root.ast_list[ast_root.ast_nb_object - 2];

    if (ast_root.ast_list[ast_root.ast_nb_object - 1]->type == J_FUNCTION)
    {
      JFunction* f = (JFunction*) ast_root.ast_list[ast_root.ast_nb_object - 1];
      JObject* resultObj;

      resultObj = JFunction_execute(f, pConcernedObject);
      if (resultObj != NULL)
      {
        if (pConcernedObject != NULL)
        {
          ast_remove_last(TRUE);
        }

        ast_remove_last(TRUE);
        ast_insert(resultObj);
        rc = J_OK;
      }
    }
  }

  return rc;
}



char* ast_getStringResult()
{
  char* s;
  s = NULL;
  if (ast_root.currentStringValue != NULL)
  {
    free(ast_root.currentStringValue);
    ast_root.currentStringValue = NULL;
  }

  if (ast_root.ast_nb_object >= 1)
  {
    s = JObject_toString(ast_root.ast_list[ast_root.ast_nb_object - 1]);
    ast_root.currentStringValue = s;
  }

  return s;
}

BOOL ast_get_offset(JObject* pObject, int32_t* pOffset)
{
  ASSERT(pObject != NULL);
  ASSERT(pOffset != NULL);

  *pOffset = JObject_toInteger(pObject);
  return TRUE;
}

J_STATUS ast_create_array_on_top(char* name)
{
  if (ast_root.ast_nb_object != 0)
  {
    int32_t offset;
    BOOL b;
    b = ast_get_offset(ast_root.ast_list[ast_root.ast_nb_object - 1], &offset);
    if (b)
    {
      ast_remove_last(TRUE);
      return ast_insert_array(name, offset);
    }
  }

  return J_ERROR;
}



// function called when a argument is detected by yacc, the first argument is in
// stack, it must be removed and inserted in a special JArgs object
J_STATUS ast_create_function_args_from_top(void)
{
  JObject* firstArgs;
  ASSERT(ast_root.ast_nb_object >= 1);
  firstArgs = ast_root.ast_list[ast_root.ast_nb_object - 1];
  ast_remove_last(FALSE); // top object will be inserted in JArgs object that's why not deleted

  JArgs* args = (JArgs*) JArgs_new();
  JArgs_insert_args(args, firstArgs);

  return ast_insert((JObject*) args);
}

J_STATUS ast_insert_function_args()
{
  JObject* arg;
  ASSERT(ast_root.ast_nb_object >= 2);
  arg = ast_root.ast_list[ast_root.ast_nb_object - 1];

  ASSERT(ast_root.ast_list[ast_root.ast_nb_object - 2]->type == J_FUNCTION_ARGS); // normally assured by parser
  JArgs* args = (JArgs*) ast_root.ast_list[ast_root.ast_nb_object - 2];
  ast_remove_last(FALSE); // top object will be inserted in JArgs object that's why not deleted

  return JArgs_insert_args(args, arg);
}


J_STATUS ast_do_operation(char mathOperation)
{
  //perform mathematical operation on last two items of the stack
  //remove them and insert result after.
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object >= 2)
  {
    JObject* op1 = ast_root.ast_list[ast_root.ast_nb_object - 2];
    JObject* op2 = ast_root.ast_list[ast_root.ast_nb_object - 1];

    JObject* result = JObject_doOperation(op1, op2, mathOperation);
    if (result != NULL)
    {
      ast_remove_last(TRUE);
      ast_remove_last(TRUE);
      ast_insert(result);
      rc = J_OK;
    }
    else
    {
      ast_setInError("Calcul not possible");
    }
  }
  else
  {
    ast_setInError("Nb operand not in according with calcul");
  }

  return rc;
}

J_STATUS ast_do_condition(jobject_condition condition)
{
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object >= 2)
  {
    JObject* op1 = ast_root.ast_list[ast_root.ast_nb_object - 2];
    JObject* op2 = ast_root.ast_list[ast_root.ast_nb_object - 1];
    JObject* result = JObject_execComparison(op1, op2, condition);
    if (result != NULL)
    {
      ast_remove_last(TRUE);
      ast_remove_last(TRUE);
      ast_insert(result);
      rc = J_OK;
    }
    else
    {
      ast_setInError("Calcul not possible");
    }
  }
  else
  {
    ast_setInError("Not enought operand to do the comparison...");
  }

  return rc;
}


J_STATUS ast_do_logical_condition(jobject_logical_condition condition)
{
  J_STATUS rc;
  JObject* pResult;
  BOOL bResult;

  bResult = FALSE;
  rc = J_ERROR;

  if (condition != AST_NOT)
  {
    if (ast_root.ast_nb_object >= 2)
    {
      JObject* op1 = ast_root.ast_list[ast_root.ast_nb_object - 2];
      JObject* op2 = ast_root.ast_list[ast_root.ast_nb_object - 1];

      switch (condition)
      {
        case AST_AND:
          bResult = JObject_toBoolean(op1) && JObject_toBoolean(op2);
          break;

        case AST_OR:
          bResult = JObject_toBoolean(op1) || JObject_toBoolean(op2);
          break;

        default:
          ASSERT(FALSE);
          break;
      }

      pResult = JBoolean_new(bResult);
      if (pResult != NULL)
      {
        ast_remove_last(TRUE);
        ast_remove_last(TRUE);
        ast_insert(pResult);
        rc = J_OK;
      }
    }
    else
    {
      trace("not possible\n");
      ASSERT(FALSE);
    }
  }
  else
  {
    if (ast_root.ast_nb_object >= 1)
    {
      JObject* op1 = ast_root.ast_list[ast_root.ast_nb_object - 1];
      bResult = !JObject_toBoolean(op1);
      pResult = JBoolean_new(bResult);
      ast_remove_last(TRUE);
      ast_insert(pResult);
      rc = J_OK;
    }
    else
    {
      trace("not possible\n");
      ASSERT(FALSE);
    }
  }

  return rc;
}


//used with only one condition
J_STATUS ast_convert_to_condition(void)
{
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object >= 1)
  {
    JObject* pObj = ast_root.ast_list[ast_root.ast_nb_object - 1];
    JObject* pResult;
    BOOL bResult;

    bResult = JObject_toBoolean(pObj);
    pResult = JBoolean_new(bResult);
    if (pResult != NULL)
    {
      ast_remove_last(TRUE);
      ast_insert(pResult);
      rc = J_OK;
    }
  }

  return rc;
}


J_STATUS ast_create_for_stmt(char* identifierName)
{
  J_STATUS rc;
  JObject* o;
  JRange* range;
  rc = J_ERROR;

  ASSERT(ast_root.ast_nb_object >= 1);

  //TODO should be removed to put a J_RANGE conversion instead
  if (ast_root.ast_list[ast_root.ast_nb_object - 1]->type != J_RANGE)
  {
    range = JObject_toRange(ast_root.ast_list[ast_root.ast_nb_object - 1]);
  }
  else
  {
    range = (JRange*) ast_root.ast_list[ast_root.ast_nb_object - 1];
  }

  ast_remove_last(FALSE); // top object will be inserted in JFor object that's why not deleted

  o = JFor_new(identifierName, range);
  if (o != NULL)
  {
    JFor_createIndexParameter((JFor*) o);
    rc = ast_insert(o);
  }

  return rc;
}

BOOL ast_forStmtIsLineToBeIgnored(void)
{
  BOOL bLineNeedToBeIgnored;
  bLineNeedToBeIgnored = FALSE;

  if (ast_root.ast_nb_object >= 1)
  {
    ASSERT(ast_root.ast_list[ast_root.ast_nb_object - 1]->type == J_FOR);

    JFor* pFor = (JFor*) (ast_root.ast_list[ast_root.ast_nb_object - 1]);

    bLineNeedToBeIgnored = JFor_isDone(pFor);
  }

  return bLineNeedToBeIgnored;
}


J_STATUS ast_create_end_for_stmt()
{
  J_STATUS rc;
  JObject* o;
  rc = J_ERROR;

  o = JEndFor_new();
  if (o != NULL)
  {
    rc = ast_insert(o);
  }

  return rc;
}

BOOL ast_executeEndForStmt(int64_t* returnOffset, int32_t* previousLine, BOOL* bStripWhiteSpace)
{
  BOOL bOK;
  bOK = FALSE;
  ASSERT(returnOffset != NULL);

  if (ast_root.ast_nb_object >= 2)
  {
    JObject* o = ast_root.ast_list[ast_root.ast_nb_object - 2];
    if (o->type == J_FOR)
    {
      JFor* pForStmt = (JFor*) o;
      BOOL isDone;
      isDone = JRange_step(pForStmt->sequencing, pForStmt->identifierOfIndex);
      if (isDone)
      {
        *returnOffset = -1;
        //parameter_delete(pForStmt->identifierOfIndex);
      }
      else
      {
        *returnOffset = pForStmt->startOffset;
        *previousLine = pForStmt->startLine;
        *bStripWhiteSpace = pForStmt->bStripWhiteSpace;
      }
      bOK = TRUE;
    }
  }

  //trace("ast_executeEndForStmt = %d\n", bOK);
  return bOK;
}


J_STATUS ast_create_if_stmt(void)
{
  J_STATUS rc;
  JObject* o;
  JObject* pResult;
  BOOL bResult;

  rc = J_ERROR;

  ASSERT(ast_root.ast_nb_object >= 1); //normally assured by the grammar
  JObject* pObj = ast_root.ast_list[ast_root.ast_nb_object - 1];

  if (ast_root.ast_list[ast_root.ast_nb_object - 1]->type != J_BOOLEAN)
  {
    bResult = JObject_toBoolean(pObj);
    pResult = JBoolean_new(bResult);
    pObj = pResult;
    ast_remove_last(TRUE);
  }
  else
  {
    ast_remove_last(FALSE); //not delete because inserted with the IF statement
  }

  o = JIF_new(pObj);
  if (o != NULL)
  {
    rc = ast_insert(o);
  }

  return rc;
}


BOOL ast_ifStmtIsLineToBeIgnored(void)
{
  ASSERT(ast_root.ast_nb_object >= 1);
  ASSERT(ast_root.ast_list[ast_root.ast_nb_object - 1]->type == J_IF);
  BOOL bLineNeedToBeIgnored;

  JIF* pIF = (JIF*) (ast_root.ast_list[ast_root.ast_nb_object - 1]);

  bLineNeedToBeIgnored = !JIF_getIfConditionIsActive(pIF);

  return bLineNeedToBeIgnored;
}


J_STATUS ast_create_end_if_stmt(void)
{
  J_STATUS rc;
  JObject* o;
  rc = J_ERROR;

  o = JEndIf_new();
  if (o != NULL)
  {
    rc = ast_insert(o);
  }

  return rc;
}

J_STATUS ast_create_else_stmt(void)
{
  J_STATUS rc;
  JObject* o;
  rc = J_ERROR;

  o = JElse_new();
  if (o != NULL)
  {
    rc = ast_insert(o);
  }

  return rc;
}


J_STATUS ast_create_list_on_top()
{
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object != 0)
  {
    JObject* firstItemOfList = ast_root.ast_list[ast_root.ast_nb_object - 1];
    ast_remove_last(FALSE);

    JList* l = (JList*) JList_new();
    if (l != NULL)
    {
      rc = JList_insert(l, firstItemOfList);
      if (rc == J_OK)
      {
        rc = ast_insert((JObject*) l);
      }
    }
  }

  return rc;
}

J_STATUS ast_list_insert_item()
{
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object >= 2)
  {
    JList* l = (JList*) ast_root.ast_list[ast_root.ast_nb_object - 2];
    ASSERT(l->base.type == J_LIST);
    JObject* nextItemOfList = ast_root.ast_list[ast_root.ast_nb_object - 1];
    ast_remove_last(FALSE);
    rc = JList_insert(l, nextItemOfList);
  }

  return rc;
}


J_STATUS ast_set_list_type(BOOL isTuple)
{
  J_STATUS rc;
  rc = J_ERROR;

  if (ast_root.ast_nb_object != 0)
  {
    JList* list = (JList*) ast_root.ast_list[ast_root.ast_nb_object - 1];

    if (list->base.type == J_LIST)
    {
      JList_setTuple(list, isTuple);
      rc = J_OK;
    }
  }

  return rc;
}


char* ast_getTypeString(jobject_type type)
{
  char* s = NULL;

  switch (type)
  {
    case J_STR_CONSTANTE:
      s = "String";
      break;

    case J_INTEGER:
      s = "Integer";
      break;

    case J_DOUBLE:
      s = "Double";
      break;

    case J_IDENTIFIER:
      s = "Identifier";
      break;

    case J_ARRAY:
      s = "Array";
      break;

    case J_FUNCTION_ARGS:
      s = "Function Argument";
      break;

    case J_FUNCTION:
      s = "Function";
      break;

    case J_BOOLEAN:
      s = "Boolean";
      break;

    case J_RANGE:
      s = "Range";
      break;

    case J_FOR:
      s = "For";
      break;

    case J_END_FOR:
      s = "End For";
      break;

    case J_IF:
      s = "If";
      break;

    case J_END_IF:
      s = "End If";
      break;

    case J_ELSE:
      s = "Else";
      break;

    case J_LIST:
      s = "List (array/tuple)";
      break;

    default:
      ASSERT(FALSE);
      break;

  }
  return s;
}

void display_function_args(JArgs* argsObj)
{
  int32_t indexArgs;
  for (indexArgs = 0; indexArgs < argsObj->nb_args; indexArgs++)
  {
    trace( "--> arg[%d]: \"%s\" (%d)\n", indexArgs,
           ast_getTypeString(argsObj->listArgs[indexArgs]->type),
           argsObj->listArgs[indexArgs]->type);
  }
}


void display_range(JRange* range)
{
  (void) range;
  trace( "--> object associed (%p)\n", range->sequencedObject);
  trace( "--> current index (%d)\n", range->currentIndex);
  trace( "--> start (%d), stop (%d), step(%d)\n", range->start, range->stop, range->step);
}

void display_list(JList* list)
{
  int32_t index;
  index = 0;
  JListItem* item;

  item = list->list;
  while (item != NULL)
  {
    trace( "--> item %d: (type %d)\n", index, item->object->type);
    item = item->next;
    index++;
  }

}

void ast_dump_stack()
{
  uint32_t i;

  trace( "---------- begin ast stack\n");
  trace( "nb item: %u\n", ast_root.ast_nb_object);
  for (i = 0; i < ast_root.ast_nb_object; i++)
  {
    trace( "item[%u]: \"%s\" (%d)\n", i, ast_getTypeString(ast_root.ast_list[i]->type), ast_root.ast_list[i]->type);
    switch (ast_root.ast_list[i]->type)
    {
      case J_STR_CONSTANTE:
        trace( "-> \"%s\"\n", ((JStringConstante*) ast_root.ast_list[i])->str_constant);
        break;

      case J_INTEGER:
        trace( "-> \"%d\"\n", ((JInteger*) ast_root.ast_list[i])->value);
        break;

      case J_DOUBLE:
        trace( "-> \"%f\"\n", ((JDouble*) ast_root.ast_list[i])->value);
        break;

      case J_BOOLEAN:
        trace( "-> \"%d\"\n", ((JBoolean*) ast_root.ast_list[i])->value);
        break;

      case J_IDENTIFIER:
        trace( "-> \"%s\"\n", ((JIdentifier*) ast_root.ast_list[i])->identifier);
        break;

      case J_ARRAY:
        trace( "-> \"%s\"\n", ((JArray*) ast_root.ast_list[i])->identifier);
        break;

      case J_FUNCTION_ARGS:
        display_function_args((JArgs*) ast_root.ast_list[i]);
        break;

      case J_FUNCTION:
        if (((JFunction*) ast_root.ast_list[i])->argList != NULL)
        {
          trace( "-> Begin inner args:\n");
          display_function_args(((JFunction*) ast_root.ast_list[i])->argList);
          trace( "-> End inner args\n");
        }
        else
        {
          trace( "-> no args\n");
        }
        break;

      case J_FOR:
        {
          trace( ": iterator \"%s\" return Point %ld\n",
                 ((JFor*) ast_root.ast_list[i])->identifierOfIndex,
                 ((JFor*) ast_root.ast_list[i])->startOffset);
          display_range(((JFor*) ast_root.ast_list[i])->sequencing);
        }
        break;

      case J_IF:
      case J_END_FOR:
      case J_END_IF:
        break;

      case J_LIST:
        display_list((JList*) ast_root.ast_list[i]);
        break;

      default:
        break;
    }
  }

  trace( "---------- end ast stack\n");
}

