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
#include "jlist.h"
#include "str_obj.h"

STATIC BOOL JList_getValue(JObject* pObject, jinjac_parameter* param);
STATIC void JList_delete(JObject* pObject);

JObject* JList_new()
{
  JList* o = NEW(JList);
  o->base.type = J_LIST;
  o->base.delete = JList_delete;
  o->base.getValue = JList_getValue;
  o->base.toBoolean = NULL;
  o->isTuple = FALSE;
  o->list = NULL;
  return (JObject*) o;
}

J_STATUS JList_insert(JList* list, JObject* o)
{
  J_STATUS s;
  JListItem* currenItem;

  s = J_OK;

  JListItem* item = NEW(JListItem);

  item->object = o;
  item->next = NULL;

  if (list->list == NULL)
  {
    list->list = item;
  }
  else
  {
    currenItem = list->list;
    while (currenItem->next != NULL)
    {
      currenItem = currenItem->next;
    }

    currenItem->next = item;
  }
  return s;
}

void JList_delete(JObject* pObject)
{
  JList* pList = (JList*) pObject;
  JListItem* pItem;
  JListItem* toDelete;

  pItem = pList->list;

  while (pItem != NULL)
  {
    toDelete = pItem;
    pItem = pItem->next;
    JObject_delete(toDelete->object);
    free(toDelete);
  }
}

BOOL JList_getValue(JObject* pObject, jinjac_parameter* param)
{
  JList* pList = (JList*) pObject;
  JListItem* pItem;
  char* strItem;
  jinjac_parameter itemParam;
  BOOL b;
  b = TRUE;

  param->type = TYPE_STRING;
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
    b = JObject_getValue(pItem->object, &itemParam);
    if (b)
    {
      switch (itemParam.type)
      {
        case TYPE_DOUBLE:
        case TYPE_INT:
          strItem = JObject_toString(pItem->object);
          str_obj_insert(&arrayResult, strItem);
          free(strItem);
          break;

        case TYPE_STRING:
          str_obj_insert(&arrayResult, "'");
          str_obj_insert(&arrayResult, itemParam.value.type_string);
          free(itemParam.value.type_string);
          str_obj_insert(&arrayResult, "'");
          break;

        default:
          ASSERT(FALSE);
          break;

      }
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

void JList_setTuple(JList* list, BOOL isTuple)
{
  if (isTuple)
  {
    list->isTuple = TRUE;
  }
  else
  {
    list->isTuple = FALSE;
  }
}
