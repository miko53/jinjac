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
#include "parameter.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "common.h"
#include <stdio.h>
#include <stdarg.h>
#include "str_obj.h"

//TODO add test to check unicity of variables


typedef struct
{
  char* key;
  parameter_type type;
  parameter_value value;
  BOOL isArray;
  void* pArrayValue;
  int arrayMaxValue;
} param_item;

STATIC param_item* item_array = NULL;
STATIC int item_nb = 0;
STATIC int item_allocated = 0;


STATIC void add_param_in_array(char* key, parameter_type type, parameter_value value);

void parameter_init(void)
{
  item_allocated = 10;
  item_nb = 0;
  item_array = calloc(10, sizeof(param_item));
  ASSERT(item_array != NULL);
}

int parameter_insert2(char* key, parameter_type type, parameter_value value)
{
  parameter param;
  param.type = type;
  param.value = value;

  return parameter_insert(key, &param);
}


int parameter_insert(char* key, parameter* param)
{
  ASSERT(key != NULL);
  ASSERT(param != NULL);
  int status;
  status = 1;

  switch (param->type)
  {
    case TYPE_STRING:
      ASSERT(param->value.type_string != NULL);
      break;
    default:
      break;
  }

  if (item_array == NULL)
  {
    parameter_init();
  }

  if (item_nb < item_allocated)
  {
    add_param_in_array(key, param->type, param->value);
  }
  else
  {
    void* temp = reallocarray(item_array, sizeof(param_item), item_allocated * 2);
    if (temp != NULL)
    {
      free(item_array);
      item_array = temp;
      item_allocated = item_allocated * 2;
      add_param_in_array(key, param->type, param->value);
    }
    else
    {
      status = 0;
    }
  }

  return status;
}

STATIC void add_param_in_array(char* key, parameter_type type, parameter_value value)
{
  item_array[item_nb].key = strdup(key);
  item_array[item_nb].type = type;
  item_array[item_nb].isArray = FALSE;
  item_array[item_nb].pArrayValue = NULL;

  switch (type)
  {
    case TYPE_DOUBLE:
      item_array[item_nb].value.type_double = value.type_double;
      break;

    case TYPE_INT:
      item_array[item_nb].value.type_int = value.type_int;
      break;

    case TYPE_STRING:
      item_array[item_nb].value.type_string = strdup(value.type_string);
      break;

    default:
      ASSERT(FALSE);
      break;
  }
  item_nb++;
}

BOOL parameter_get(char* key, parameter* param, BOOL* isArray)
{
  BOOL bFounded;
  int i;

  bFounded = FALSE;

  for (i = 0; i < item_nb; i++)
  {
    if (strcmp(key, item_array[i].key) == 0)
    {
      param->type = item_array[i].type;
      param->value = item_array[i].value;
      if (isArray != NULL)
      {
        *isArray = item_array[i].isArray;
      }
      return TRUE;
    }
  }

  return bFounded;
}

int parameter_update(char* key, parameter_value newValue)
{
  int rc;
  int i;
  rc = -1;
  if (item_nb > 0)
  {
    //NOTE: loop done in inverse mode to allow to have index Name overloaded by the loop one
    for (i = item_nb - 1; i >= 0; i--)
    {
      if (strcmp(key, item_array[i].key) == 0)
      {
        if (item_array[i].type == TYPE_STRING)
        {
          if (item_array[i].value.type_string != NULL)
          {
            free(item_array[i].value.type_string);
          }
          item_array[i].value.type_string = strdup(newValue.type_string);
        }
        else
        {
          item_array[i].value = newValue;
        }
        rc = 0;
        break;
      }
    }
  }

  fprintf(stdout, "parameter to update '%s' not found\n", key);
  return rc;
}


BOOL parameter_array_getValue(char* key, int offset, parameter_value* v)
{
  int i;
  BOOL b;
  b = FALSE;
  ASSERT(v != NULL);

  for (i = 0; i < item_nb; i++)
  {
    if (strcmp(key, item_array[i].key) == 0)
    {
      param_item* current;
      current = &item_array[i];

      if (current->isArray == TRUE)
      {
        if (offset < current->arrayMaxValue)
        {
          switch (current->type)
          {
            case TYPE_DOUBLE:
              v->type_double = ((double*) current->pArrayValue)[offset];
              b = TRUE;
              break;
            case TYPE_INT:
              v->type_int = ((int*) current->pArrayValue)[offset];
              b = TRUE;
              break;

            case TYPE_STRING:
              v->type_string = ((char**) current->pArrayValue)[offset];
              b = TRUE;
              break;

            default:
              ASSERT(FALSE);
              break;
          }
        }
        else
        {
          fprintf(stdout, "array '%s' out of range %d > %d\n", key, offset,   current->arrayMaxValue);
        }
      }
      else
      {
        fprintf(stdout, "'%s' is not an array\n", key);
      }
    }
  }

  return b;
}


BOOL parameter_array_getProperties(char* key, parameter_type* type, int* nbItem)
{
  int i;
  ASSERT(key != NULL);
  ASSERT(type != NULL);

  for (i = 0; i < item_nb; i++)
  {
    if (strcmp(key, item_array[i].key) == 0)
    {
      if (nbItem != NULL)
      {
        *nbItem = item_array[i].arrayMaxValue;
      }

      *type = item_array[i].type;
      return item_array[i].isArray;
    }
  }
  return FALSE;
}


int parameter_array_insert(char* key, parameter_type type, int nbValue, ...)
{
  ASSERT(key != NULL);
  va_list valist;

  va_start(valist, nbValue);

  switch (type)
  {
    case TYPE_STRING:
      break;
    default:
      break;
  }

  int status;
  status = 1;

  if (item_array == NULL)
  {
    parameter_init();
  }

  if (item_nb < item_allocated)
  {
    item_array[item_nb].key = strdup(key);
    item_array[item_nb].type = type;
    item_array[item_nb].isArray = TRUE;
    item_array[item_nb].arrayMaxValue = nbValue;

    switch (type)
    {
      case TYPE_DOUBLE:
        item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(double));
        break;

      case TYPE_INT:
        item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(int));
        break;

      case TYPE_STRING:
        item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(char*));
        break;

      default:
        ASSERT(FALSE);
        break;
    }

    for (int i = 0; i < nbValue; i++)
    {
      switch (type)
      {
        case TYPE_DOUBLE:
          ((double*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, double);
          break;

        case TYPE_INT:
          ((int*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, int);
          break;

        case TYPE_STRING:
          ((char**) item_array[item_nb].pArrayValue)[i] = va_arg( valist, char* );
          break;

        default:
          ASSERT(FALSE);
          break;
      }

    }
    item_nb++;
  }
  else
  {
    void* temp = reallocarray(item_array, sizeof(param_item), item_allocated * 2);
    if (temp != NULL)
    {
      free(item_array);
      item_array = temp;
      item_allocated = item_allocated * 2;
      item_array[item_nb].key = strdup(key);
      item_array[item_nb].type = type;
      item_array[item_nb].isArray = TRUE;
      item_array[item_nb].arrayMaxValue = nbValue;

      switch (type)
      {
        case TYPE_DOUBLE:
          item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(double));
          break;

        case TYPE_INT:
          item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(int));
          break;

        case TYPE_STRING:
          item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(char*));
          break;

        default:
          ASSERT(FALSE);
          break;
      }


      for (int i = 0; i < nbValue; i++)
      {
        switch (type)
        {
          case TYPE_DOUBLE:
            ((double*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, double);
            break;

          case TYPE_INT:
            ((int*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, int);
            break;

          case TYPE_STRING:
            ((char**) item_array[item_nb].pArrayValue)[i] = va_arg( valist, char* );
            break;

          default:
            ASSERT(FALSE);
            break;
        }

      }
      item_nb++;
    }
    else
    {
      status = 0;
    }
  }

  va_end(valist);
  return status;
}

char* parameter_convertArrayToString(char* key)
{
  char* pString;
  BOOL bIsArray;
  parameter_type type;
  int nbItems;
  int indexItem = -1;
  str_obj arrayResult;
  char tampon[50];

  pString = NULL;

  bIsArray = parameter_array_getProperties(key, &type, &nbItems);
  if (bIsArray)
  {
    str_obj_create(&arrayResult);
    for (int i = 0; i < item_nb; i++)
    {
      if (strcmp(key, item_array[i].key) == 0)
      {
        indexItem = i;
        break;
      }
    }
    ASSERT(indexItem != -1); //it is not possible to not find the key...

    str_obj_insert(&arrayResult, "[");
    for (int i = 0; i < nbItems; i++)
    {
      switch (type)
      {
        case TYPE_DOUBLE:
          snprintf(tampon, 50, "%f", ((double*) item_array[indexItem].pArrayValue)[i]);
          break;

        case TYPE_INT:
          snprintf(tampon, 50, "%d", ((int*) item_array[indexItem].pArrayValue)[i]);
          break;

        case TYPE_STRING:
          snprintf(tampon, 50, "u'%s'", ((char**) item_array[indexItem].pArrayValue)[i]);
          break;

        default:
          ASSERT(FALSE);
          break;
      }
      str_obj_insert(&arrayResult, tampon);
      if (i != (nbItems - 1))
      {
        str_obj_insert(&arrayResult, ", ");
      }
    }
    str_obj_insert(&arrayResult, "]");
    pString = arrayResult.s;
  }

  return pString;
}


int parameter_delete(char* key)
{
  int rc;
  int i;
  rc = -1;
  if (item_nb > 0)
  {
    //NOTE: loop done in inverse mode to allow to have index Name overloaded by the loop one
    for (i = item_nb - 1; i >= 0; i--)
    {
      if (strcmp(key, item_array[i].key) == 0)
      {
        if (item_array[i].isArray)
        {
          free(item_array[i].pArrayValue);
        }
        else if (item_array[i].type == TYPE_STRING)
        {
          free(item_array[i].value.type_string);
        }

        free(item_array[i].key);
        memset(&item_array[i], 0, sizeof(param_item));
        rc = 0;
        break;
      }
    }
  }

  fprintf(stdout, "parameter to update '%s' not found\n", key);
  return rc;
}


void parameter_delete_all()
{
  for (int i = 0; i < item_nb; i++)
  {
    if (item_array[i].isArray)
    {
      free(item_array[i].pArrayValue);
    }
    else if (item_array[i].type == TYPE_STRING)
    {
      free(item_array[i].value.type_string);
    }

    free(item_array[i].key);
  }

  free(item_array);
  item_nb = 0;
  item_allocated = 0;
  item_array = NULL;
}
