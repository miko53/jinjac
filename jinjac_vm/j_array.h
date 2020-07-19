#ifndef J_ARRAY_H
#define J_ARRAY_H

#include "common.h"
#include "j_object.h"

typedef struct j_array_item_base
{
  j_object* object;
  struct j_array_item_base* next;
} j_array_item;

typedef struct
{
  j_object base;
  BOOL isTuple;
  j_array_item* list;
} j_object_array;

extern j_object_array* j_object_array_new(BOOL isTuple);
extern int32_t j_object_array_insert(j_object_array* pArrayObj, j_object* o);

#endif /* J_ARRAY_H */

