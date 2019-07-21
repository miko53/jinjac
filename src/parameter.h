#ifndef _PARAMETER_H
#define _PARAMETER_H

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
extern parameter_value param_getValue(char* key);
extern parameter_type param_getType(char* key);


#endif /* _PARAMETER_H */

