#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "ast.h"
#include "common.h"
#include "convert.h"
#include "buildin.h"

#define MAX_OBJECT (50)
#define NEW(obj)  malloc(sizeof(obj))

static ast ast_root;

static JObject* ast_list[MAX_OBJECT];
static unsigned int ast_nb_object;

static void JObject_delete(JObject* pObject);
static void ast_remove_last(BOOL toDelete);

void ast_clean()
{
  ast_root.inError = FALSE;
  if (ast_root.currentStringValue != NULL)
  {
    free(ast_root.currentStringValue );
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



typedef enum
{
  INT,
  DOUBLE,
  STRING,
  BOOLEAN
} args_type;

typedef struct
{
  filter_fct fct;
  const char* name;
  int nb_args;
  args_type args_type[NB_MAX_ARGS];
  void* args_default[NB_MAX_ARGS];
} fct_converter;


fct_converter tab_fct_converter[] =
{
  { .fct = (filter_fct) capitalize, .name = "capitalize", .nb_args = 0 },
  {
    .fct = (filter_fct) center, .name = "center", .nb_args = 1,
    .args_type = { INT},
    .args_default = { (void*) 80 }
  },
  { .fct = (filter_fct) format, .name = "format", .nb_args = NB_MAX_ARGS,  .args_type = { INT },  .args_default = { NULL } },
  { .fct = (filter_fct) lower, .name = "lower", .nb_args = 0 },
  { .fct = (filter_fct) upper, .name = "upper", .nb_args = 0 },
  { .fct = (filter_fct) title, .name = "title", .nb_args = 0 },
  { .fct = (filter_fct) trim, .name = "trim", .nb_args = 0},
  {
    .fct = (filter_fct) truncate, .name = "truncate", .nb_args = 4,
    .args_type = { INT, BOOLEAN, STRING, INT},
    .args_default = { (void*) 255,  (void*) FALSE, "...", 0 }
  },
};


int getFunction(char* fctName)
{
  int sizeMax = sizeof(tab_fct_converter) / sizeof(fct_converter);
  int i;

  for (i = 0; i < sizeMax; i++)
  {
    if (strcmp(tab_fct_converter[i].name, fctName) == 0)
    {
      return i;
    }
  }

  fprintf(stdout, "error: function %s not found !", fctName);
  return -1;
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

JObject* JStringConstante_new(char* name)
{
  JStringConstante* o = NEW(JStringConstante);
  o->base.type = J_STR_CONSTANTE;
  o->str_constant = name;
  return (JObject*) o;
}

JObject* JIdentifier_new(char* name)
{
  JIdentifier* o = NEW(JIdentifier);
  o->base.type = J_IDENTIFIER;
  o->identifier = name;
  return (JObject*) o;
}


JObject* JInteger_new(int i)
{
  JInteger* o = NEW(JInteger);
  o->base.type = J_INTEGER;
  o->value = i;
  return (JObject*) o;
}

JObject* JDouble_new(double d)
{
  JDouble* o = NEW(JDouble);
  o->base.type = J_DOUBLE;
  o->value = d;
  return (JObject*) o;
}

JObject* JBoolean_new(BOOL b)
{
  JBoolean* o = NEW(JBoolean);
  o->base.type = J_BOOLEAN;
  o->value = b;
  return (JObject*) o;
}


JObject* JFunction_new(char* fct)
{
  int functionID = getFunction(fct);
  if (functionID != -1)
  {
    JFunction* o = NEW(JFunction);
    o->base.type = J_FUNCTION;
    o->argList = NULL;
    o->functionID = functionID;
    return (JObject*) o;
  }
  else
  {
    fprintf(stdout, "filtered fct not found !\n");
  }

  return NULL;
}

JObject* JArgs_new(void)
{
  JArgs* o = NEW(JArgs);
  o->base.type = J_FUNCTION_ARGS;
  o->nb_args = 0;
  return (JObject*) o;
}

JObject* JArray_new(char* name, int offset)
{
  JArray* o = NEW(JArray);
  o->base.type = J_ARRAY;
  o->identifier = name;
  o->offset = offset;
  return (JObject*) o;
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

char* JObject_toString(JObject* pObject)
{
  ASSERT(pObject != NULL);
  char* s;
  s = NULL;

  switch (pObject->type)
  {
    case J_STR_CONSTANTE:
      s = strdup(((JStringConstante*) pObject)->str_constant);
      break;

    case J_INTEGER:
      s = intToStr(((JInteger*) pObject)->value);
      break;

    case J_DOUBLE:
      s = doubleToStr(((JDouble*) pObject)->value);
      break;

    case J_IDENTIFIER:
      {
        JIdentifier* pIdent;
        pIdent = (JIdentifier*) pObject;
        parameter_type type = param_getType(pIdent->identifier);
        switch (type)
        {
          case TYPE_STRING:
            s = strdup((char*) param_getValue(pIdent->identifier).type_string);
            break;

          case TYPE_INT:
            s = intToStr(param_getValue(pIdent->identifier).type_int);
            if (s == NULL)
            {
              fprintf(stdout, "ID error\n");
              getAstRoot()->inError = TRUE;
            }
            break;

          case TYPE_DOUBLE:
            s = doubleToStr(param_getValue(pIdent->identifier).type_double);
            if (s == NULL)
            {
              fprintf(stdout, "ID error\n");
              getAstRoot()->inError = TRUE;
            }
            break;

          default:
            getAstRoot()->inError = TRUE;
            fprintf(stdout, "unknown '%s' identifier\n", pIdent->identifier);
            ASSERT(FALSE);
            break;
        }
      }
      break;

    case J_ARRAY:
      {
        JArray* pArray;
        pArray = (JArray*) pObject;
        BOOL bOk;
        parameter_value v;
        parameter_type type = param_getType(pArray->identifier);
        bOk = param_array_getValue(pArray->identifier, pArray->offset, &v);
        if (bOk)
        {
          switch (type)
          {
            case TYPE_STRING:
              s = strdup((char*) v.type_string);
              break;

            case TYPE_DOUBLE:
              s = doubleToStr(v.type_double);
              if (s == NULL)
              {
                fprintf(stdout, "array conversion error\n");
                getAstRoot()->inError = TRUE;
              }
              break;

            case TYPE_INT:
              s = intToStr(v.type_int);
              if (s == NULL)
              {
                fprintf(stdout, "array conversion error\n");
                getAstRoot()->inError = TRUE;
              }
              break;

            default:
              getAstRoot()->inError = TRUE;
              fprintf(stdout, "unknown '%s' array\n", pArray->identifier);
              ASSERT(FALSE);
              break;
          }
        }

      }
      break;

    default:
      fprintf(stdout, "not yet implemented...(type = %d)\n", pObject->type);
      break;
  }

  return s;
}


parameter_value JObject_getValue(JObject* pObject, parameter_type* pType)
{
  ASSERT(pObject != NULL);
  parameter_value s;
  s.type_string = NULL;

  switch (pObject->type)
  {
    case J_STR_CONSTANTE:
      s.type_string = strdup(((JStringConstante*) pObject)->str_constant);
      if (pType)
      {
        *pType = TYPE_STRING;
      }
      break;

    case J_INTEGER:
      s.type_int = (((JInteger*) pObject)->value);
      if (pType)
      {
        *pType = TYPE_INT;
      }
      break;

    case J_DOUBLE:
      s.type_double = (((JDouble*) pObject)->value);
      if (pType)
      {
        *pType = TYPE_DOUBLE;
      }
      break;

    case J_IDENTIFIER:
      {
        JIdentifier* pIdent;
        pIdent = (JIdentifier*) pObject;
        parameter_type type = param_getType(pIdent->identifier);
        switch (type)
        {
          case TYPE_STRING:
            s.type_string = strdup((char*) param_getValue(pIdent->identifier).type_string);
            break;

          case TYPE_INT:
          case TYPE_DOUBLE:
            s =  param_getValue(pIdent->identifier);
            break;

          default:
            getAstRoot()->inError = TRUE;
            fprintf(stdout, "unknown '%s' identifier\n", pIdent->identifier);
            ASSERT(FALSE);
            break;
        }
        if (pType)
        {
          *pType = type;
        }
      }
      break;

    case J_ARRAY:
      {
        JArray* pArray;
        pArray = (JArray*) pObject;
        BOOL bOk;
        parameter_value v;
        parameter_type type = param_getType(pArray->identifier);
        bOk = param_array_getValue(pArray->identifier, pArray->offset, &v);
        if (bOk)
        {
          switch (type)
          {
            case TYPE_STRING:
              s.type_string = strdup((char*) v.type_string);
              break;

            case TYPE_DOUBLE:
            case TYPE_INT:
              s = v;
              break;

            default:
              getAstRoot()->inError = TRUE;
              fprintf(stdout, "unknown '%s' array\n", pArray->identifier);
              ASSERT(FALSE);
              break;
          }
          if (pType)
          {
            *pType = type;
          }
        }

      }
      break;

    default:
      fprintf(stdout, "not yet implemented...(type = %d)\n", pObject->type);
      break;
  }

  return s;
}

void JObject_delete(JObject* pObject)
{
  switch (pObject->type)
  {
    case J_STR_CONSTANTE:
      free (((JStringConstante*) pObject)->str_constant);
      break;

    case J_IDENTIFIER:
      free (((JIdentifier*) pObject)->identifier);
      break;

    case J_INTEGER:
    case J_DOUBLE:
    case J_BOOLEAN:
      //do nothing
      break;

    case J_FUNCTION:
      {
        JFunction* fct = ((JFunction*) pObject);
        if (fct->argList != NULL)
        {
          JObject_delete((JObject*) fct->argList);
        }
      }
      break;

    case J_ARRAY:
      free (((JArray*) pObject)->identifier);
      break;

    case J_FUNCTION_ARGS:
      {
        JArgs* args = ((JArgs*) pObject);
        int i;
        for (i = 0; i < args->nb_args; i++)
        {
          JObject_delete(args->listArgs[i]);
          args->listArgs[i] = NULL;
        }
        args->nb_args = 0;
      }
      break;

    default:
      fprintf(stdout, "type = %d\n", pObject->type);
      ASSERT(FALSE);
      break;
  }

  free(pObject);
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

int JObject_getIntValue(JObject* obj)
{
  ASSERT(obj != NULL);
  int r;
  r = 0;

  switch (obj->type)
  {
    case J_ARRAY:
      fprintf(stdout, "Not yet implemented...\n"); //TODO
      break;

    case J_INTEGER:
      r = ((JInteger*) obj)->value;
      break;
    case J_BOOLEAN:
      r = ((JBoolean*) obj)->value;
      break;

    case J_DOUBLE:
    case J_IDENTIFIER:
      fprintf(stdout, "Not yet implemented...\n"); //TODO
      break;

    default:
      fprintf(stdout, "warning: incompatible type is %d, expected %d\n", obj->type, J_INTEGER);
      break;
  }

  return r;
}


char* JFunction_execute(JFunction* f, char* currentStringValue)
{
  char* s;
  s = currentStringValue;
  fct_converter* fct_item;
  fct_item = &tab_fct_converter[f->functionID];

  switch (f->functionID)
  {
    //function with no argument (except filtering string)
    case FCT_CAPITALIZE:
    case FCT_LOWER:
    case FCT_UPPER:
    case FCT_TITLE:
    case FCT_TRIM:
      if ((f->argList != NULL) && (f->argList->nb_args != 0))
      {
        fprintf(stdout, "warning! unexpected number of arguments for function %s\n", fct_item->name);
      }
      else
      {
        s = fct_item->fct(currentStringValue);
      }
      break;

    //function with 4 arguments
    case FCT_TRUNCATE:
      {
        int minNbArgs = 0;
        void* a[4];

        if (f->argList != NULL)
        {
          minNbArgs = f->argList->nb_args > fct_item->nb_args ? fct_item->nb_args : f->argList->nb_args;
        }

        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case DOUBLE:
              break;

            case INT:
            case BOOLEAN:
              a[i] = (void*) (long) JObject_getIntValue(f->argList->listArgs[i]);
              break;

            case STRING:
              a[i] = (void*) JObject_toString(f->argList->listArgs[i]);
              break;

            default:
              ASSERT(FALSE);
              break;
          }
        }

        //set default value now.
        for (int i = minNbArgs; i < fct_item->nb_args; i++)
        {
          a[i] = fct_item->args_default[i];
        }

        s = fct_item->fct(currentStringValue, a[0], a[1], a[2], a[3]);

        //dessallocate the allocated string argument after execution
        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case STRING:
              free(a[i]);
              break;

            default:
              break;
          }
        }
      }
      break;

    //function with one argument
    case FCT_CENTER:
      {
        int minNbArgs = 0;
        void* a[1];

        if (f->argList != NULL)
        {
          minNbArgs = f->argList->nb_args > fct_item->nb_args ? fct_item->nb_args : f->argList->nb_args;
        }

        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case DOUBLE:
              break;

            case INT:
            case BOOLEAN:
              a[i] = (void*) (long) JObject_getIntValue(f->argList->listArgs[i]);
              break;

            case STRING:
              a[i] = (void*) JObject_toString(f->argList->listArgs[i]);
              break;

            default:
              ASSERT(FALSE);
              break;
          }
        }

        //set default value now.
        for (int i = minNbArgs; i < fct_item->nb_args; i++)
        {
          a[i] = fct_item->args_default[i];
        }

        s = fct_item->fct(currentStringValue, a[0]);

        //dessallocate the allocated string argument after execution
        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case STRING:
              free(a[i]);
              break;

            default:
              break;
          }
        }
      }
      break;

    case FCT_FORMAT:
      {
        int nbArgs = 0;
        parameter_value a[NB_MAX_ARGS];
        parameter_type t[NB_MAX_ARGS];

        if (f->argList != NULL)
        {
          nbArgs = f->argList->nb_args;
        }
        if (nbArgs > 10)
        {
          nbArgs = 10;
        }

        for (int i = 0; i < nbArgs; i++)
        {
          parameter_value v;
          parameter_type type;
          v = JObject_getValue(f->argList->listArgs[i], &type);
          a[i] = v;
          t[i] = type;
          if (type == TYPE_DOUBLE)
          {
            fprintf(stdout, "double value = %f\n", v.type_double);
          }
          //memcpy(&a[i], &v, sizeof(parameter_value));

          if (type == TYPE_DOUBLE)
          {
            fprintf(stdout, "a[%d] ==> double value = %.12f\n", i, a[i]);
          }
        }

        for (int i = nbArgs; i < NB_MAX_ARGS; i++)
        {
          a[i].type_string = NULL;
          t[i] = TYPE_STRING;
        }

        s = format(currentStringValue, nbArgs, a, t);

        //dessallocate the allocated string argument after execution
        for (int i = 0; i < nbArgs; i++)
        {
          switch (t[i])
          {
            case TYPE_STRING:
              free(a[i].type_string);
              break;

            default:
              break;
          }
        }
      }
      break;

    default:
      ASSERT(FALSE);
      break;
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
    //s = f->function(getAstRoot()->currentStringValue); //TODO add argument
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

int JArgs_insert_args(JArgs* obj, JObject* argToInsert)
{
  ASSERT(obj != NULL);
  ASSERT(argToInsert != NULL);
  int rc;

  if (obj->nb_args > NB_MAX_ARGS)
  {
    rc = -1;
  }
  else
  {
    obj->listArgs[obj->nb_args] = argToInsert;
    obj->nb_args++;
    rc = 0;
  }

  return rc;
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


char* ast_getTypeString(ast_type type)
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
  for (indexArgs = 0; indexArgs < argsObj->nb_args; indexArgs++)
  {
    fprintf(stdout, ">> object type = %s (%d)\n",
            ast_getTypeString(argsObj->listArgs[indexArgs]->type),
            argsObj->listArgs[indexArgs]->type);
  }
}


int ast_dump_stack()
{
  unsigned int i;

  fprintf(stdout, "dump_ast\n----------\n");
  fprintf(stdout, "nb item %d\n", ast_nb_object);
  for (i = 0; i < ast_nb_object; i++)
  {
    fprintf(stdout, "> object type = %s (%d)\n", ast_getTypeString(ast_list[i]->type), ast_list[i]->type);
    switch (ast_list[i]->type)
    {
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

  fprintf(stdout, "----------\n");
  return 0;
}








