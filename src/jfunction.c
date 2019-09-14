
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jfunction.h"
#include "buildin.h"

typedef char* (*buildin_fct)(char*, ...);

typedef enum
{
  INT,
  DOUBLE,
  STRING,
  BOOLEAN
} args_type;

typedef struct
{
  buildin_fct fct;
  const char* name;
  int nb_args;
  args_type args_type[NB_MAX_ARGS];
  void* args_default[NB_MAX_ARGS];
} fct_converter;


fct_converter tab_fct_converter[] =
{
  { .fct = (buildin_fct) capitalize, .name = "capitalize", .nb_args = 0 },
  {
    .fct = (buildin_fct) center, .name = "center", .nb_args = 1,
    .args_type = { INT},
    .args_default = { (void*) 80 }
  },
  { .fct = (buildin_fct) format, .name = "format", .nb_args = NB_MAX_ARGS },
  { .fct = (buildin_fct) lower, .name = "lower", .nb_args = 0 },
  { .fct = (buildin_fct) upper, .name = "upper", .nb_args = 0 },
  {
    .fct = NULL, .name = "range", .nb_args = 3,
    .args_type = { INT, INT, INT},
    .args_default = { 0, 0, (void*) 1}
  },
  { .fct = (buildin_fct) title, .name = "title", .nb_args = 0 },
  { .fct = (buildin_fct) trim, .name = "trim", .nb_args = 0},
  {
    .fct = (buildin_fct) truncate, .name = "truncate", .nb_args = 4,
    .args_type = { INT, BOOLEAN, STRING, INT},
    .args_default = { (void*) 255,  (void*) FALSE, "...", 0 }
  },
};


STATIC int getFunctionID(char* fctName)
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

  error("error: function %s not found !", fctName);
  return -1;
}

void JFunction_delete(JObject* pObject)
{
  JFunction* fct = ((JFunction*) pObject);
  if (fct->argList != NULL)
  {
    JObject_delete((JObject*) fct->argList);
  }
}

JObject* JFunction_new(char* fct)
{
  int functionID = getFunctionID(fct);
  if (functionID != -1)
  {
    JFunction* o = NEW(JFunction);
    o->base.type = J_FUNCTION;
    o->base.delete = JFunction_delete;
    o->base.getValue = NULL;
    o->argList = NULL;
    o->functionID = functionID;
    return (JObject*) o;
  }
  else
  {
    error("filtered fct not found !\n");
  }

  return NULL;
}

void JArgs_delete(JObject* pObject)
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

JObject* JArgs_new(void)
{
  JArgs* o = NEW(JArgs);
  o->base.type = J_FUNCTION_ARGS;
  o->base.delete = JArgs_delete;
  o->base.getValue = NULL;
  o->nb_args = 0;
  return (JObject*) o;
}

JObject* JFunction_execute(JFunction* f, JObject* pCurrentObject)
{
  JObject* resultObject;
  char* s;
  fct_converter* fct_item;

  resultObject = NULL;
  s = NULL;
  //s = currentStringValue;

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
        error("warning! unexpected number of arguments for function %s\n", fct_item->name);
      }
      else
      {
        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = fct_item->fct(s);
          resultObject = JStringConstante_new(s);
        }
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
              a[i] = (void*) (long) JObject_toInteger(f->argList->listArgs[i]);
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

        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = fct_item->fct(s, a[0], a[1], a[2], a[3]);
          resultObject = JStringConstante_new(s);
        }

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
              a[i] = (void*) (long) JObject_toInteger(f->argList->listArgs[i]);
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

        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = fct_item->fct(s, a[0]);
          resultObject = JStringConstante_new(s);
        }

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
        parameter par[NB_MAX_ARGS];

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
          BOOL bOk;
          bOk = JObject_getValue(f->argList->listArgs[i], &par[i]);
        }

        for (int i = nbArgs; i < NB_MAX_ARGS; i++)
        {
          par[i].type = TYPE_STRING;
          par[i].value.type_string = NULL;
        }

        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = format(s, nbArgs, par);
          resultObject = JStringConstante_new(s);
        }

        //dessallocate the allocated string argument after execution
        for (int i = 0; i < nbArgs; i++)
        {
          switch (par[i].type)
          {
            case TYPE_STRING:
              free(par[i].value.type_string);
              break;

            default:
              break;
          }
        }
      }
      break;

    case FCT_RANGE:
      //give number of argument
      {
        int nbArgs = f->argList->nb_args;
        switch (nbArgs)
        {
          case 0: // not possible error...
            break;

          case 1: //only range(stop)
            resultObject = JRange_new(NULL, 0, JObject_toInteger(f->argList->listArgs[0]), 1);
            break;

          case 2: //only range(start, stop)
            resultObject = JRange_new(NULL, JObject_toInteger(f->argList->listArgs[0]),
                                      JObject_toInteger(f->argList->listArgs[1]), 1);
            break;

          default:
          case 3: // range (start, stop, step) ... extras args are ignored
            resultObject = JRange_new(NULL, JObject_toInteger(f->argList->listArgs[0]),
                                      JObject_toInteger(f->argList->listArgs[1]), JObject_toInteger(f->argList->listArgs[2]));
            break;
        }
      }
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return resultObject;
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

