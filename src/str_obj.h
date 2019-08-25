#ifndef STR_OBJ_H
#define STR_OBJ_H


typedef struct
{
  char* s;
  int size;
  int allocatedSize;
} str_obj;

extern void str_obj_create(str_obj* obj);
extern void str_obj_free(str_obj* obj);
extern void str_obj_insertChar(str_obj* obj, char src);
extern void str_obj_insert(str_obj* obj, char* src);

#endif /* STR_OBJ_H */
