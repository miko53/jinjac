#ifndef _PARAMETER_H
#define _PARAMETER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  TYPE_STRING,
  TYPE_INT,
  TYPE_DOUBLE,
} parameter_type;

typedef union
{
  char* type_string;
  int type_int;
  double type_double;
} parameter_value;

typedef struct
{
  parameter_type type;
  parameter_value value;
} parameter;


extern void parameter_init(void);
extern void parameter_delete_all();

extern int parameter_insert(char* key, parameter* param);
extern int parameter_delete(char* key);
extern int parameter_insert2(char* key, parameter_type type, parameter_value value);
extern BOOL parameter_get(char* key, parameter* param);
extern int parameter_update(char* key, parameter_value newValue);

extern int parameter_array_insert(char* key, parameter_type type, int nbValue, ...);
extern BOOL parameter_array_getProperties(char* key, parameter_type* type, int* nbItem);
extern BOOL parameter_array_getValue(char* key, int offset, parameter_value* v);

#ifdef __cplusplus
}
#endif

#endif /* _PARAMETER_H */

