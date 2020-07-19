
#ifndef J_OBJECT_H
#define J_OBJECT_H

#include "common.h"
#include "j_value.h"

#ifdef __cplusplus
extern "C" {
#endif

  
typedef enum
{
  STRING,
  INTEGER,
  DOUBLE,
  BOOLEAN,
  IDENTIFIER,
  RANGE,
  ITERATOR,
  ARRAY,
} j_object_type;
  
typedef enum
{
  COND_EQUAL,
  COND_DIFFERENT,
  COND_HIGH_AND_EQUAL_THAN,
  COND_HIGH_THAN,
  COND_LOWER_AND_EQUAL_THAN,
  COND_LOWER_THAN,
} j_condition;

typedef enum
{
  COND_LOG_OR,
  COND_LOG_AND,
  COND_LOG_NOT,
  COND_LOG_IS
} j_logical_condition;

typedef struct j_object_base
{
  j_object_type type;
  struct j_object_base* (*clone)(struct j_object_base* o);
  void (*delete)(struct j_object_base* o);
  BOOL (*getValue)(struct j_object_base* pObject, j_value* data);
  uint32_t (*getCount)(struct j_object_base* pObject);
  struct j_object_base* (*getAtIndex)(struct j_object_base* pObject, int32_t index);
  BOOL (*isEndSequenceReached)(struct j_object_base* pObject, int32_t index);
  BOOL (*toBoolean)(struct j_object_base* pObject);
} j_object;
  
typedef struct
{
  j_object base;
  char* s;
} j_object_string;
  
typedef struct
{
  j_object base;
  uint32_t integer;
} j_object_integer;

typedef struct
{
  j_object base;
  double floatingNumber;
} j_object_double;

typedef struct
{
  j_object base;
  BOOL value;
} j_object_boolean;

typedef struct
{
  j_object base;
  char* identifier;
} j_object_identifier;

typedef struct
{
  j_object base;
  int32_t start;
  int32_t stop;
  int32_t step;
} j_object_range;

typedef struct
{
  j_object base;
  j_object* pSequencedObj;
  int32_t current;
} j_object_iterator;

extern char* j_object_toString(j_object* pObject);
extern int32_t j_object_toInteger(j_object* pObject);
extern void  j_object_delete(j_object* pObject);
extern BOOL  j_object_getValue(j_object* pObject, j_value* value);
extern uint32_t j_object_getCount(j_object* pObject);

extern BOOL  j_object_toBoolean(j_object* pObject);
extern j_object* j_object_clone(j_object* pObject);
extern j_object * j_object_getAtIndex(j_object* pObject, int32_t offset);
extern BOOL j_object_isEndSequenceReached(j_object* pObject, int32_t currentIndex);

extern j_object_string*  j_object_string_new(char* s);
extern j_object_integer* j_object_integer_new(uint32_t v);
extern j_object_double* j_object_double_new(double v);
extern j_object_identifier* j_object_identifier_new(char* name);
extern j_object_range* j_object_range_new(int32_t start, int32_t stop, int32_t step);
extern j_object_iterator* j_object_iterator_new(j_object* pSequencedObj);
extern j_object_boolean* j_object_boolean_new(BOOL b);

extern j_object* j_object_iterator_get_first(j_object_iterator* pObject);
extern j_object* j_object_iterator_get_next(j_object_iterator* pObject);

extern BOOL j_object_iterator_isFinished(j_object_iterator* pObject);
extern void  j_object_iterator_increment(j_object_iterator* pObject);
extern j_object* j_object_doOperation(j_object* op1, j_object* op2, char mathOperation);

extern j_object* j_object_doCondition(j_object* op1, j_object* op2, j_condition condition);

extern void j_object_display(j_object* pObject);

#ifdef __cplusplus
}
#endif

#endif /* J_OBJECT_H */

