#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "common.h"
#include "buildin.h"
#include "jobject.h"
#include "jfunction.h"

#define MAX_OBJECT (50)

typedef struct
{
  BOOL inError;
  char* currentStringValue;
  unsigned int ast_nb_object;
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

  for (unsigned int i = 0; i < ast_root.ast_nb_object; i++)
  {
    JObject_delete(ast_root.ast_list[i]);
    ast_root.ast_list[i] = NULL;
  }
  ast_root.ast_nb_object = 0;
}

void ast_setInError(char* errorString)
{
  (void) errorString;
  ast_root.inError = TRUE;
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

        default:
          status = OK_DONE;
          break;
      }
    }
  }

  return status;
}


BOOL ast_getInError(void)
{
  return ast_root.inError;
}

BOOL ast_setBeginOfForStatement(long offset)
{
  BOOL bOk;
  bOk = FALSE;
  if (ast_root.ast_nb_object >= 1)
  {
    JObject* top = ast_root.ast_list[ast_root.ast_nb_object - 1];
    if (top->type == J_FOR)
    {
      JFor_setStartPoint((JFor*) top, offset);
      bOk = TRUE;
    }
  }

  return bOk;
}



int ast_insert(JObject* o)
{
  int rc;
  rc = -1;

  if (ast_root.ast_nb_object < MAX_OBJECT)
  {
    ast_root.ast_list[ast_root.ast_nb_object] = o;
    ast_root.ast_nb_object++;
    rc = 0;
  }

  return rc;
}

int ast_insert_constante(char* name)
{
  JObject* o = JStringConstante_new(name);
  return ast_insert(o);
}

int ast_insert_identifier(char* name)
{
  JObject* o = JIdentifier_new(name);
  return ast_insert(o);
}

int ast_insert_integer(int i)
{
  JObject* o = JInteger_new(i);
  return ast_insert(o);
}

int ast_insert_double(double d)
{
  JObject* o = JDouble_new(d);
  return ast_insert(o);
}

int ast_insert_boolean(BOOL b)
{
  JObject* o = JBoolean_new(b);
  return ast_insert(o);
}


int ast_insert_function(char* fct)
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

  return -1;
}

int ast_insert_array(char* name, int offset)
{
  int rc;
  JObject* o;

  rc = -1;
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


int ast_execute_function()
{
  int rc;
  JObject* pConcernedObject;

  pConcernedObject = NULL;
  rc = -1;

  if (ast_root.ast_nb_object >= 1)
  {
    if (ast_root.ast_nb_object >= 2)
    {
      pConcernedObject = ast_root.ast_list[ast_root.ast_nb_object - 2];
    }

    ASSERT(ast_root.ast_list[ast_root.ast_nb_object - 1]->type == J_FUNCTION);//TODO check error instead ...

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
      rc = 0;
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

BOOL ast_get_offset(JObject* pObject, int* pOffset)
{
  ASSERT(pObject != NULL);
  ASSERT(pOffset != NULL);

  *pOffset = JObject_toInteger(pObject);
  return TRUE;
}

int ast_create_array_on_top(char* name)
{
  if (ast_root.ast_nb_object != 0)
  {
    int offset;
    BOOL b;
    b = ast_get_offset(ast_root.ast_list[ast_root.ast_nb_object - 1], &offset);
    if (b)
    {
      ast_remove_last(TRUE);
      return ast_insert_array(name, offset);
    }
  }

  return -1;
}



// function called when a argument is detected by yacc, the first argument is in
// stack, it must be removed and inserted in a special JArgs object
int ast_create_function_args_from_top(void)
{
  JObject* firstArgs;
  ASSERT(ast_root.ast_nb_object >= 1);
  firstArgs = ast_root.ast_list[ast_root.ast_nb_object - 1];
  ast_remove_last(FALSE); // top object will be inserted in JArgs object that's why not deleted

  JArgs* args = (JArgs*) JArgs_new();
  JArgs_insert_args(args, firstArgs);

  return ast_insert((JObject*) args);
}

int ast_insert_function_args()
{
  JObject* arg;
  ASSERT(ast_root.ast_nb_object >= 2);
  arg = ast_root.ast_list[ast_root.ast_nb_object - 1];

  ASSERT(ast_root.ast_list[ast_root.ast_nb_object - 2]->type == J_FUNCTION_ARGS);
  JArgs* args = (JArgs*) ast_root.ast_list[ast_root.ast_nb_object - 2];
  ast_remove_last(FALSE); // top object will be inserted in JArgs object that's why not deleted

  return JArgs_insert_args(args, arg);
}


int ast_do_operation(char mathOperation)
{
  //perform mathematical operation on last two items of the stack
  //remove them and insert result after.
  int rc;
  rc = -1;

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
      rc = 0;
    }
    else
    {
      ast_setInError("Calcul not possible");
    }
  }
  else
  {
    ast_setInError("Nb operande not in according with calcul");
  }

  return rc;
}


int ast_create_for_stmt(char* identifierName)
{
  int rc;
  JObject* o;
  JRange* range;
  rc = -1;

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
    JFor_createIndexParameter((JFor*) o); //TODO rc a prendre en compte
    rc = ast_insert(o);
  }

  return rc;
}

BOOL ast_forStmtIsLineToBeIgnored()
{
  ASSERT(ast_root.ast_nb_object >= 1);
  ASSERT(ast_root.ast_list[ast_root.ast_nb_object - 1]->type == J_FOR);
  BOOL bLineNeedToBeIgnored;

  JFor* pFor = (JFor*) (ast_root.ast_list[ast_root.ast_nb_object - 1]);

  bLineNeedToBeIgnored = JFor_isDone(pFor);

  return bLineNeedToBeIgnored;
}


int ast_create_end_for_stmt()
{
  int rc;
  JObject* o;
  rc = -1;

  o = JEndFor_new();
  if (o != NULL)
  {
    rc = ast_insert(o);
  }

  return rc;
}

BOOL ast_executeEndForStmt(long* returnOffset)
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
        parameter_delete(pForStmt->identifierOfIndex);
      }
      else
      {
        *returnOffset = pForStmt->startOffset;
      }
      bOK = TRUE;
    }
  }

  return bOK;
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

    default:
      ASSERT(FALSE);
      break;

  }
  return s;
}

void display_function_args(JArgs* argsObj)
{
  int indexArgs;
  trace("\n");
  for (indexArgs = 0; indexArgs < argsObj->nb_args; indexArgs++)
  {
    trace( " arg[%d]: \"%s\" (%d)\n", indexArgs,
           ast_getTypeString(argsObj->listArgs[indexArgs]->type),
           argsObj->listArgs[indexArgs]->type);
  }
}


void display_range(JRange* range)
{
  trace( "> object associed (%p)\n", range->sequencedObject);
  trace( "> current index (%d)\n", range->currentIndex);
  trace( "> start (%d), stop (%d), step(%d)\n", range->start, range->stop, range->step);
}


int ast_dump_stack()
{
  unsigned int i;

  trace( "---------- begin ast stack\n");
  trace( "nb item: %d\n", ast_root.ast_nb_object);
  for (i = 0; i < ast_root.ast_nb_object; i++)
  {
    trace( "item[%d]: \"%s\" (%d)", i, ast_getTypeString(ast_root.ast_list[i]->type), ast_root.ast_list[i]->type);
    switch (ast_root.ast_list[i]->type)
    {
      case J_STR_CONSTANTE:
        trace( ": \"%s\"\n", ((JStringConstante*) ast_root.ast_list[i])->str_constant);
        break;

      case J_INTEGER:
        trace( ": \"%d\"\n", ((JInteger*) ast_root.ast_list[i])->value);
        break;

      case J_DOUBLE:
        trace( ": \"%f\"\n", ((JDouble*) ast_root.ast_list[i])->value);
        break;

      case J_BOOLEAN:
        trace( ": \"%d\"\n", ((JBoolean*) ast_root.ast_list[i])->value);
        break;

      case J_IDENTIFIER:
        trace( ": \"%s\"\n", ((JIdentifier*) ast_root.ast_list[i])->identifier);
        break;

      case J_ARRAY:
        trace( ": \"%s\"\n", ((JArray*) ast_root.ast_list[i])->identifier);
        break;

      case J_FUNCTION_ARGS:
        display_function_args((JArgs*) ast_root.ast_list[i]);
        break;

      case J_FUNCTION:
        if (((JFunction*) ast_root.ast_list[i])->argList != NULL)
        {
          trace( "> Begin inner args:\n");
          display_function_args(((JFunction*) ast_root.ast_list[i])->argList);
          trace( "> End inner args\n");
        }
        else
        {
          trace( ">> no args\n");
        }
        break;

      case J_FOR:
        {
          trace( ": iterator \"%s\" return Point %ld\n",
                 ((JFor*) ast_root.ast_list[i])->identifierOfIndex,
                 ((JFor*) ast_root.ast_list[i])->startOffset);
          display_range(((JFor*) ast_root.ast_list[i])->sequencing);
        }

      case J_END_FOR:
        break;

      default:
        break;
    }
  }

  trace( "---------- end ast stack\n");
  return 0;
}

