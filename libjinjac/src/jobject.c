/*
 * Copyright (c) 2019 miko53
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jobject.h"
#include "convert.h"
#include "ast.h"

BOOL JObject_getValue(JObject* pObject, jinjac_parameter* param)
{
  ASSERT(pObject != NULL);
  ASSERT(param != NULL);
  BOOL bOk;
  bOk = FALSE;

  if (pObject->getValue != NULL)
  {
    bOk = pObject->getValue(pObject, param);
  }
  else
  {
    error(ERROR_LEVEL, "can't convert object type %d into value\n", pObject->type);
  }

  return bOk;
}

BOOL JStringConstante_getValue(struct JObjects* pObject, jinjac_parameter* param)
{
  param->value.type_string = strdup(((JStringConstante*) pObject)->str_constant);
  param->type = TYPE_STRING;
  return TRUE;
}

BOOL JBoolean_getValue(struct JObjects* pObject, jinjac_parameter* param)
{
  param->value.type_int = ((JBoolean*) pObject)->value;
  param->type = TYPE_INT;
  return TRUE;
}


BOOL JInteger_getValue(struct JObjects* pObject, jinjac_parameter* param)
{
  param->value.type_int = (((JInteger*) pObject)->value);
  param->type = TYPE_INT;
  return TRUE;
}

BOOL JDouble_getValue(struct JObjects* pObject, jinjac_parameter* param)
{
  param->value.type_double = (((JDouble*) pObject)->value);
  param->type = TYPE_DOUBLE;
  return TRUE;
}

BOOL JIdentifier_getValue(struct JObjects* pObject, jinjac_parameter* param)
{
  BOOL bOk;
  BOOL isArray;
  int64_t privKey;
  JIdentifier* pIdent;
  pIdent = (JIdentifier*) pObject;
  bOk = parameter_search(pIdent->identifier, &privKey, &isArray);
  if (!bOk)
  {
    error(WARNING_LEVEL, "warning: unknown '%s' identifier\n", pIdent->identifier);
    param->type = TYPE_STRING;
    param->value.type_string = strdup("");
    bOk = TRUE;
  }
  else
  {
    if (isArray)
    {
      bOk = TRUE;
      param->value.type_string = parameter_convertArrayToString(privKey);
      param->type = TYPE_STRING;
      //trace("array ==> %s\n", param->value.type_string);
    }
    else
    {
      parameter_get(privKey, param);
      if (param->type == TYPE_STRING)
      {
        param->value.type_string = strdup(param->value.type_string);
      }
    }
  }
  return bOk;
}


BOOL JArray_getValue(struct JObjects* pObject, jinjac_parameter* param)
{
  BOOL bOk;
  J_STATUS status;
  int64_t privKey;
  JArray* pArray;
  pArray = (JArray*) pObject;

  bOk = parameter_search(pArray->identifier, &privKey, NULL);
  if (bOk)
  {
    bOk = parameter_array_getProperties(privKey, &param->type, NULL);
    if (bOk)
    {
      status = parameter_array_getValue(privKey, pArray->offset, &param->value);
      if (status == J_OK)
      {
        if (param->type == TYPE_STRING)
        {
          param->value.type_string = strdup(param->value.type_string);
        }
      }
      else
      {
        bOk = FALSE;
      }
    }
  }
  return bOk;
}

BOOL JObject_toBoolean(JObject* pObject)
{
  ASSERT(pObject != NULL);
  BOOL bResult;
  bResult = FALSE;

  if (pObject->toBoolean != NULL)
  {
    bResult = pObject->toBoolean(pObject);
  }
  else
  {
    error(ERROR_LEVEL, "can't convert object type %d into boolean\n", pObject->type);
    ASSERT(FALSE); // should not appear...
  }

  return bResult;
}


BOOL JStringConstante_toBoolean(JObject* pObject)
{
  BOOL r;
  JStringConstante* p = (JStringConstante*) pObject;
  r = TRUE;
  if (strcmp(p->str_constant, "") == 0)
  {
    r = FALSE;
  }
  return r;
}

BOOL JInteger_toBoolean(JObject* pObject)
{
  BOOL r;
  JInteger* p = (JInteger*) pObject;
  r = TRUE;

  if (p->value == 0)
  {
    r = FALSE;
  }

  return r;
}


BOOL JDouble_toBoolean(JObject* pObject)
{
  BOOL r;
  JDouble* p = (JDouble*) pObject;
  r = TRUE;

  if (p->value == 0.0)
  {
    r = FALSE;
  }

  return r;
}

BOOL JBoolean_toBoolean(JObject* pObject)
{
  JBoolean* p = (JBoolean*) pObject;
  return p->value;
}

BOOL JIdentifier_toBoolean(JObject* pObject)
{
  BOOL bOk;
  BOOL isArray;
  int64_t privKey;
  jinjac_parameter param;
  JIdentifier* pIdent;
  pIdent = (JIdentifier*) pObject;
  bOk = parameter_search(pIdent->identifier, &privKey, &isArray);
  if (bOk)
  {
    if (isArray)
    {
      bOk = TRUE;
    }
    else
    {
      parameter_get(privKey, &param);
      switch (param.type)
      {
        case TYPE_STRING:
          if (strcmp(param.value.type_string, "") == 0)
          {
            bOk = FALSE;
          }
          break;
        case TYPE_DOUBLE:
          if (param.value.type_double == 0.0)
          {
            bOk = FALSE;
          }
          break;

        case TYPE_INT:
          if (param.value.type_int == 0)
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
  }

  return bOk;
}

BOOL JArray_toBoolean(JObject* pObject)
{
  BOOL bOk;
  int64_t privKey;
  J_STATUS status;
  jinjac_parameter param;
  JArray* pArray;
  pArray = (JArray*) pObject;

  bOk = parameter_search(pArray->identifier, &privKey, NULL);
  if (bOk)
  {
    BOOL bIsArray;
    bIsArray = parameter_array_getProperties(privKey, &param.type, NULL);
    if (bIsArray)
    {
      status = parameter_array_getValue(privKey, pArray->offset, &param.value);
      if (status == J_OK)
      {
        switch (param.type)
        {
          case TYPE_STRING:
            if (strcmp(param.value.type_string, "") == 0)
            {
              bOk = FALSE;
            }
            break;
          case TYPE_DOUBLE:
            if (param.value.type_double == 0.0)
            {
              bOk = FALSE;
            }
            break;

          case TYPE_INT:
            if (param.value.type_int == 0)
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
        bOk = FALSE;
      }
    }
  }

  return bOk;
}


//TODO Tocheck

BOOL JRange_toBoolean(JObject* pObject)
{
  return ((JBoolean*) pObject)->value;
}

void JObject_delete(JObject* pObject)
{
  if (pObject->delete != NULL)
  {
    pObject->delete (pObject);
  }
  free(pObject);
}

void JStringConstante_delete(JObject* o)
{
  free (((JStringConstante*) o)->str_constant);
}

JObject* JStringConstante_new(char* name)
{
  JStringConstante* o = NEW(JStringConstante);
  o->base.type = J_STR_CONSTANTE;
  o->base.delete = JStringConstante_delete;
  o->base.getValue = JStringConstante_getValue;
  o->base.toBoolean = JStringConstante_toBoolean;
  o->str_constant = name;
  return (JObject*) o;
}

void JIdentifier_delete(JObject* o)
{
  free (((JIdentifier*) o)->identifier);
}

JObject* JIdentifier_new(char* name)
{
  JIdentifier* o = NEW(JIdentifier);
  o->base.type = J_IDENTIFIER;
  o->base.delete = JIdentifier_delete;
  o->base.getValue = JIdentifier_getValue;
  o->base.toBoolean = JIdentifier_toBoolean;
  o->identifier = name;
  return (JObject*) o;
}


JObject* JInteger_new(int i)
{
  JInteger* o = NEW(JInteger);
  o->base.type = J_INTEGER;
  o->base.delete = NULL;
  o->base.getValue = JInteger_getValue;
  o->base.toBoolean = JInteger_toBoolean;
  o->value = i;
  return (JObject*) o;
}

JObject* JDouble_new(double d)
{
  JDouble* o = NEW(JDouble);
  o->base.type = J_DOUBLE;
  o->base.delete = NULL;
  o->base.getValue = JDouble_getValue;
  o->base.toBoolean = JDouble_toBoolean;
  o->value = d;
  return (JObject*) o;
}

JObject* JBoolean_new(BOOL b)
{
  JBoolean* o = NEW(JBoolean);
  o->base.type = J_BOOLEAN;
  o->base.delete = NULL;
  o->base.getValue = JBoolean_getValue;
  o->base.toBoolean = JBoolean_toBoolean;
  o->value = b;
  return (JObject*) o;
}


void JArray_delete(JObject* pObject)
{
  free (((JArray*) pObject)->identifier);
}

JObject* JArray_new(char* name, int offset)
{
  JArray* o = NEW(JArray);
  o->base.type = J_ARRAY;
  o->base.delete = JArray_delete;
  o->base.getValue = JArray_getValue;
  o->base.toBoolean = JArray_toBoolean;
  o->identifier = name;
  o->offset = offset;
  return (JObject*) o;
}

void JRange_delete(JObject* pObject)
{
  JRange* r = ((JRange*) pObject);
  if (r->sequencedObject != NULL)
  {
    JObject_delete(r->sequencedObject);
  }

}

JObject* JRange_new(JObject* objectToBeSequenced, int32_t start, int32_t stop, int32_t step)
{
  JRange* o = NEW(JRange);
  o->base.type = J_RANGE;
  o->base.delete = JRange_delete;
  o->base.getValue = NULL;
  o->base.toBoolean = NULL;
  o->sequencedObject = objectToBeSequenced;
  o->start = start;
  o->stop = stop;
  o->step = step;
  o->currentIndex = start;
  return (JObject*) o;
}

void JFor_delete(JObject* pObject)
{
  JFor* f = ((JFor*) pObject);
  if (f->sequencing != NULL)
  {
    JObject_delete((JObject*) f->sequencing);
  }
  parameter_delete(f->identifierOfIndex);
  free(f->identifierOfIndex);
}


JObject* JFor_new(char* nameIdentifier, JRange* sequence)
{
  JFor* o = NEW(JFor);
  o->base.type = J_FOR;
  o->base.delete = JFor_delete;
  o->base.getValue = NULL;
  o->base.toBoolean = NULL;
  o->identifierOfIndex = nameIdentifier;
  o->sequencing = sequence;
  return (JObject*) o;
}

JObject* JEndFor_new(void)
{
  JEndFor* o = NEW(JEndFor);
  o->base.type = J_END_FOR;
  o->base.delete = NULL;
  o->base.getValue = NULL;
  o->base.toBoolean = NULL;
  return (JObject*) o;
}


J_STATUS JFor_setStartPoint(JFor* obj, int64_t offset, int32_t noLine, BOOL bStripWhiteSpace)
{
  obj->startOffset = offset;
  obj->startLine = noLine;
  obj->bStripWhiteSpace = bStripWhiteSpace;
  return J_OK;
}

J_STATUS JFor_createIndexParameter(JFor* obj)
{
  J_STATUS rc;
  JRange* seq = obj->sequencing;
  rc = J_OK;

  if (seq->sequencedObject == NULL) //default is INT
  {
    jinjac_parameter param;
    param.type = TYPE_INT;
    param.value.type_int = seq->currentIndex;
    jinjac_parameter_insert(obj->identifierOfIndex, &param);
  }
  else
  {
    jinjac_parameter param;
    BOOL isArray;
    BOOL bOk;
    int64_t privKey;
    int32_t nbItems;

    switch (seq->sequencedObject->type)
    {
      case J_IDENTIFIER:
        bOk = parameter_search(((JIdentifier*)seq->sequencedObject)->identifier, &privKey, NULL);
        if (bOk)
        {
          isArray = parameter_array_getProperties(privKey, &param.type, &nbItems);
          if (isArray)
          {
            obj->sequencing->stop = nbItems;
            parameter_array_getValue(privKey, seq->currentIndex, &param.value);
            jinjac_parameter_insert(obj->identifierOfIndex, &param);
          }
          else
          {
            obj->sequencing->stop = 0;
          }
        }
        else
        {
          error(WARNING_LEVEL, "%s unknow identifier\n", ((JIdentifier*) seq->sequencedObject)->identifier);
          obj->sequencing->stop = 0;
          rc = J_ERROR;
        }
        break;

      default:
        error(ERROR_LEVEL, "type = %d\n", seq->sequencedObject->type);
        rc = J_ERROR;
        ASSERT(FALSE);  //TODO
        break;
    }
  }

  return rc;
}

BOOL JFor_isDone(JFor* obj)
{
  JRange* r = obj->sequencing;
  BOOL bDone;
  bDone = FALSE;

  if (r->currentIndex >= r->stop)
  {
    bDone = TRUE;
  }

  return bDone;
}


BOOL JRange_step(JRange* obj, char* indexIdentifierName)
{
  BOOL isDone;
  isDone = FALSE;
  ASSERT(obj != NULL);
  ASSERT(indexIdentifierName != NULL);

  if (obj->sequencedObject == NULL)
  {
    obj->currentIndex += obj->step;
    if (obj->currentIndex >= obj->stop)
    {
      isDone = TRUE;
    }

    parameter_update(indexIdentifierName, (jinjac_parameter_value) obj->currentIndex);
  }
  else
  {
    obj->currentIndex++;
    if (obj->currentIndex >= obj->stop)
    {
      isDone = TRUE;
    }
    else
    {
      jinjac_parameter_value paramValue;
      int64_t privKey;
      BOOL bOk;
      BOOL bIsArray;

      switch (obj->sequencedObject->type)
      {
        case J_IDENTIFIER:
          bOk = parameter_search(((JIdentifier*) obj->sequencedObject)->identifier, &privKey, &bIsArray);
          ASSERT(bOk == TRUE);
          if ((bOk) && (bIsArray))
          {
            J_STATUS status = parameter_array_getValue(privKey, obj->currentIndex, &paramValue);
            if (status == J_OK)
            {
              parameter_update(indexIdentifierName, paramValue);
            }
            else
            {
              ASSERT(FALSE);
            }
          }
          break;

        default:
          ASSERT(FALSE);  //TODO
          break;
      }
    }
  }

  return isDone;
}


void JIF_delete(JObject* pObject)
{
  JIF* f = ((JIF*) pObject);
  if (f->condition != NULL)
  {
    JObject_delete((JObject*) f->condition);
  }
}


JObject* JIF_new(JObject* condition)
{
  JIF* o = NEW(JIF);
  o->base.type = J_IF;
  o->base.delete = JIF_delete;
  o->base.getValue = NULL;
  o->base.toBoolean = NULL;
  o->condition = condition;
  return (JObject*) o;
}

BOOL JIF_getIfConditionIsActive(JIF* pIf)
{
  BOOL b;

  if (pIf->condition->type == J_BOOLEAN)
  {
    b = ((JBoolean*) pIf->condition)->value;
  }
  else
  {
    b = pIf->condition->toBoolean(pIf->condition);
  }

  return b;
}

JObject* JEndIf_new(void)
{
  JEndIf* o = NEW(JEndIf);
  o->base.type = J_END_IF;
  o->base.delete = NULL;
  o->base.getValue = NULL;
  o->base.toBoolean = NULL;
  return (JObject*) o;
}

extern JObject* JElse_new(void)
{
  JElse* o = NEW(JElse);
  o->base.type = J_ELSE;
  o->base.delete = NULL;
  o->base.getValue = NULL;
  o->base.toBoolean = NULL;
  return (JObject*) o;
}


char* JObject_toString(JObject* pObject)
{
  jinjac_parameter param;
  BOOL bOk;
  char* s;
  s = NULL;

  bOk = JObject_getValue(pObject, &param);
  if (bOk)
  {
    switch (param.type)
    {
      case TYPE_STRING:
        s = param.value.type_string;
        break;

      case TYPE_DOUBLE:
        s = doubleToStr(param.value.type_double);
        break;

      case TYPE_INT:
        s = intToStr(param.value.type_int);
        break;

      default:
        trace("type =%d\n", param.type);
        ASSERT(FALSE);
        s = NULL;
        break;
    }
  }

  return s;
}

JRange* JObject_toRange(JObject* pObject)
{
  ASSERT(pObject != NULL);
  JRange* jRange;

  jRange = (JRange*) JRange_new(pObject, 0, 1, 1);

  return jRange;
}

int32_t JObject_toInteger(JObject* obj)
{
  ASSERT(obj != NULL);
  jinjac_parameter param;
  BOOL bOk;
  int32_t r;
  r = 0;

  bOk = JObject_getValue(obj, &param);
  if (bOk)
  {
    switch (param.type)
    {
      case TYPE_STRING:
        //do nothing
        break;

      case TYPE_DOUBLE:
        r = (int32_t) param.value.type_double;
        break;

      case TYPE_INT:
        r = param.value.type_int;
        break;

      default:
        ASSERT(FALSE);
        break;
    }
  }

  return r;
}



BOOL isTypeOkForCalcul(jinjac_parameter_type type)
{
  BOOL bOk;
  bOk = TRUE;

  if (type == TYPE_STRING)
  {
    bOk = FALSE;
  }

  return bOk;
}

typedef struct
{
  jinjac_parameter_type type_op1;
  jinjac_parameter_type type_op2;
  jinjac_parameter_type type_result;
} op_decision;

enum
{
  OPERATION_DDD,
  OPERATION_DID,
  OPERATION_IDD,
  OPERATION_III
};

STATIC const op_decision operation_array_decision[] =
{
  { TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE }, //, calcul_ddd },
  { TYPE_DOUBLE, TYPE_INT, TYPE_DOUBLE }, //, calcul_did },
  { TYPE_INT, TYPE_DOUBLE, TYPE_DOUBLE }, //, calcul_idd },
  { TYPE_INT, TYPE_INT, TYPE_INT } //, calcul_iii},
};

STATIC int32_t select_operation(jinjac_parameter_type t1, jinjac_parameter_type t2)
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

JObject* JObject_doOperation(JObject* op1, JObject* op2, char mathOperation)
{
  ASSERT(op1 != NULL);
  ASSERT(op2 != NULL);

  JObject* pObjResult = NULL;
  BOOL bOk1, bOk2;

  //object for mathematical can'not be string.
  jinjac_parameter paramOp1;
  jinjac_parameter paramOp2;

  bOk1 = JObject_getValue(op1, &paramOp1);
  bOk2 = JObject_getValue(op2, &paramOp2);

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
          pObjResult = JDouble_new(r);
        }
        break;

      case OPERATION_DID:
        {
          double r = calcul_did(paramOp1.value.type_double, paramOp2.value.type_int, mathOperation);
          pObjResult = JDouble_new(r);
        }
        break;

      case OPERATION_IDD:
        {
          double r = calcul_idd(paramOp1.value.type_int, paramOp2.value.type_double, mathOperation);
          pObjResult = JDouble_new(r);
        }
        break;

      case OPERATION_III:
        {
          int r = calcul_iii(paramOp1.value.type_int, paramOp2.value.type_int, mathOperation);
          pObjResult = JInteger_new(r);
        }
        break;

      default:
        ASSERT(FALSE);
        break;
    }
  }

  return pObjResult;
}


JObject* JObject_execComparison(JObject* op1, JObject* op2, jobject_condition condition)
{
  JObject* pObjectResult;
  pObjectResult = NULL;

  //BOOL bOk1;
  //BOOL bOk2;
  BOOL bComparisonResult;
  jinjac_parameter paramOp1;
  jinjac_parameter paramOp2;

  /*bOk1 =*/ JObject_getValue(op1, &paramOp1);
  /*bOk2 =*/ JObject_getValue(op2, &paramOp2);

  if ((paramOp1.type == TYPE_STRING) && (paramOp2.type == TYPE_STRING))
  {
    if (((condition == AST_EQUAL) && (strcmp(paramOp1.value.type_string, paramOp2.value.type_string) == 0)) ||
        ((condition == AST_DIFFERENT) && (strcmp(paramOp1.value.type_string, paramOp2.value.type_string) != 0)))
    {
      pObjectResult = JBoolean_new(TRUE);
    }
    else
    {
      pObjectResult = JBoolean_new(FALSE);
    }
    param_delete(&paramOp1);
    param_delete(&paramOp2);
  }
  else if ((paramOp1.type == TYPE_STRING) || (paramOp2.type == TYPE_STRING))
  {
    //trace("one of the two operation is a string ==> FALSE\n");
    pObjectResult = JBoolean_new(FALSE);
    param_delete(&paramOp1);
    param_delete(&paramOp2);
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

    pObjectResult = JBoolean_new(bComparisonResult);
  }

  return pObjectResult;
}


