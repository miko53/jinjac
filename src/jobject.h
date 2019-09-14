#ifndef JOBJECT_H
#define JOBJECT_H

#include "common.h"
#include "parameter.h"

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
  J_RANGE,
  J_FOR,
  J_END_FOR
} jobject_type;

typedef struct JObjects
{
  jobject_type type;
  void (*delete)(struct JObjects* o);
  BOOL (*getValue)(struct JObjects* pObject, parameter* param);
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

typedef struct
{
  JObject base;
  JObject* sequencedObject;
  int start;
  int stop;
  int step;
  int currentIndex;
} JRange;

typedef struct
{
  JObject base;
  char* identifierOfIndex;
  JRange* sequencing;
  long startOffset;
} JFor;


typedef struct
{
  JObject base;
} JEndFor;

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
  FCT_RANGE,
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

//constructor
extern JObject* JStringConstante_new(char* name);
extern JObject* JIdentifier_new(char* name);
extern JObject* JInteger_new(int i);
extern JObject* JDouble_new(double d);
extern JObject* JBoolean_new(BOOL b);
extern JObject* JFunction_new(char* fct);
extern JObject* JArgs_new(void);
extern JObject* JArray_new(char* name, int offset);
extern JObject* JFor_new(char* nameIdentifier, JRange* sequence);
extern JObject* JEndFor_new(void);

//JObject
extern char* JObject_toString(JObject* pObject);
extern BOOL JObject_getValue(JObject* pObject, parameter* param);

extern int JObject_toInteger(JObject* obj);
extern JObject* JObject_doOperation(JObject* op1, JObject* op2, char mathOperation);
extern void JObject_delete(JObject* pObject);
extern JRange* JObject_toRange(JObject* pObject);

//JFunction
extern JObject* JFunction_execute(JFunction* f, JObject* pCurrentObject);
extern int JArgs_insert_args(JArgs* obj, JObject* argToInsert);

//JFor
extern int JFor_setStartPoint(JFor* obj, long offset);
extern int JFor_createIndexParameter(JFor* obj);
extern BOOL JFor_isDone(JFor* obj);
//JRange
extern BOOL JRange_step(JRange* obj, char* indexIdentifierName);

#endif /* JOBJECT_H */
