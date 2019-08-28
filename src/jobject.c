#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jobject.h"
#include "buildin.h"
#include "convert.h"
#include "ast.h"

#define NEW(obj)  malloc(sizeof(obj))

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
  { .fct = (filter_fct) format, .name = "format", .nb_args = NB_MAX_ARGS },
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


static int getFunctionID(char* fctName)
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
  int functionID = getFunctionID(fct);
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

