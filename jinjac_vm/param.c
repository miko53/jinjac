
#include "param.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "str_obj.h"
#include "trace.h"

typedef struct
{
  char* name;
  j_value_list* value;
} param;

typedef struct param_list_s
{
  param data;
  struct param_list_s* next;
} param_node;

static void param_add(char* name, j_value_list* pData);

#define PARAM_HASH_TABLE_DEFAULT_SIZE     (50)

static param_node** param_hash;
static uint32_t param_nbItem;

uint32_t hash_string(char* str)
{
  uint32_t hash = 5381;
  int32_t c;

  while ((c = *str++) != 0)
  {
    hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
  }

  return hash;
}


int32_t param_initialize(uint32_t hashTableSize)
{
  J_STATUS rc;
  rc = J_ERROR;
  if (hashTableSize != 0)
  {
    param_nbItem = hashTableSize;
  }
  else
  {
    param_nbItem = PARAM_HASH_TABLE_DEFAULT_SIZE;
  }

  param_hash = calloc(param_nbItem, sizeof(param_node*));
  if (param_hash != NULL)
  {
    rc = J_OK;
  }
  return rc;
}

j_value_list* param_search(char* name, param_node* root)
{
  param_node* pItem;
  pItem = root;
  while (pItem != NULL)
  {
    if (strcmp(name, pItem->data.name) == 0)
    {
      return pItem->data.value;
    }

    pItem = pItem->next;
  }
  return NULL;
}


static void param_add(char* name, j_value_list* pData)
{
  int32_t hash;
  hash = hash_string(name) % param_nbItem;

  param_node* pNode = NEW(param_node);
  ASSERT(pNode != NULL);
  if (pNode != NULL)
  {
    pNode->data.name = name;
    pNode->data.value = pData;
    pNode->next = param_hash[hash];
    param_hash[hash] = pNode;
  }
}

int32_t param_insert(char* name, j_value_type type, int32_t nbItems, ...)
{
  J_STATUS rc;
  rc = J_ERROR;
  va_list valist;
  va_start(valist, nbItems);
  j_value_list* previous = NULL;
  j_value_list* first = NULL;

  for (int32_t i = 0; i < nbItems; i++)
  {
    j_value_list* pItem = NEW(j_value_list);
    pItem->next = NULL;
    if (previous == NULL)
    {
      previous = pItem;
      first = pItem;
    }
    else
    {
      previous->next = pItem;
      previous = pItem;
    }

    switch (type)
    {
      case J_INT:
        {
          int32_t v = va_arg(valist, int32_t);
          pItem->value.type = type;
          pItem->value.value.type_int = v;
        }
        break;

      case J_DOUBLE:
        {
          double v = va_arg(valist, double);
          pItem->value.type = type;
          pItem->value.value.type_double = v;
        }
        break;

      case J_STRING:
        {
          char* v = va_arg(valist, char*);
          pItem->value.type = type;
          pItem->value.value.type_string = strdup(v);
        }
        break;

      default:
        ASSERT(FALSE);
        break;
    }
  }

  char* pName = strdup(name);
  if (pName != NULL)
  {
    rc = J_OK;
    param_add(pName, first);
  }
  else
  {
    //delete all list
    while (first != NULL)
    {
      j_value_list* next;
      next = first->next;
      j_value_destroy(&first->value);
      free(first);
      first = next;
    }

    free(pName);
  }
  va_end(valist);
  return rc;
}

void j_value_list_destroy(j_value_list* pList)
{
  j_value_list* first;
  first = pList;
  //delete all list
  while (first != NULL)
  {
    j_value_list* next;
    next = first->next;
    j_value_destroy(&first->value);
    free(first);
    first = next;
  }

}

void param_destroy(void)
{
  uint32_t i;
  for (i = 0; i < param_nbItem; i++)
  {
    if (param_hash[i] != NULL)
    {
      param_node* pItem;

      pItem = param_hash[i];
      while (pItem != NULL)
      {
        param_node* pTemp;
        pTemp = pItem->next;
        j_value_list_destroy(pItem->data.value);
        free(pItem->data.name);
        free(pItem);
        pItem = pTemp;
      }
      param_hash[i] = NULL;
    }
  }

  free(param_hash);
}

param_node* param_getNode(char* name, int32_t* hash, param_node** previous)
{
  BOOL bFound;
  param_node* node;
  bFound = FALSE;
  *previous = NULL;

  *hash = hash_string(name) % param_nbItem;
  node = param_hash[*hash];

  if (node != NULL)
  {
    while ((node != NULL) && (bFound == FALSE))
    {
      if (strcmp(node->data.name, name) == 0)
      {
        bFound = TRUE;
      }
      else
      {
        *previous = node;
        node = node->next;
      }
    }
  }

  if (!bFound)
  {
    node = NULL;
  }

  return node;
}


void param_delete(char* name)
{
  int32_t hash;
  param_node* node;
  param_node* previous;

  node = param_getNode(name, &hash, &previous);
  if (node != NULL)
  {
    j_value_list_destroy(node->data.value);
    free(node->data.name);
    if (previous != NULL)
    {
      previous->next = node->next;
    }
    else
    {
      param_hash[hash] = node->next;
    }
    free(node);
  }
  else
  {
    trace("trying to delete an inexisting param...\n");
  }
}

BOOL param_get(char* name, j_value_list** v)
{
  int32_t hash;
  BOOL bFound;
  param_node* node;
  bFound = FALSE;

  hash = hash_string(name) % param_nbItem;
  node = param_hash[hash];

  if (node != NULL)
  {
    while ((node != NULL) && (bFound == FALSE))
    {
      if (strcmp(node->data.name, name) == 0)
      {
        *v = node->data.value;
        bFound = TRUE;
      }
      else
      {
        node = node->next;
      }
    }
  }

  return bFound;
}

uint32_t param_getCount(j_value_list* head)
{
  uint32_t count;
  count = 0;
  while (head != NULL)
  {
    count++;
    head = head->next;
  }
  return count;
}


int32_t param_update(char* name, j_value* v)
{
  J_STATUS rc;
  int32_t hash;
  param_node* previous;
  rc = J_ERROR;

  param_node* paramNode;
  paramNode = param_getNode(name, &hash, &previous);

  if (paramNode != NULL)
  {
    if (paramNode->data.value != NULL)
    {
      j_value_list_destroy(paramNode->data.value);
    }
    j_value_list* pItem = NEW(j_value_list);
    pItem->next = NULL;
    pItem->value = *v;
    if (v->type == J_STRING)
    {
      pItem->value.value.type_string = strdup(v->value.type_string);
    }

    paramNode->data.value = pItem;
    rc = J_OK;
  }

  return rc;
}


char* param_convertArrayToString(j_value_list* v)
{
  char* pString;
  str_obj arrayResult;
  char tampon[50];
  j_value_list* pItem;
  pString = NULL;

  if (v->next != NULL)
  {
    str_obj_create(&arrayResult, 10);
    str_obj_insert(&arrayResult, "[");

    pItem = v;
    while (pItem != NULL)
    {
      switch (pItem->value.type)
      {
        case J_DOUBLE:
          snprintf(tampon, 50, "%f", pItem->value.value.type_double);
          break;

        case J_INT:
          snprintf(tampon, 50, "%d",  pItem->value.value.type_int);
          break;

        case J_STRING:
          snprintf(tampon, 50, "u'%s'", pItem->value.value.type_string);
          break;

        default:
          ASSERT(FALSE);
          break;
      }
      str_obj_insert(&arrayResult, tampon);
      if (pItem->next != NULL)
      {
        str_obj_insert(&arrayResult, ", ");
      }

      pItem = pItem->next;
    }
    str_obj_insert(&arrayResult, "]");
    pString = arrayResult.s;
  }

  return pString;
}


BOOL param_getAt(j_value_list* head, int32_t offset, j_value** pResult)
{
  BOOL bOk;
  int32_t index;
  j_value_list* current;

  bOk = FALSE;

  current = head;
  index = 0;
  while ((current != NULL) && (index != offset))
  {
    current = current->next;
    index++;
  }

  if ((index == offset) && (current != NULL))
  {
    *pResult = &current->value;
    bOk = TRUE;
  }

  return bOk;
}
