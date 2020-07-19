
#include "common.h"
#include "j_object.h"
#include "j_array.h"
#include "str_obj.h"

static BOOL j_object_array_getValue(j_object* pObject, j_value* param);
static void j_object_array_delete(j_object* pObject);
static uint32_t j_object_array_getCount(j_object* pObject);
static j_object* j_object_array_getAtIndex(j_object* pObject, int32_t offset);

j_object_array * j_object_array_new(BOOL isTuple)
{
  j_object_array* pObject = NEW(j_object_array);
  pObject->base.type = ARRAY;
  pObject->base.clone = NULL;
  pObject->base.delete = j_object_array_delete;
  pObject->base.toBoolean = NULL;
  pObject->base.getValue = j_object_array_getValue;
  pObject->base.getCount = j_object_array_getCount;
  pObject->base.getAtIndex = j_object_array_getAtIndex;
  pObject->base.isEndSequenceReached = NULL;
  pObject->isTuple = isTuple;
  pObject->list = NULL;  
  return pObject;
}

int32_t j_object_array_insert(j_object_array* pArrayObj, j_object* o)
{
  int32_t s;
  j_array_item* currenItem;

  s = 0;

  j_array_item* item = NEW(j_array_item);

  item->object = o;
  item->next = NULL;

  if (pArrayObj->list == NULL)
  {
    pArrayObj->list = item;
  }
  else
  {
    currenItem = pArrayObj->list;
    while (currenItem->next != NULL)
    {
      currenItem = currenItem->next;
    }

    currenItem->next = item;
  }
  return s;
}

void j_object_array_delete(j_object* pObject)
{
  j_object_array* pList = (j_object_array*) pObject;
  j_array_item* pItem;
  j_array_item* toDelete;

  pItem = pList->list;

  while (pItem != NULL)
  {
    toDelete = pItem;
    pItem = pItem->next;
    j_object_delete(toDelete->object);
    free(toDelete);
  }
}

BOOL j_object_array_getValue(j_object* pObject, j_value* param)
{
  j_object_array* pList = (j_object_array*) pObject;
  j_array_item* pItem;
  char* strItem;
  j_value itemParam;
  BOOL b;
  b = TRUE;

  param->type = J_STRING;
  param->value.type_string = NULL;
  pItem = pList->list;

  str_obj arrayResult;
  str_obj_create(&arrayResult, 20);

  if (pList->isTuple)
  {
    str_obj_insertChar(&arrayResult, '(');
  }
  else
  {
    str_obj_insertChar(&arrayResult, '[');
  }

  while ((pItem != NULL) && (b == TRUE))
  {
    b = j_object_getValue(pItem->object, &itemParam);
    if (b)
    {
      switch (itemParam.type)
      {
        case J_DOUBLE:
        case J_INT:
          strItem = j_object_toString(pItem->object);
          str_obj_insert(&arrayResult, strItem);
          free(strItem);
          break;

        case J_STRING:
          str_obj_insert(&arrayResult, "'");
          str_obj_insert(&arrayResult, itemParam.value.type_string);
          str_obj_insert(&arrayResult, "'");
          break;

        default:
          ASSERT(FALSE);
          break;

      }
      j_value_destroy(&itemParam);
      pItem = pItem->next;

      if (pItem != NULL)
      {
        str_obj_insert(&arrayResult, ", ");
      }
    }
  }

  if (pList->isTuple)
  {
    str_obj_insertChar(&arrayResult, ')');
  }
  else
  {
    str_obj_insertChar(&arrayResult, ']');
  }

  param->value.type_string = arrayResult.s;
  return b;
}


uint32_t j_object_array_getCount(j_object* pObject)
{
  j_object_array* pArray = (j_object_array*) pObject;
  uint32_t count;
  j_array_item* head;
  count = 0;
  head = pArray->list;
  while (head != NULL)
  {
    count++;
    head = head->next;
  }
  return count;
}

j_object * j_object_array_getAtIndex(j_object* pObject, int32_t offset)
{
  j_object_array* pArray = (j_object_array*) pObject;
  int32_t index;
  j_array_item* current;
  j_object* pResult = NULL;
  
  current = pArray->list;
  index = 0;
  while ((current != NULL) && (index != offset))
  {
    current = current->next;
    index++;
  }
  
  if ((index == offset) && (current != NULL))
  {
    pResult = j_object_clone(current->object);
  }
  
  return pResult;
}
