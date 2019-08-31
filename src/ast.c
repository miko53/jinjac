#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "common.h"
#include "buildin.h"
#include "jobject.h"

#define MAX_OBJECT (50)

static ast ast_root;

static JObject* ast_list[MAX_OBJECT];
static unsigned int ast_nb_object;

static void ast_remove_last(BOOL toDelete);

void ast_clean()
{
  ast_root.inError = FALSE;
  if (ast_root.currentStringValue != NULL)
  {
    free(ast_root.currentStringValue);
    ast_root.currentStringValue = NULL;
  }

  for (unsigned int i = 0; i < ast_nb_object; i++)
  {
    JObject_delete(ast_list[i]);
    ast_list[i] = NULL;
  }
  ast_nb_object = 0;
}

ast* getAstRoot(void)
{
  return &ast_root;
}

int ast_insert(JObject* o)
{
  int rc;
  rc = -1;

  if (ast_nb_object < MAX_OBJECT)
  {
    ast_list[ast_nb_object] = o;
    ast_nb_object++;
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

  if (ast_nb_object >= 1)
  {
    top = ast_list[ast_nb_object - 1];
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
  JObject* o = JArray_new(name, offset);
  if (o != NULL)
  {
    return ast_insert(o);
  }

  return -1;
}



static void ast_remove_last(BOOL toDelete)
{
  if (toDelete)
  {
    JObject_delete(ast_list[ast_nb_object - 1]);
  }

  ast_list[ast_nb_object - 1] = NULL;
  ast_nb_object--;
}


char* ast_convert_to_string()
{
  char* s;
  s = NULL;
  if (ast_nb_object != 0)
  {
    s = JObject_toString(ast_list[ast_nb_object - 1]);
    ast_remove_last(TRUE);
  }

  return s;
}


char* ast_apply_filtering()
{
  char* s;
  s = NULL;
  if (ast_nb_object != 0)
  {
    ASSERT(ast_list[ast_nb_object - 1]->type == J_FUNCTION);

    JFunction* f = (JFunction*) ast_list[ast_nb_object - 1];
    s = JFunction_execute(f, getAstRoot()->currentStringValue);
    ast_remove_last(TRUE);
  }

  return s;
}

BOOL ast_get_offset(JObject* pObject, int* pOffset)
{
  ASSERT(pObject != NULL);
  ASSERT(pOffset != NULL);
  BOOL  b;
  b = FALSE;

  switch (pObject->type)
  {
    case J_IDENTIFIER:
      {
        JIdentifier* pIdent;
        pIdent = (JIdentifier*) pObject;
        parameter_type type = param_getType(pIdent->identifier);
        switch (type)
        {
          case TYPE_INT:
            *pOffset = param_getValue(pIdent->identifier).type_int;
            b = TRUE;
            break;

          case TYPE_STRING:
          case TYPE_DOUBLE:
          default:
            fprintf(stdout, "error: type '%s' can't be convert to integer\n", pIdent->identifier);
            break;
        }
      }

      break;

    case J_INTEGER:
      *pOffset = ((JInteger*) pObject)->value;
      b = TRUE;
      break;

    default:
      fprintf(stdout, "this object can give offset array\n");
      break;
  }

  if (b)
  {
    fprintf(stdout, "offset of array is %d\n", *pOffset);
  }

  return b;
}


int ast_create_array_on_top(char* name)
{
  if (ast_nb_object != 0)
  {
    int offset;
    BOOL b;
    b = ast_get_offset(ast_list[ast_nb_object - 1], &offset);
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
  ASSERT(ast_nb_object >= 1);
  firstArgs = ast_list[ast_nb_object - 1];
  ast_remove_last(FALSE); // top object will be inserted in JArgs object that's why not deleted

  JArgs* args = (JArgs*) JArgs_new();
  JArgs_insert_args(args, firstArgs);

  return ast_insert((JObject*) args);
}

int ast_insert_function_args()
{
  JObject* arg;
  ASSERT(ast_nb_object >= 2);
  arg = ast_list[ast_nb_object - 1];

  ASSERT(ast_list[ast_nb_object - 2]->type == J_FUNCTION_ARGS);
  JArgs* args = (JArgs*) ast_list[ast_nb_object - 2];
  ast_remove_last(FALSE); // top object will be inserted in JArgs object that's why not deleted

  return JArgs_insert_args(args, arg);
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

    default:
      ASSERT(FALSE);
      break;

  }
  return s;
}

void display_function_args(JArgs* argsObj)
{
  int indexArgs;
  fprintf(stdout, "\n");
  for (indexArgs = 0; indexArgs < argsObj->nb_args; indexArgs++)
  {
    fprintf(stdout, " arg[%d]: \"%s\" (%d)\n", indexArgs,
            ast_getTypeString(argsObj->listArgs[indexArgs]->type),
            argsObj->listArgs[indexArgs]->type);
  }
}


int ast_dump_stack()
{
  unsigned int i;

  fprintf(stdout, "---------- begin ast stack\n");
  fprintf(stdout, "nb item: %d\n", ast_nb_object);
  for (i = 0; i < ast_nb_object; i++)
  {
    fprintf(stdout, "item[%d]: \"%s\" (%d)", i, ast_getTypeString(ast_list[i]->type), ast_list[i]->type);
    switch (ast_list[i]->type)
    {
      case J_STR_CONSTANTE:
        fprintf(stdout, ": \"%s\"\n", ((JStringConstante*) ast_list[i])->str_constant);
        break;

      case J_INTEGER:
        fprintf(stdout, ": \"%d\"\n", ((JInteger*) ast_list[i])->value);
        break;

      case J_DOUBLE:
        fprintf(stdout, ": \"%f\"\n", ((JDouble*) ast_list[i])->value);
        break;

      case J_BOOLEAN:
        fprintf(stdout, ": \"%d\"\n", ((JBoolean*) ast_list[i])->value);
        break;

      case J_IDENTIFIER:
        fprintf(stdout, ": \"%s\"\n", ((JIdentifier*) ast_list[i])->identifier);
        break;

      case J_ARRAY:
        fprintf(stdout, ": \"%s\"\n", ((JArray*) ast_list[i])->identifier);
        break;

      case J_FUNCTION_ARGS:
        display_function_args((JArgs*) ast_list[i]);
        break;

      case J_FUNCTION:
        if (((JFunction*) ast_list[i])->argList != NULL)
        {
          fprintf(stdout, "> Begin inner args:\n");
          display_function_args(((JFunction*) ast_list[i])->argList);
          fprintf(stdout, "> End inner args\n");
        }
        else
        {
          fprintf(stdout, ">> no args\n");
        }
        break;

      default:
        break;
    }
  }

  fprintf(stdout, "---------- end ast stack\n");
  return 0;
}








