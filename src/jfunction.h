#ifndef _JFUNCTION_H
#define _JFUNCTION_H

#include "jobject.h"

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

#define NB_MAX_ARGS   (10)

typedef struct
{
  JObject base;
  JObject* listArgs[NB_MAX_ARGS];
  int nb_args;
} JArgs;

typedef struct
{
  JObject base;
  fct_id functionID;
  JArgs* argList;
} JFunction;


//JFunction
extern JObject* JFunction_execute(JFunction* f, JObject* pCurrentObject);
extern int JArgs_insert_args(JArgs* obj, JObject* argToInsert);



#endif /* _JFUNCTION_H */
