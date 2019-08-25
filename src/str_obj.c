#include "str_obj.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

void str_obj_create(str_obj* obj)
{
  obj->s = malloc(10);
  ASSERT(obj->s != NULL);
  obj->s[0] = '\0';
  obj->allocatedSize = 10;
  obj->size = 0;
}

void str_obj_free(str_obj* obj)
{
  if (obj->s)
  {
    free(obj->s);
  }

  obj->s = NULL;
  obj->size = 0;
  obj->allocatedSize = 0;
}

void str_obj_realloc(str_obj* obj, int newSize)
{
  ASSERT(obj != NULL);
  char* n = realloc(obj->s, newSize);
  ASSERT(n != NULL);
  obj->s = n;
  obj->allocatedSize = newSize;
}


void str_obj_insertChar(str_obj* obj, char src)
{
  ASSERT(obj != NULL);
  ASSERT(obj->s != NULL);

  if ((obj->size + 1) >= obj->allocatedSize)
  {
    str_obj_realloc(obj, obj->allocatedSize * 2);
  }

  int size = obj->size;
  obj->s[size] = src;
  size++;
  obj->s[size] = '\0';
  obj->size = size;
}

void str_obj_insert(str_obj* obj, char* src)
{
  int s = strlen(src);
  if (obj->size + s + 1 >= obj->allocatedSize)
  {
    str_obj_realloc(obj, obj->allocatedSize * 2);
  }

  strncat(obj->s, src, s);
  obj->size += s;
  obj->s[obj->size] = '\0';
}
