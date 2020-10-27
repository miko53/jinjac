
#include "j_object.h"
#include <stdio.h>
#include <string.h>
#include "convert.h"
#include "param.h"
#include "trace.h"

char* j_object_toString(j_object* pObject)
{
  j_value param;
  BOOL bOk;
  char* s;
  s = NULL;

  bOk = j_object_getValue(pObject, &param);
  if (bOk)
  {
    switch (param.type)
    {
      case J_STRING:
        s = param.value.type_string;
        break;

      case J_DOUBLE:
        s = doubleToStr(param.value.type_double);
        break;

      case J_INT:
        s = intToStr(param.value.type_int);
        break;

      default:
        //trace("type =%d\n", param.type);
        ASSERT(FALSE);
        s = NULL;
        break;
    }
  }

  return s;
}

int32_t j_object_toInteger(j_object* pObject)
{
  j_value param;
  BOOL bOk;
  char* s = NULL;
  int32_t r = 0;

  bOk = j_object_getValue(pObject, &param);
  if (bOk)
  {
    switch (param.type)
    {
      case J_STRING:
        r = strtol(param.value.type_string, &s, 0);
        break;

      case J_DOUBLE:
        r = (int32_t) param.value.type_double;
        break;

      case J_INT:
        r = param.value.type_int;
        break;

      default:
        //trace("type =%d\n", param.type);
        ASSERT(FALSE);
        break;
    }
    j_value_destroy(&param);
  }

  return r;
}

BOOL j_object_toBoolean(j_object* pObject)
{
  if (pObject->getValue != NULL)
  {
    return pObject->toBoolean(pObject);
  }
  else
  {
    trace("can't convert to boolean\n");
  }
  return FALSE;
}


BOOL j_object_getValue(j_object* pObject, j_value* value)
{
  if (pObject->getValue != NULL)
  {
    return pObject->getValue(pObject, value);
  }
  else
  {
    trace("can't obtain value\n");
    value->type = J_INT;
    value->value.type_int = 0;
  }
  return FALSE;
}

uint32_t j_object_getCount(j_object* pObject)
{
  if (pObject->getCount != NULL)
  {
    return pObject->getCount(pObject);
  }
  else
  {
    trace("getCount: not possible\n");
  }

  return 0;
}

j_object* j_object_clone(j_object* pObject)
{
  if (pObject->clone != NULL)
  {
    return pObject->clone(pObject);
  }
  else
  {
    trace("clone : not possible\n");
  }

  return NULL;
}


j_object* j_object_getAtIndex(j_object* pObject, int32_t offset)
{
  if (pObject->getAtIndex != NULL)
  {
    return pObject->getAtIndex(pObject, offset);
  }
  else
  {
    trace("getAtIndex: not possible\n");
    j_object_display(pObject);
  }

  return (j_object*) j_object_string_new(strdup(""));
}

BOOL j_object_isEndSequenceReached(j_object* pObject, int32_t currentIndex)
{
  if (pObject->isEndSequenceReached != NULL)
  {
    return pObject->isEndSequenceReached(pObject, currentIndex);
  }
  else
  {
    trace("isEndSequenceReached: not possible\n");
    j_object_display(pObject);
  }

  return TRUE;
}

void j_object_delete(j_object* pObject)
{
  if (pObject->delete != NULL)
  {
    pObject->delete (pObject);
  }
  free(pObject);
}

void j_object_string_delete(j_object* pObject)
{
  free(((j_object_string*) pObject)->s);
}

void j_object_identifier_delete(j_object* pObject)
{
  free (((j_object_identifier*) pObject)->identifier);
}

BOOL j_object_string_getValue(j_object* pObject, j_value* value)
{
  value->value.type_string = strdup(((j_object_string*) pObject)->s);
  value->type = J_STRING;
  return TRUE;
}

BOOL j_object_integer_getValue(j_object* pObject, j_value* value)
{
  value->value.type_int = (((j_object_integer*) pObject)->integer);
  value->type = J_INT;
  return TRUE;
}

BOOL j_object_boolean_getValue(j_object* pObject, j_value* value)
{
  value->value.type_int = (((j_object_boolean*) pObject)->value);
  value->type = J_INT;
  return TRUE;
}

BOOL j_object_double_getValue(j_object* pObject, j_value* value)
{
  value->value.type_double = (((j_object_double*) pObject)->floatingNumber);
  value->type = J_DOUBLE;
  return TRUE;
}

BOOL j_object_identifier_getValue(j_object* pObject,
                                  j_value* param) //struct JObjects* pObject, jinjac_parameter* param)
{
  BOOL bOk;
  j_value_list* pParam;
  j_object_identifier* pIdent;
  pParam = NULL;
  pIdent = (j_object_identifier*) pObject;
  bOk = param_get(pIdent->identifier, &pParam);
  if (!bOk)
  {
    //error(WARNING_LEVEL, "warning: unknown '%s' identifier\n", pIdent->identifier);
    param->type = J_STRING;
    param->value.type_string = strdup("");
    bOk = TRUE;
  }
  else
  {
    if (pParam == NULL)
    {
      //special case, value create but not initialised --> nil value
      param->type = J_STRING;
      param->value.type_string = strdup("");
      bOk = TRUE;
    }
    else
    {
      if (pParam->next != NULL)
      {
        bOk = TRUE;
        param->value.type_string = param_convertArrayToString(pParam);
        param->type = J_STRING;
        //trace("array ==> %s\n", param->value.type_string);
      }
      else
      {
        *param = pParam->value;
        if (pParam->value.type == J_STRING)
        {
          param->value.type_string = strdup(param->value.type_string);
        }
      }
    }
  }
  return bOk;
}

BOOL j_object_integer_toBoolean(j_object* pObject)
{
  BOOL r;
  j_object_integer* p = (j_object_integer*) pObject;
  r = TRUE;
  if (p->integer == 0)
  {
    r = FALSE;
  }
  return r;
}

BOOL j_object_boolean_toBoolean(j_object* pObject)
{
  j_object_boolean* p = (j_object_boolean*) pObject;
  return p->value;
}

BOOL j_object_double_toBoolean(j_object* pObject)
{
  BOOL r;
  j_object_double* p = (j_object_double*) pObject;
  r = TRUE;
  if (p->floatingNumber == 0.0)
  {
    r = FALSE;
  }
  return r;
}

BOOL j_object_string_toBoolean(j_object* pObject)
{
  BOOL r;
  j_object_string* p = (j_object_string*) pObject;
  r = TRUE;
  if (strcmp(p->s, "") == 0)
  {
    r = FALSE;
  }
  return r;
}

BOOL j_object_identifier_toBoolean(j_object* pObject)
{
  BOOL bOk;
  j_value_list* pParam;
  j_object_identifier* pIdent;
  pParam = NULL;
  pIdent = (j_object_identifier*) pObject;
  bOk = param_get(pIdent->identifier, &pParam);
  if (bOk)
  {
    if (pParam == NULL) //special case, value create but not initialised --> nil value
    {
      bOk = FALSE;
    }
    else
    {
      if (pParam->next == NULL)
      {
        switch (pParam->value.type)
        {
          case J_STRING:
            if (strcmp(pParam->value.value.type_string, "") == 0)
            {
              bOk = FALSE;
            }
            break;
          case J_DOUBLE:
            if (pParam->value.value.type_double == 0.0)
            {
              bOk = FALSE;
            }
            break;

          case J_INT:
            if (pParam->value.value.type_int == 0)
            {
              bOk = FALSE;
            }
            break;

          default:
            bOk = FALSE;
            ASSERT(FALSE);
            break;
        }
      }
      else
      {
        //in case of array return TRUE
        ;
      }
    }
  }

  return bOk;
}

j_object* j_object_identifier_getAtIndex(j_object* pObject, int32_t offset)
{
  BOOL bOk;
  j_object* ret = NULL;
  j_value_list* pParam;
  j_object_identifier* pIdent;
  pParam = NULL;
  pIdent = (j_object_identifier*) pObject;
  bOk = param_get(pIdent->identifier, &pParam);
  if (!bOk)
  {
    //error(WARNING_LEVEL, "warning: unknown '%s' identifier\n", pIdent->identifier);
    ret = (j_object*) j_object_string_new(strdup(""));
    bOk = TRUE;
  }
  else
  {
    j_value* vResult;
    uint32_t count;
    count = param_getCount(pParam);
    if ((count == 1) && (pParam->value.type == J_STRING))
    {
      char v[2];
      v[1] = '\0';
      v[0] = pParam->value.value.type_string[offset];
      ret = (j_object*) j_object_string_new(strdup(v));
    }
    else if (count == 1) // not a string a double of int --> not iterable
    {
      ret = NULL;
      ret = (j_object*) j_object_string_new(strdup(""));
    }
    else
    {
      bOk = param_getAt(pParam, offset, &vResult);
      if (bOk)
      {
        switch (vResult->type)
        {
          case J_INT:
            ret = (j_object*) j_object_integer_new(vResult->value.type_int);
            break;

          case J_DOUBLE:
            ret = (j_object*) j_object_double_new(vResult->value.type_double);
            break;

          case J_STRING:
            ret = (j_object*) j_object_string_new(strdup(vResult->value.type_string));
            break;

          default:
            ASSERT(FALSE);
            break;
        }
      }
      else
      {
        ret = (j_object*) j_object_string_new(strdup(""));
      }
    }
  }

  return ret;
}

uint32_t j_object_identifier_getCount(j_object* pObject)
{
  j_object_identifier* pIdent = (j_object_identifier*) pObject;
  uint32_t count;
  BOOL bOk;
  j_value_list* pParam;

  bOk = param_get(pIdent->identifier, &pParam);
  if (!bOk)
  {
    count = 0;
  }
  else
  {
    count = param_getCount(pParam);
    if (count == 1)
    {
      if (pParam->value.type == J_STRING)
      {
        count = strlen(pParam->value.value.type_string);
      }
      else
      {
        count = 0;  //In this case it is not iterable so return 0
      }
    }
  }

  return count;
}

j_object* j_object_string_getAtIndex(j_object* pObject, int32_t offset)
{
  j_object_string* p = (j_object_string*) pObject;
  j_object_string* ret = NULL;
  char v[2];

  v[1] = '\0';
  v[0] = p->s[offset];
  ret = j_object_string_new(strdup(v));
  return (j_object*) ret;
}

uint32_t j_object_string_getCount(j_object* pObject)
{
  j_object_string* p = (j_object_string*) pObject;
  return strlen(p->s);
}

j_object* j_object_integer_clone(j_object* pObject)
{
  return (j_object*) j_object_integer_new(((j_object_integer*) pObject)->integer);
}

j_object_integer* j_object_integer_new(uint32_t v)
{
  j_object_integer* pObject = NEW(j_object_integer);
  pObject->base.type = INTEGER;
  pObject->base.clone = j_object_integer_clone;
  pObject->base.delete = NULL;
  pObject->base.toBoolean = j_object_integer_toBoolean;
  pObject->base.getCount = NULL;
  pObject->base.getValue = j_object_integer_getValue;
  pObject->base.getAtIndex = NULL;
  pObject->base.isEndSequenceReached = NULL;
  pObject->integer = v;
  return pObject;
}

j_object* j_object_double_clone(j_object* pObject)
{
  return (j_object*) j_object_double_new(((j_object_double*) pObject)->floatingNumber);
}

j_object_double* j_object_double_new(double v)
{
  j_object_double* pObject = NEW(j_object_double);
  pObject->base.type = DOUBLE;
  pObject->base.clone = j_object_double_clone;
  pObject->base.delete = NULL;
  pObject->base.toBoolean = NULL;
  pObject->base.getCount = NULL;
  pObject->base.getValue = j_object_double_getValue;
  pObject->base.getAtIndex = NULL;
  pObject->base.isEndSequenceReached = NULL;
  pObject->floatingNumber = v;
  return pObject;
}

j_object* j_object_string_clone(j_object* pObject)
{
  return (j_object*) j_object_string_new(strdup(((j_object_string*) pObject)->s));
}

j_object_string* j_object_string_new(char* s)
{
  j_object_string* pObject = NEW(j_object_string);
  pObject->base.type = STRING;
  pObject->base.clone = j_object_string_clone;
  pObject->base.delete = j_object_string_delete;
  pObject->base.toBoolean = j_object_string_toBoolean;
  pObject->base.getCount = j_object_string_getCount;
  pObject->base.getValue = j_object_string_getValue;
  pObject->base.getAtIndex = j_object_string_getAtIndex;
  pObject->base.isEndSequenceReached = NULL;
  pObject->s = s;
  return pObject;
}


BOOL j_object_identifier_isEndSequenceReached(j_object* pObject, int32_t index)
{
  j_object_identifier* pIdent = (j_object_identifier*) pObject;
  BOOL bOk;
  j_value_list* pParam;
  BOOL isOutOfRange;
  isOutOfRange = FALSE;

  bOk = param_get(pIdent->identifier, &pParam);
  if (!bOk)
  {
    //error(WARNING_LEVEL, "warning: unknown '%s' identifier should not occurs here...\n", pIdent->identifier);
    isOutOfRange = TRUE;
  }
  else
  {
    uint32_t count;
    count = param_getCount(pParam);
    //trace("count = %d index = %d\n", count, index);
    if (index >= (int32_t) count)
    {
      isOutOfRange = TRUE;
    }
  }

  return isOutOfRange;
}

j_object* j_object_identifier_clone(j_object* pObject)
{
  return (j_object*) j_object_identifier_new(strdup(((j_object_identifier*) pObject)->identifier));
}


j_object_identifier* j_object_identifier_new(char* name)
{
  j_object_identifier* pObject = NEW(j_object_identifier);
  pObject->base.type = IDENTIFIER;
  pObject->base.clone = j_object_identifier_clone;
  pObject->base.delete = j_object_identifier_delete;
  pObject->base.toBoolean = j_object_identifier_toBoolean;
  pObject->base.getCount = j_object_identifier_getCount;
  pObject->base.getValue = j_object_identifier_getValue;
  pObject->base.getAtIndex = j_object_identifier_getAtIndex;
  pObject->base.isEndSequenceReached = j_object_identifier_isEndSequenceReached;
  pObject->identifier = name;

  return pObject;
}

j_object* j_object_boolean_clone(j_object* pObject)
{
  return (j_object*) j_object_boolean_new(((j_object_boolean*) pObject)->value);
}


j_object_boolean* j_object_boolean_new(BOOL b)
{
  j_object_boolean* pObject = NEW(j_object_boolean);
  pObject->base.type = BOOLEAN;
  pObject->base.clone = j_object_boolean_clone;
  pObject->base.delete = NULL;
  pObject->base.toBoolean = j_object_boolean_toBoolean;
  pObject->base.getCount = NULL;
  pObject->base.getValue = j_object_boolean_getValue;
  pObject->base.getAtIndex = NULL;
  pObject->base.isEndSequenceReached = NULL;
  pObject->value = b;
  return pObject;
}


j_object* j_object_range_getAtIndex(j_object* pObject, int32_t offset)
{
  int32_t value;
  j_object_range* p = (j_object_range*) pObject;
  value = p->start + offset * p->step;
  return (j_object*) j_object_integer_new(value);
}


BOOL j_object_range_isEndSequenceReached (j_object* pObject, int32_t index)
{
  j_object_range* p = (j_object_range*) pObject;
  int32_t value;
  BOOL isOutOfRange;
  isOutOfRange = FALSE;

  value =  p->start + index * p->step;

  if (value >= p->stop)
  {
    isOutOfRange = TRUE;
  }

  return isOutOfRange;
}

j_object_range* j_object_range_new(int32_t start, int32_t stop, int32_t step)
{
  j_object_range* pObject = NEW(j_object_range);
  pObject->base.type = RANGE;
  pObject->base.clone = NULL;
  pObject->base.delete = NULL;
  pObject->base.toBoolean = NULL;
  pObject->base.getValue = NULL;
  pObject->base.getCount = NULL;
  pObject->base.getAtIndex = j_object_range_getAtIndex;
  pObject->base.isEndSequenceReached = j_object_range_isEndSequenceReached;
  pObject->start = start;
  pObject->stop = stop;
  pObject->step = step;

  return pObject;
}

j_object_iterator* j_object_iterator_new(j_object* pSequencedObj)
{
  j_object_iterator* pObject = NEW(j_object_iterator);
  pObject->base.type = ITERATOR;
  pObject->base.clone = NULL;
  pObject->base.delete = NULL;
  pObject->base.toBoolean = NULL;
  pObject->base.getValue = NULL;
  pObject->base.getAtIndex = NULL;
  pObject->base.getCount = NULL;
  pObject->pSequencedObj = pSequencedObj;
  pObject->current = 0;
  return pObject;
}

j_object* j_object_iterator_get_first(j_object_iterator* pObject)
{
  j_object* pResult = NULL;

  pObject->current = 0;
  pResult = j_object_getAtIndex(pObject->pSequencedObj, pObject->current);
  return pResult;
}

j_object* j_object_iterator_get_next(j_object_iterator* pObject)
{
  j_object* pResult = NULL;

  pResult = j_object_getAtIndex(pObject->pSequencedObj, pObject->current);
  return pResult;
}

BOOL j_object_iterator_isFinished(j_object_iterator* pObject)
{
  BOOL b;
  b = j_object_isEndSequenceReached(pObject->pSequencedObj, pObject->current);
  return b;
}

void j_object_iterator_increment(j_object_iterator* pObject)
{
  pObject->current++;
}


BOOL isTypeOkForCalcul(j_value_type type)
{
  BOOL bOk;
  bOk = TRUE;

  if (type == J_STRING)
  {
    bOk = FALSE;
  }

  return bOk;
}

typedef struct
{
  j_value_type type_op1;
  j_value_type type_op2;
  j_value_type type_result;
} op_decision;

enum
{
  OPERATION_DDD,
  OPERATION_DID,
  OPERATION_IDD,
  OPERATION_III
};

static const op_decision operation_array_decision[] =
{
  { J_DOUBLE, J_DOUBLE, J_DOUBLE }, //, calcul_ddd },
  { J_DOUBLE, J_INT, J_DOUBLE }, //, calcul_did },
  { J_INT, J_DOUBLE, J_DOUBLE }, //, calcul_idd },
  { J_INT, J_INT, J_INT } //, calcul_iii},
};

static int32_t select_operation(j_value_type t1, j_value_type t2)
{
  uint32_t i;

  for (i = 0; i < sizeof(operation_array_decision) / sizeof(op_decision); i++)
  {
    if ((operation_array_decision[i].type_op1 == t1) &&
        (operation_array_decision[i].type_op2 == t2))
    {
      return i;
    }
  }

  ASSERT(FALSE);
  return -1;
}

j_object* j_object_doOperation(j_object* op1, j_object* op2, char mathOperation)
{
  ASSERT(op1 != NULL);
  ASSERT(op2 != NULL);

  j_object* pObjResult = NULL;
  BOOL bOk1, bOk2;

  //object for mathematical can'not be string.
  j_value paramOp1;
  j_value paramOp2;

  bOk1 = j_object_getValue(op1, &paramOp1);
  bOk2 = j_object_getValue(op2, &paramOp2);

  if (bOk1)
  {
    bOk1 = isTypeOkForCalcul(paramOp1.type);
    if (!bOk1)
    {
      free(paramOp1.value.type_string);
    }
  }

  if (bOk2)
  {
    bOk2 = isTypeOkForCalcul(paramOp2.type);
    if (!bOk2)
    {
      free(paramOp2.value.type_string);
    }
  }

  if ((bOk1 == TRUE) && (bOk2 == TRUE))
  {
    int32_t s = select_operation(paramOp1.type, paramOp2.type);
    switch (s)
    {
      case OPERATION_DDD:
        {
          double r = calcul_ddd(paramOp1.value.type_double, paramOp2.value.type_double, mathOperation);
          pObjResult = (j_object*) j_object_double_new(r);
        }
        break;

      case OPERATION_DID:
        {
          double r = calcul_did(paramOp1.value.type_double, paramOp2.value.type_int, mathOperation);
          pObjResult = (j_object*) j_object_double_new(r);
        }
        break;

      case OPERATION_IDD:
        {
          double r = calcul_idd(paramOp1.value.type_int, paramOp2.value.type_double, mathOperation);
          pObjResult = (j_object*) j_object_double_new(r);
        }
        break;

      case OPERATION_III:
        {
          int r = calcul_iii(paramOp1.value.type_int, paramOp2.value.type_int, mathOperation);
          pObjResult = (j_object*) j_object_integer_new(r);
        }
        break;

      default:
        ASSERT(FALSE);
        break;
    }
  }

  return pObjResult;
}

j_object* j_object_doCondition(j_object* op1, j_object* op2, j_condition condition)
{
  j_object* pObjectResult;
  pObjectResult = NULL;
  //BOOL bOk1;
  //BOOL bOk2;
  BOOL bComparisonResult;
  j_value paramOp1;
  j_value paramOp2;

  /*bOk1 =*/ j_object_getValue(op1, &paramOp1);
  /*bOk2 =*/ j_object_getValue(op2, &paramOp2);

  if ((paramOp1.type == J_STRING) && (paramOp2.type == J_STRING))
  {
    if (((condition == COND_EQUAL) && (strcmp(paramOp1.value.type_string, paramOp2.value.type_string) == 0)) ||
        ((condition == COND_DIFFERENT) && (strcmp(paramOp1.value.type_string, paramOp2.value.type_string) != 0)))
    {
      pObjectResult = (j_object*) j_object_boolean_new(TRUE);
    }
    else
    {
      pObjectResult = (j_object*) j_object_boolean_new(FALSE);
    }
    j_value_destroy(&paramOp1);
    j_value_destroy(&paramOp2);
  }
  else if ((paramOp1.type == J_STRING) || (paramOp2.type == J_STRING))
  {
    //trace("one of the two operation is a string ==> FALSE\n");
    pObjectResult = (j_object*) j_object_boolean_new(FALSE);
    j_value_destroy(&paramOp1);
    j_value_destroy(&paramOp2);
  }
  else
  {
    int32_t s = select_operation(paramOp1.type, paramOp2.type);
    switch (s)
    {
      case OPERATION_DDD:
        bComparisonResult = compare_ddd(paramOp1.value.type_double, paramOp2.value.type_double, condition);
        break;

      case OPERATION_DID:
        bComparisonResult = compare_did(paramOp1.value.type_double, paramOp2.value.type_int, condition);
        break;

      case OPERATION_IDD:
        bComparisonResult = compare_idd(paramOp1.value.type_int, paramOp2.value.type_double, condition);
        break;

      case OPERATION_III:
        bComparisonResult = compare_iii(paramOp1.value.type_int, paramOp2.value.type_int, condition);
        break;

      default:
        bComparisonResult = FALSE;
        ASSERT(FALSE);
        break;
    }

    pObjectResult = (j_object*) j_object_boolean_new(bComparisonResult);
  }

  return pObjectResult;
}

void j_object_display(j_object* pObject)
{
  switch (pObject->type)
  {
    case STRING:
      trace("STRING");
      break;

    case INTEGER:
      trace("INTEGER");
      break;

    case DOUBLE:
      trace("DOUBLE");
      break;

    case BOOLEAN:
      trace("BOOLEAN");
      break;

    case IDENTIFIER:
      trace("IDENTIFIER");
      {
        j_object_identifier* p = (j_object_identifier*) pObject;
        trace(" '%s'\n", p->identifier);
      }
      break;

    case RANGE:
      trace("RANGE");
      break;

    case ITERATOR:
      trace("ITERATOR");
      break;

    default:
      trace("unknow object type %d\n", pObject->type);
      break;
  }
}
