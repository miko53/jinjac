#ifndef _JOBJECT_H
#define _JOBJECT_H

#include "common.h"
#include "parameter.h"

#ifdef __cplusplus
extern "C" {
#endif

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

//JFor
extern int JFor_setStartPoint(JFor* obj, long offset);
extern int JFor_createIndexParameter(JFor* obj);
extern BOOL JFor_isDone(JFor* obj);

//JRange
extern JObject* JRange_new(JObject* objectToBeSequenced, int start, int stop, int step);
extern BOOL JRange_step(JRange* obj, char* indexIdentifierName);

#ifdef __cplusplus
}
#endif

#endif /* _JOBJECT_H */
