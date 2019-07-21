
#include "parameter.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct
{
  char* key;
  char* value;
} param_item;

static param_item* item_array = NULL;
static int item_nb = 0;
static int item_allocated = 0;

void parameter_init(void)
{
  item_allocated = 10;
  item_nb = 0;
  item_array = calloc(10, sizeof(param_item));
  assert(item_array != NULL);
}


int insert_parameter(char* key, char* value)
{
  assert(key != NULL);
  assert(value != NULL);
  int status;
  status = 1;

  if (item_array == NULL)
  {
    parameter_init();
  }

  if (item_nb < item_allocated)
  {
    item_array[item_nb].key = strdup(key);
    item_array[item_nb].value = strdup(value);
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
      item_array[item_nb].value = strdup(value);
      item_nb++;
    }
    else
    {
      status = 0;
    }
  }

  return status;
}


char* param_getValue(char* key)
{
  int i;
  for (i = 0; i < item_nb; i++)
  {
    if (strcmp(key, item_array[i].key) == 0)
    {
      return item_array[i].value;
    }
  }
  return NULL;
}

