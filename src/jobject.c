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

BOOL JObject_getValue(JObject* pObject, parameter* param)
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
    error("can't convert object type %d into value\n", pObject->type);
  }

  return bOk;
}

BOOL JStringConstante_getValue(struct JObjects* pObject, parameter* param)
{
  param->value.type_string = strdup(((JStringConstante*) pObject)->str_constant);
  param->type = TYPE_STRING;
  return TRUE;
}

BOOL JBoolean_getValue(struct JObjects* pObject, parameter* param)
{
  param->value.type_int = ((JBoolean*) pObject)->value;
  param->type = TYPE_INT;
  return TRUE;
}


BOOL JInteger_getValue(struct JObjects* pObject, parameter* param)
{
  param->value.type_int = (((JInteger*) pObject)->value);
  param->type = TYPE_INT;
  return TRUE;
}

BOOL JDouble_getValue(struct JObjects* pObject, parameter* param)
{
  param->value.type_double = (((JDouble*) pObject)->value);
  param->type = TYPE_DOUBLE;
  return TRUE;
}

BOOL JIdentifier_getValue(struct JObjects* pObject, parameter* param)
{
  BOOL bOk;
  BOOL isArray;
  JIdentifier* pIdent;
  pIdent = (JIdentifier*) pObject;
  bOk = parameter_get(pIdent->identifier, param, &isArray);
  if (!bOk)
  {
    error("warning: unknown '%s' identifier\n", pIdent->identifier);
    param->type = TYPE_STRING;
    param->value.type_string = strdup("");
    bOk = TRUE;
  }
  else
  {
    if (isArray)
    {
      bOk = TRUE;
      param->value.type_string = parameter_convertArrayToString(pIdent->identifier);
      param->type = TYPE_STRING;
      trace("array ==> %s\n", param->value.type_string);
    }
    else if (param->type == TYPE_STRING)
    {
      param->value.type_string = strdup(param->value.type_string);
    }
  }
  return bOk;
}


BOOL JArray_getValue(struct JObjects* pObject, parameter* param)
{
  BOOL bOk;
  JArray* pArray;
  pArray = (JArray*) pObject;
  bOk = parameter_array_getProperties(pArray->identifier, &param->type, NULL);
  if (bOk)
  {
    bOk = parameter_array_getValue(pArray->identifier, pArray->offset, &param->value);
    if (bOk)
    {
      if (param->type == TYPE_STRING)
      {
        param->value.type_string = strdup(param->value.type_string);
      }
    }
  }
  return bOk;
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
  o->identifier = name;
  return (JObject*) o;
}


JObject* JInteger_new(int i)
{
  JInteger* o = NEW(JInteger);
  o->base.type = J_INTEGER;
  o->base.delete = NULL;
  o->base.getValue = JInteger_getValue;
  o->value = i;
  return (JObject*) o;
}

JObject* JDouble_new(double d)
{
  JDouble* o = NEW(JDouble);
  o->base.type = J_DOUBLE;
  o->base.delete = NULL;
  o->base.getValue = JDouble_getValue;
  o->value = d;
  return (JObject*) o;
}

JObject* JBoolean_new(BOOL b)
{
  JBoolean* o = NEW(JBoolean);
  o->base.type = J_BOOLEAN;
  o->base.delete = NULL;
  o->base.getValue = JBoolean_getValue;
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

JObject* JRange_new(JObject* objectToBeSequenced, int start, int stop, int step)
{
  JRange* o = NEW(JRange);
  o->base.type = J_RANGE;
  o->base.delete = JRange_delete;
  o->base.getValue = NULL;
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
  free(f->identifierOfIndex);
}


JObject* JFor_new(char* nameIdentifier, JRange* sequence)
{
  JFor* o = NEW(JFor);
  o->base.type = J_FOR;
  o->base.delete = JFor_delete;
  o->base.getValue = NULL;
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
  return (JObject*) o;
}

int JFor_setStartPoint(JFor* obj, long offset)
{
  obj->startOffset = offset;
  return 0;
}

int JFor_createIndexParameter(JFor* obj)
{
  int rc;
  JRange* seq = obj->sequencing;
  rc = 0;

  if (seq->sequencedObject == NULL) //default is INT
  {
    parameter param;
    param.type = TYPE_INT;
    param.value.type_int = seq->currentIndex;
    parameter_insert(obj->identifierOfIndex, &param);
  }
  else
  {
    parameter param;
    BOOL isArray;
    int nbItems;

    switch (seq->sequencedObject->type)
    {
      case J_IDENTIFIER:
        isArray = parameter_array_getProperties(((JIdentifier*)seq->sequencedObject)->identifier, &param.type, &nbItems);
        if (isArray)
        {
          obj->sequencing->stop = nbItems;
          parameter_array_getValue(((JArray*) seq->sequencedObject)->identifier, seq->currentIndex, &param.value);
          parameter_insert(obj->identifierOfIndex, &param);
        }
        else
        {
          error("warning: %s unknow identifier\n", ((JIdentifier*) seq->sequencedObject)->identifier);
          obj->sequencing->stop = 0;
          rc = -1;
        }

        break;

      default:
        error("type = %d\n", seq->sequencedObject->type);
        rc = -1;
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

    parameter_update(indexIdentifierName, (parameter_value) obj->currentIndex);
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
      parameter_value paramValue;
      switch (obj->sequencedObject->type)
      {
        case J_IDENTIFIER:
          parameter_array_getValue(((JIdentifier*) obj->sequencedObject)->identifier, obj->currentIndex, &paramValue);
          parameter_update(indexIdentifierName, paramValue);
          break;

        default:
          ASSERT(FALSE);  //TODO
          break;
      }
    }
  }

  return isDone;
}

char* JObject_toString(JObject* pObject)
{
  parameter param;
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

int JObject_toInteger(JObject* obj)
{
  ASSERT(obj != NULL);
  parameter param;
  BOOL bOk;
  int r;
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
        r = (int) param.value.type_double;
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



BOOL isTypeOkForCalcul(parameter_type type)
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
  parameter_type type_op1;
  parameter_type type_op2;
  parameter_type type_result;
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

STATIC int select_operation(parameter_type t1, parameter_type t2)
{
  unsigned int i;

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
  parameter paramOp1;
  parameter paramOp2;

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
    int s = select_operation(paramOp1.type, paramOp2.type);
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


