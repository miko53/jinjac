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
  int32_t nb_args;
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
  { .fct = (buildin_fct) join, .name = "join", .nb_args = 1, .args_type = { STRING }, .args_default = { (void*) "" }},
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


STATIC int32_t getFunctionID(char* fctName)
{
  int32_t sizeMax = sizeof(tab_fct_converter) / sizeof(fct_converter);
  int32_t i;

  for (i = 0; i < sizeMax; i++)
  {
    if (strcmp(tab_fct_converter[i].name, fctName) == 0)
    {
      return i;
    }
  }

  error(ERROR_LEVEL, "function %s not found !", fctName);
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
  int32_t functionID = getFunctionID(fct);
  if (functionID != -1)
  {
    JFunction* o = NEW(JFunction);
    o->base.type = J_FUNCTION;
    o->base.delete = JFunction_delete;
    o->base.getValue = NULL;
    o->base.toBoolean = NULL;
    o->argList = NULL;
    o->functionID = functionID;
    return (JObject*) o;
  }
  else
  {
    error(ERROR_LEVEL, "filtered fct not found !\n");
  }

  return NULL;
}

//TODO
/*
BOOL JFunction_toBoolean(JObject* pObject)
{
    return FALSE;
}
*/

void JArgs_delete(JObject* pObject)
{
  JArgs* args = ((JArgs*) pObject);
  int32_t i;
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
  o->base.toBoolean = NULL;
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
        error(WARNING_LEVEL, "warning! unexpected number of arguments for function %s\n", fct_item->name);
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
        int32_t minNbArgs = 0;
        void* a[4];

        if (f->argList != NULL)
        {
          minNbArgs = f->argList->nb_args > fct_item->nb_args ? fct_item->nb_args : f->argList->nb_args;
        }

        for (int32_t i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case DOUBLE:
              break;

            case INT:
            case BOOLEAN:
              a[i] = (void*) (int64_t) JObject_toInteger(f->argList->listArgs[i]);
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
        for (int32_t i = minNbArgs; i < fct_item->nb_args; i++)
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
        for (int32_t i = 0; i < minNbArgs; i++)
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
        int32_t minNbArgs = 0;
        void* a[1];
        a[0] = NULL;

        if (f->argList != NULL)
        {
          minNbArgs = f->argList->nb_args > fct_item->nb_args ? fct_item->nb_args : f->argList->nb_args;
        }

        for (int32_t i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case DOUBLE:
              break;

            case INT:
            case BOOLEAN:
              a[i] = (void*) (int64_t) JObject_toInteger(f->argList->listArgs[i]);
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
        for (int32_t i = minNbArgs; i < fct_item->nb_args; i++)
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
        for (int32_t i = 0; i < minNbArgs; i++)
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

    case FCT_JOIN:
      {
        char* pArg;
        BOOL toDelete = FALSE;
        if ((f->argList == NULL) || (f->argList->nb_args == 0))
        {
          pArg = fct_item->args_default[0];
        }
        else
        {
          pArg = JObject_toString(f->argList->listArgs[0]);
          toDelete = TRUE;
        }

        s = join(pCurrentObject, pArg);
        if (s != NULL)
        {
          resultObject = JStringConstante_new(s);
        }
        else
        {
          resultObject = JStringConstante_new(strdup(""));
        }

        if (toDelete)
        {
          free(pArg);
        }
      }
      break;

    case FCT_FORMAT:
      {
        int32_t nbArgs = 0;
        jinjac_parameter par[NB_MAX_ARGS];

        if (f->argList != NULL)
        {
          nbArgs = f->argList->nb_args;
        }
        if (nbArgs > 10)
        {
          nbArgs = 10;
        }

        for (int32_t i = 0; i < nbArgs; i++)
        {
          /*BOOL bOk;
          bOk =*/ JObject_getValue(f->argList->listArgs[i], &par[i]);
        }

        for (int32_t i = nbArgs; i < NB_MAX_ARGS; i++)
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
        for (int32_t i = 0; i < nbArgs; i++)
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
        int32_t nbArgs = f->argList->nb_args;
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

J_STATUS JArgs_insert_args(JArgs* obj, JObject* argToInsert)
{
  ASSERT(obj != NULL);
  ASSERT(argToInsert != NULL);
  J_STATUS rc;

  if (obj->nb_args > NB_MAX_ARGS)
  {
    rc = J_ERROR;
  }
  else
  {
    obj->listArgs[obj->nb_args] = argToInsert;
    obj->nb_args++;
    rc = J_OK;
  }

  return rc;
}

