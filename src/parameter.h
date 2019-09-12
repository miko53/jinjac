#ifndef _PARAMETER_H
#define _PARAMETER_H

#include "common.h"

typedef enum
{
  TYPE_STRING,
  TYPE_INT,
  TYPE_DOUBLE,
  TYPE_UNKOWN
} parameter_type;

typedef union
{
  char* type_string;
  int type_int;
  double type_double;
} parameter_value;

extern void parameter_init(void);
extern int insert_parameter(char* key, parameter_type type, parameter_value value);
extern int insert_array_parameter(char* key, parameter_type type, int nbValue, ...);
extern int update_parameter(char* key, parameter_value newValue);

extern parameter_value param_getValue(char* key);
extern parameter_type param_getType(char* key);
extern BOOL param_isArray(char* key, int* nbItem);

extern BOOL param_array_getValue(char* key, int offset, parameter_value* v);
extern void param_delete_all();


#endif /* _PARAMETER_H */

