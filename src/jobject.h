#ifndef JOBJECT_H
#define JOBJECT_H

#include "common.h"
#include "parameter.h"

typedef char* (*filter_fct)(char*, ...);

typedef enum
{
  J_STR_CONSTANTE,
  J_INTEGER,
  J_DOUBLE,
  J_BOOLEAN,
  J_IDENTIFIER,
  J_ARRAY,
  J_FUNCTION_ARGS,
  J_FUNCTION,
} jobject_type;

typedef struct
{
  jobject_type type;
} JObject;

typedef struct
{
  JObject base;
  char* str_constant;
} JStringConstante;

typedef struct
{
  JObject base;
  int value;
} JInteger;

typedef struct
{
  JObject base;
  double value;
} JDouble;

typedef struct
{
  JObject base;
  BOOL value;
} JBoolean;

typedef struct
{
  JObject base;
  char* identifier;
} JIdentifier;

typedef struct
{
  JObject base;
  char* identifier;
  int offset;
} JArray;

#define NB_MAX_ARGS   (10)

typedef struct
{
  JObject base;
  JObject* listArgs[NB_MAX_ARGS];
  int nb_args;
} JArgs;

typedef enum
{
  FCT_CAPITALIZE,
  FCT_CENTER,
  FCT_FORMAT,
  FCT_LOWER,
  FCT_UPPER,
  FCT_TITLE,
  FCT_TRIM,
  FCT_TRUNCATE
} fct_id;


typedef struct
{
  JObject base;
  fct_id functionID;
  JArgs* argList;
} JFunction;


extern JObject* JStringConstante_new(char* name);
extern JObject* JIdentifier_new(char* name);
extern JObject* JInteger_new(int i);
extern JObject* JDouble_new(double d);
extern JObject* JBoolean_new(BOOL b);
extern JObject* JFunction_new(char* fct);
extern JObject* JArgs_new(void);
extern JObject* JArray_new(char* name, int offset);
extern char* JObject_toString(JObject* pObject);
extern parameter_value JObject_getValue(JObject* pObject, parameter_type* pType);
extern int JObject_toIntValue(JObject* obj);

extern void JObject_delete(JObject* pObject);
extern char* JFunction_execute(JFunction* f, char* currentStringValue);
extern int JArgs_insert_args(JArgs* obj, JObject* argToInsert);


#endif /* JOBJECT_H */
