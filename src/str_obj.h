#ifndef _STR_OBJ_H
#define _STR_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* _STR_OBJ_H */
