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

#define PARAM_USER_KEY_MARK          (0x8000000000000000)

STATIC jinjac_parameter_callback jinjac_parameter_userSearch;

J_STATUS jinjac_parameter_register(jinjac_parameter_callback* callback)
{
  J_STATUS s;
  s = J_ERROR;

  if ((callback->search != NULL) &&
      (callback->get != NULL) &&
      (callback->array_getValue != NULL) &&
      (callback->array_getProperties != NULL))
  {
    jinjac_parameter_userSearch = *callback;
    s = J_OK;
  }

  return s;
}


//TODO add test to check unicity of variables


typedef struct
{
  BOOL isUsed;
  char* key;
  jinjac_parameter_type type;
  jinjac_parameter_value value;
  BOOL isArray;
  void* pArrayValue;
  int32_t arrayMaxValue;
} param_item;

STATIC param_item* item_array = NULL;
STATIC int32_t item_nb = 0;
STATIC int32_t item_allocated = 0;

STATIC void add_param_in_array(char* key, jinjac_parameter_type type, jinjac_parameter_value value);

void parameter_init(void)
{
  item_allocated = 10;
  item_nb = 0;
  item_array = calloc(item_allocated, sizeof(param_item));
  ASSERT(item_array != NULL);
}

int parameter_insert2(char* key, jinjac_parameter_type type, jinjac_parameter_value value)
{
  jinjac_parameter param;
  param.type = type;
  param.value = value;

  return jinjac_parameter_insert(key, &param);
}


J_STATUS jinjac_parameter_insert(char* key, jinjac_parameter* param)
{
  ASSERT(key != NULL);
  ASSERT(param != NULL);
  J_STATUS status;
  status = J_OK;

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
    void* temp = reallocarray(item_array, item_allocated * 2, sizeof(param_item));
    if (temp != NULL)
    {
      //free(item_array);
      item_array = temp;
      item_allocated = item_allocated * 2;
      add_param_in_array(key, param->type, param->value);
    }
    else
    {
      status = J_ERROR;
    }
  }

  return status;
}

STATIC void add_param_in_array(char* key, jinjac_parameter_type type, jinjac_parameter_value value)
{
  trace("insert item '%s' at %d\n", key, item_nb);
  item_array[item_nb].isUsed = TRUE;
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


BOOL parameter_search(char* key, int64_t* privKey, BOOL* isArray)
{
  BOOL bFounded;
  int32_t i;
  ASSERT(key != NULL);
  ASSERT(privKey != NULL);

  bFounded = FALSE;

  for (i = 0; ((i < item_nb) && (bFounded == FALSE)); i++)
  {
    if ((item_array[i].isUsed == TRUE) && (strcmp(key, item_array[i].key) == 0))
    {
      *privKey = i;
      if (isArray != NULL)
      {
        *isArray = item_array[i].isArray;
      }
      bFounded = TRUE;
    }
  }

  if ((!bFounded) && (jinjac_parameter_userSearch.search != NULL))
  {
    int32_t userKey;
    int32_t bIsArray;
    bFounded = jinjac_parameter_userSearch.search(key, &userKey, &bIsArray);
    if (isArray != NULL)
    {
      if (bIsArray == 1)
      {
        *isArray = TRUE;
      }
      else
      {
        *isArray = FALSE;
      }
    }
    if (bFounded)
    {
      *privKey = PARAM_USER_KEY_MARK | userKey;
    }
  }

  return bFounded;
}

J_STATUS parameter_get(int64_t privKey, jinjac_parameter* param)
{
  J_STATUS s;
  ASSERT(param != NULL);

  if ((privKey & PARAM_USER_KEY_MARK) == PARAM_USER_KEY_MARK)
  {
    int32_t userKey = (privKey & 0xFFFFFFFF);
    if (jinjac_parameter_userSearch.get != NULL)
    {
      s = jinjac_parameter_userSearch.get(userKey, param);
    }
    else
    {
      s = J_ERROR;
    }
    return s;
  }

  s = J_OK;
  if (privKey < item_nb)
  {
    param->type = item_array[privKey].type;
    param->value = item_array[privKey].value;
  }
  else
  {
    ASSERT(FALSE);
    s = J_ERROR;
  }

  return s;
}

J_STATUS parameter_update(char* key, jinjac_parameter_value newValue)
{
  J_STATUS rc;
  int32_t i;
  rc = J_ERROR;
  if (item_nb > 0)
  {
    //NOTE: loop done in inverse mode to allow to have index Name overloaded by the loop one
    for (i = item_nb - 1; i >= 0; i--)
    {
      if ((item_array[i].isUsed == TRUE)  && (strcmp(key, item_array[i].key) == 0))
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
        rc = J_OK;
        break;
      }
    }
  }

  if (rc == J_ERROR)
  {
    trace("parameter to update '%s' not found\n", key);
  }

  return rc;
}

J_STATUS parameter_array_getValue(int64_t privKey, int32_t offset, jinjac_parameter_value* v)
{
  J_STATUS s;
  s = J_OK;

  if ((privKey & PARAM_USER_KEY_MARK) == PARAM_USER_KEY_MARK)
  {
    int32_t userKey = (privKey & 0xFFFFFFFF);
    if (jinjac_parameter_userSearch.array_getValue != NULL)
    {
      s = jinjac_parameter_userSearch.array_getValue(userKey, offset, v);
    }
    else
    {
      s = J_ERROR;
    }
    return s;
  }


  if (privKey < item_nb)
  {
    param_item* current;
    current = &item_array[privKey];
    if (current->isArray == TRUE)
    {
      if (offset < current->arrayMaxValue)
      {
        switch (current->type)
        {
          case TYPE_DOUBLE:
            v->type_double = ((double*) current->pArrayValue)[offset];
            break;
          case TYPE_INT:
            v->type_int = ((int32_t*) current->pArrayValue)[offset];
            break;

          case TYPE_STRING:
            v->type_string = ((char**) current->pArrayValue)[offset];
            break;

          default:
            ASSERT(FALSE);
            s = J_ERROR;
            break;
        }
      }
      else
      {
        fprintf(stdout, "array '%s' out of range %d > %d\n", current->key, offset, current->arrayMaxValue);
        s = J_ERROR;
      }
    }
    else
    {
      fprintf(stdout, "'%s' is not an array\n", current->key);
      s = J_ERROR;
    }
  }
  else
  {
    s = J_ERROR;
    ASSERT(FALSE);
  }


  return s;
}

BOOL parameter_array_getProperties(int64_t privKey, jinjac_parameter_type* type, int32_t* nbItem)
{
  BOOL bIsArray;
  ASSERT(type != NULL);
  bIsArray = FALSE;

  if ((privKey & PARAM_USER_KEY_MARK) == PARAM_USER_KEY_MARK)
  {
    int32_t userKey = (privKey & 0xFFFFFFFF);
    if (jinjac_parameter_userSearch.array_getProperties != NULL)
    {
      int32_t bArray = FALSE;
      bArray = jinjac_parameter_userSearch.array_getProperties(userKey, type, nbItem);
      if (bArray == 1)
      {
        bIsArray = TRUE;
      }
      else
      {
        bIsArray = FALSE;
      }

    }
    else
    {
      bIsArray = FALSE;
    }
    return bIsArray;
  }

  if (privKey < item_nb)
  {
    if (nbItem != NULL)
    {
      *nbItem = item_array[privKey].arrayMaxValue;
    }
    *type = item_array[privKey].type;
    bIsArray = TRUE;
  }
  else
  {
    ASSERT(FALSE);
  }


  return bIsArray;
}

J_STATUS jinjac_parameter_array_insert(char* key, jinjac_parameter_type type, int32_t nbValue, ...)
{
  ASSERT(key != NULL);
  va_list valist;

  trace("insert (array) item '%s' (nb=%d) at %d\n", key, nbValue, item_nb);
  va_start(valist, nbValue);

  switch (type)
  {
    case TYPE_STRING:
      break;
    default:
      break;
  }

  J_STATUS status;
  status = J_OK;

  if (item_array == NULL)
  {
    parameter_init();
  }

  if (item_nb < item_allocated)
  {
    item_array[item_nb].isUsed = TRUE;
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

    for (int32_t i = 0; i < nbValue; i++)
    {
      switch (type)
      {
        case TYPE_DOUBLE:
          ((double*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, double);
          break;

        case TYPE_INT:
          ((int32_t*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, int32_t);
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
    void* temp = reallocarray(item_array, item_allocated * 2, sizeof(param_item));
    if (temp != NULL)
    {
      item_array = temp;
      item_allocated = item_allocated * 2;
      item_array[item_nb].isUsed = TRUE;
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
          item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(int32_t));
          break;

        case TYPE_STRING:
          item_array[item_nb].pArrayValue = malloc(nbValue * sizeof(char*));
          break;

        default:
          ASSERT(FALSE);
          break;
      }


      for (int32_t i = 0; i < nbValue; i++)
      {
        switch (type)
        {
          case TYPE_DOUBLE:
            ((double*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, double);
            break;

          case TYPE_INT:
            ((int32_t*) item_array[item_nb].pArrayValue)[i] = va_arg(valist, int32_t);
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
      status = J_ERROR;
    }
  }

  va_end(valist);
  return status;
}

char* parameter_convertArrayToString(int64_t privKey)
{
  char* pString;
  BOOL bIsArray;
  jinjac_parameter_type type;
  int32_t nbItems;
  str_obj arrayResult;
  char tampon[50];

  pString = NULL;

  bIsArray = parameter_array_getProperties(privKey, &type, &nbItems);
  if (bIsArray)
  {
    str_obj_create(&arrayResult, 10);
    str_obj_insert(&arrayResult, "[");
    for (int32_t i = 0; i < nbItems; i++)
    {
      switch (type)
      {
        case TYPE_DOUBLE:
          snprintf(tampon, 50, "%f", ((double*) item_array[privKey].pArrayValue)[i]);
          break;

        case TYPE_INT:
          snprintf(tampon, 50, "%d", ((int32_t*) item_array[privKey].pArrayValue)[i]);
          break;

        case TYPE_STRING:
          snprintf(tampon, 50, "u'%s'", ((char**) item_array[privKey].pArrayValue)[i]);
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


J_STATUS parameter_delete(char* key)
{
  J_STATUS rc;
  int32_t i;
  rc = J_ERROR;

  trace("delete parameter '%s'\n", key);
  if (item_nb > 0)
  {
    //NOTE: loop done in inverse mode to allow to have index Name overloaded by the loop one
    for (i = item_nb - 1; i >= 0; i--)
    {
      if ((item_array[i].isUsed == TRUE) && (strcmp(key, item_array[i].key) == 0))
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
        rc = J_OK;
        break;
      }
    }
  }

  if (rc == J_ERROR)
  {
    trace("parameter to delete '%s' not found\n", key);
  }

  return rc;
}


void jinjac_parameter_delete_all(void)
{
  for (int32_t i = 0; i < item_nb; i++)
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


void param_delete(jinjac_parameter* param)
{
  ASSERT(param != NULL);

  if (param->type == TYPE_STRING)
  {
    free(param->value.type_string);
    param->value.type_string = NULL;
  }
}

