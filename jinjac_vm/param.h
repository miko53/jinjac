
#ifndef PARAM_H
#define PARAM_H

#include "j_value.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct j_value_list_s
{
  j_value value;
  struct j_value_list_s* next;
} j_value_list;
  
extern int32_t param_initialize(uint32_t hashTableSize);
//extern int32_t param_add(char* name, j_value_list* pData);
extern int32_t param_insert(char* name, j_value_type type, int32_t nbItems, ...);
extern void param_delete(char* name);

extern int32_t param_update(char* name, j_value* v);

extern void param_destroy(void);

extern BOOL param_get(char* name, j_value_list** v);
extern BOOL param_getAt(j_value_list* head, int32_t offset, j_value** pResult);
extern uint32_t param_getCount(j_value_list* head);
extern char* param_convertArrayToString(j_value_list* v);

#ifdef __cplusplus
}
#endif

#endif /* PARAM_H */
