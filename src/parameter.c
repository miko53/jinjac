
#include "parameter.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "common.h"
#include <stdio.h>
#include <stdarg.h>

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

static param_item* item_array = NULL;
static int item_nb = 0;
static int item_allocated = 0;


static void add_param_in_array(char* key, parameter_type type, parameter_value value);

void parameter_init(void)
{
  item_allocated = 10;
  item_nb = 0;
  item_array = calloc(10, sizeof(param_item));
  assert(item_array != NULL);
}



int insert_parameter(char* key, parameter_type type, parameter_value value)
{
  assert(key != NULL);

  switch (type)
  {
    case TYPE_STRING:
      assert(value.type_string != NULL);
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
    add_param_in_array(key, type, value);
  }
  else
  {
    void* temp = reallocarray(item_array, sizeof(param_item), item_allocated * 2);
    if (temp != NULL)
    {
      free(item_array);
      item_array = temp;
      item_allocated = item_allocated * 2;
      add_param_in_array(key, type, value);
    }
    else
    {
      status = 0;
    }
  }

  return status;
}

static void add_param_in_array(char* key, parameter_type type, parameter_value value)
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

parameter_value param_getValue(char* key)
{
  int i;
  parameter_value v;
  for (i = 0; i < item_nb; i++)
  {
    if (strcmp(key, item_array[i].key) == 0)
    {
      return item_array[i].value;
    }
  }

  ASSERT(FALSE);
  v.type_string = NULL;
  return v;
}

int update_parameter(char* key, parameter_value newValue)
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



BOOL param_array_getValue(char* key, int offset, parameter_value* v)
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


parameter_type param_getType(char* key)
{
  int i;
  for (i = 0; i < item_nb; i++)
  {
    if (strcmp(key, item_array[i].key) == 0)
    {
      return item_array[i].type;
    }
  }
  return TYPE_UNKOWN;
}

BOOL param_isArray(char* key, int* nbItem)
{
  int i;
  ASSERT(key != NULL);
  ASSERT(nbItem != NULL);

  for (i = 0; i < item_nb; i++)
  {
    if (strcmp(key, item_array[i].key) == 0)
    {
      *nbItem = item_array[i].arrayMaxValue;
      return item_array[i].isArray;
    }
  }
  return FALSE;
}


int insert_array_parameter(char* key, parameter_type type, int nbValue, ...)
{
  assert(key != NULL);
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

  return status;

}

void param_delete_all()
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
