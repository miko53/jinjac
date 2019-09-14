#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jobject.h"
#include "buildin.h"
#include "convert.h"
#include "ast.h"

#define NEW(obj)  malloc(sizeof(obj))

typedef char* (*buildin_fct)(char*, ...);

typedef enum
{
  INT,
  DOUBLE,
  STRING,
  BOOLEAN
} args_type;

typedef struct
{
  buildin_fct fct;
  const char* name;
  int nb_args;
  args_type args_type[NB_MAX_ARGS];
  void* args_default[NB_MAX_ARGS];
} fct_converter;


fct_converter tab_fct_converter[] =
{
  { .fct = (buildin_fct) capitalize, .name = "capitalize", .nb_args = 0 },
  {
    .fct = (buildin_fct) center, .name = "center", .nb_args = 1,
    .args_type = { INT},
    .args_default = { (void*) 80 }
  },
  { .fct = (buildin_fct) format, .name = "format", .nb_args = NB_MAX_ARGS },
  { .fct = (buildin_fct) lower, .name = "lower", .nb_args = 0 },
  { .fct = (buildin_fct) upper, .name = "upper", .nb_args = 0 },
  {
    .fct = NULL, .name = "range", .nb_args = 3,
    .args_type = { INT, INT, INT},
    .args_default = { 0, 0, (void*) 1}
  },
  { .fct = (buildin_fct) title, .name = "title", .nb_args = 0 },
  { .fct = (buildin_fct) trim, .name = "trim", .nb_args = 0},
  {
    .fct = (buildin_fct) truncate, .name = "truncate", .nb_args = 4,
    .args_type = { INT, BOOLEAN, STRING, INT},
    .args_default = { (void*) 255,  (void*) FALSE, "...", 0 }
  },
};


static int getFunctionID(char* fctName)
{
  int sizeMax = sizeof(tab_fct_converter) / sizeof(fct_converter);
  int i;

  for (i = 0; i < sizeMax; i++)
  {
    if (strcmp(tab_fct_converter[i].name, fctName) == 0)
    {
      return i;
    }
  }

  fprintf(stdout, "error: function %s not found !", fctName);
  return -1;
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
  o->identifier = name;
  return (JObject*) o;
}


JObject* JInteger_new(int i)
{
  JInteger* o = NEW(JInteger);
  o->base.type = J_INTEGER;
  o->base.delete = NULL;
  o->value = i;
  return (JObject*) o;
}

JObject* JDouble_new(double d)
{
  JDouble* o = NEW(JDouble);
  o->base.type = J_DOUBLE;
  o->base.delete = NULL;
  o->value = d;
  return (JObject*) o;
}

JObject* JBoolean_new(BOOL b)
{
  JBoolean* o = NEW(JBoolean);
  o->base.type = J_BOOLEAN;
  o->base.delete = NULL;
  o->value = b;
  return (JObject*) o;
}

void JFunction_delete(JObject* pObject)
{
  JFunction* fct = ((JFunction*) pObject);
  if (fct->argList != NULL)
  {
    JObject_delete((JObject*) fct->argList);
  }
}

JObject* JFunction_new(char* fct)
{
  int functionID = getFunctionID(fct);
  if (functionID != -1)
  {
    JFunction* o = NEW(JFunction);
    o->base.type = J_FUNCTION;
    o->base.delete = JFunction_delete;
    o->argList = NULL;
    o->functionID = functionID;
    return (JObject*) o;
  }
  else
  {
    fprintf(stdout, "filtered fct not found !\n");
  }

  return NULL;
}

void JArgs_delete(JObject* pObject)
{
  JArgs* args = ((JArgs*) pObject);
  int i;
  for (i = 0; i < args->nb_args; i++)
  {
    JObject_delete(args->listArgs[i]);
    args->listArgs[i] = NULL;
  }
  args->nb_args = 0;

}

JObject* JArgs_new(void)
{
  JArgs* o = NEW(JArgs);
  o->base.type = J_FUNCTION_ARGS;
  o->base.delete = JArgs_delete;
  o->nb_args = 0;
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
  o->identifierOfIndex = nameIdentifier;
  o->sequencing = sequence;
  return (JObject*) o;
}

JObject* JEndFor_new(void)
{
  JEndFor* o = NEW(JEndFor);
  o->base.type = J_END_FOR;
  o->base.delete = NULL;
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
    insert_parameter(obj->identifierOfIndex, TYPE_INT, (parameter_value) seq->currentIndex);
  }
  else
  {
    parameter_type paramType;
    parameter_value paramValue;
    BOOL isArray;
    int nbItems;

    switch (seq->sequencedObject->type)
    {
      case J_IDENTIFIER:
        isArray = param_isArray(((JIdentifier*)seq->sequencedObject)->identifier, &nbItems);
        if (isArray)
        {
          obj->sequencing->stop = nbItems;
          paramType = param_getType(((JArray*)seq->sequencedObject)->identifier);
          ASSERT(paramType != TYPE_UNKOWN);
          param_array_getValue(((JArray*) seq->sequencedObject)->identifier, seq->currentIndex, &paramValue);
          insert_parameter(obj->identifierOfIndex, paramType, paramValue);
        }
        else
        {
          fprintf(stdout, "can't convert %s into range\n", ((JIdentifier*) seq->sequencedObject)->identifier);
          rc = -1;
        }

        break;


      default:
        fprintf(stdout, "type = %d\n", seq->sequencedObject->type);
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

    update_parameter(indexIdentifierName, (parameter_value) obj->currentIndex);
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
          param_array_getValue(((JIdentifier*) obj->sequencedObject)->identifier, obj->currentIndex, &paramValue);
          update_parameter(indexIdentifierName, paramValue);
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
  parameter_value v;
  parameter_type type;
  char* s;

  v = JObject_getValue(pObject, &type);
  switch (type)
  {
    case TYPE_STRING:
      s = v.type_string;
      break;

    case TYPE_DOUBLE:
      s = doubleToStr(v.type_double);
      break;

    case TYPE_INT:
      s = intToStr(v.type_int);
      break;

    case TYPE_UNKOWN:
      fprintf(stdout, "can't convert to string, unknown type error\n");
      ast_setInError("TYPE UNKOWN1");
      s = NULL;
      break;

    default:
      fprintf(stdout, "type =%d\n", type);
      ASSERT(FALSE);
      s = NULL;
      break;
  }

  return s;
}


parameter_value JObject_getValue(JObject* pObject, parameter_type* pType)
{
  ASSERT(pObject != NULL);
  parameter_value s;
  s.type_string = NULL;

  switch (pObject->type)
  {
    case J_STR_CONSTANTE:
      s.type_string = strdup(((JStringConstante*) pObject)->str_constant);
      if (pType)
      {
        *pType = TYPE_STRING;
      }
      break;

    case J_BOOLEAN:
      s.type_int = ((JBoolean*) pObject)->value;
      if (pType)
      {
        *pType = TYPE_INT;
      }
      break;

    case J_INTEGER:
      s.type_int = (((JInteger*) pObject)->value);
      if (pType)
      {
        *pType = TYPE_INT;
      }
      break;

    case J_DOUBLE:
      s.type_double = (((JDouble*) pObject)->value);
      if (pType)
      {
        *pType = TYPE_DOUBLE;
      }
      break;

    case J_IDENTIFIER:
      {
        JIdentifier* pIdent;
        pIdent = (JIdentifier*) pObject;
        parameter_type type = param_getType(pIdent->identifier);
        switch (type)
        {
          case TYPE_STRING:
            s.type_string = strdup((char*) param_getValue(pIdent->identifier).type_string);
            break;

          case TYPE_INT:
          case TYPE_DOUBLE:
            s =  param_getValue(pIdent->identifier);
            break;

          default:
            ast_setInError("UNKOWN IDENTIFIER");
            fprintf(stdout, "unknown '%s' identifier\n", pIdent->identifier);
            ASSERT(FALSE); //TODO better management this kind of error can happened
            break;
        }
        if (pType)
        {
          *pType = type;
        }
      }
      break;

    case J_ARRAY:
      {
        JArray* pArray;
        pArray = (JArray*) pObject;
        BOOL bOk;
        parameter_value v;
        parameter_type type = param_getType(pArray->identifier);
        bOk = param_array_getValue(pArray->identifier, pArray->offset, &v);
        if (bOk)
        {
          switch (type)
          {
            case TYPE_STRING:
              s.type_string = strdup((char*) v.type_string);
              break;

            case TYPE_DOUBLE:
            case TYPE_INT:
              s = v;
              break;

            default:
              ast_setInError("UNKOWN ARRAY");
              fprintf(stdout, "unknown '%s' array\n", pArray->identifier);
              ASSERT(FALSE);
              break;
          }
          if (pType)
          {
            *pType = type;
          }
        }
        else
        {
          if (pType)
          {
            *pType = TYPE_UNKOWN;
          }
        }

      }
      break;

    case J_FUNCTION:
      fprintf(stdout, "a function can not be converted to value\n");
      ASSERT(FALSE);
      break;

    default:
      fprintf(stdout, "not yet implemented...(type = %d)\n", pObject->type);
      break;
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


int JObject_toIntValue(JObject* obj)
{
  ASSERT(obj != NULL);
  parameter_type type;
  parameter_value value = JObject_getValue(obj, &type);
  int r;
  r = 0;

  switch (type)
  {
    case TYPE_STRING:
    case TYPE_UNKOWN:
      //do nothing
      break;

    case TYPE_DOUBLE:
      r = (int) value.type_double;
      break;

    case TYPE_INT:
      r = value.type_int;
      break;

    default:
      ASSERT(FALSE);
      break;
  }
  return r;
}




JObject* JFunction_execute(JFunction* f, JObject* pCurrentObject)
{
  JObject* resultObject;
  char* s;
  fct_converter* fct_item;

  resultObject = NULL;
  s = NULL;
  //s = currentStringValue;

  fct_item = &tab_fct_converter[f->functionID];
  switch (f->functionID)
  {
    //function with no argument (except filtering string)
    case FCT_CAPITALIZE:
    case FCT_LOWER:
    case FCT_UPPER:
    case FCT_TITLE:
    case FCT_TRIM:
      if ((f->argList != NULL) && (f->argList->nb_args != 0))
      {
        fprintf(stdout, "warning! unexpected number of arguments for function %s\n", fct_item->name);
      }
      else
      {
        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = fct_item->fct(s);
          resultObject = JStringConstante_new(s);
        }
      }
      break;

    //function with 4 arguments
    case FCT_TRUNCATE:
      {
        int minNbArgs = 0;
        void* a[4];

        if (f->argList != NULL)
        {
          minNbArgs = f->argList->nb_args > fct_item->nb_args ? fct_item->nb_args : f->argList->nb_args;
        }

        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case DOUBLE:
              break;

            case INT:
            case BOOLEAN:
              a[i] = (void*) (long) JObject_toIntValue(f->argList->listArgs[i]);
              break;

            case STRING:
              a[i] = (void*) JObject_toString(f->argList->listArgs[i]);
              break;

            default:
              ASSERT(FALSE);
              break;
          }
        }

        //set default value now.
        for (int i = minNbArgs; i < fct_item->nb_args; i++)
        {
          a[i] = fct_item->args_default[i];
        }

        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = fct_item->fct(s, a[0], a[1], a[2], a[3]);
          resultObject = JStringConstante_new(s);
        }

        //dessallocate the allocated string argument after execution
        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case STRING:
              free(a[i]);
              break;

            default:
              break;
          }
        }
      }
      break;

    //function with one argument
    case FCT_CENTER:
      {
        int minNbArgs = 0;
        void* a[1];

        if (f->argList != NULL)
        {
          minNbArgs = f->argList->nb_args > fct_item->nb_args ? fct_item->nb_args : f->argList->nb_args;
        }

        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case DOUBLE:
              break;

            case INT:
            case BOOLEAN:
              a[i] = (void*) (long) JObject_toIntValue(f->argList->listArgs[i]);
              break;

            case STRING:
              a[i] = (void*) JObject_toString(f->argList->listArgs[i]);
              break;

            default:
              ASSERT(FALSE);
              break;
          }
        }

        //set default value now.
        for (int i = minNbArgs; i < fct_item->nb_args; i++)
        {
          a[i] = fct_item->args_default[i];
        }

        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = fct_item->fct(s, a[0]);
          resultObject = JStringConstante_new(s);
        }

        //dessallocate the allocated string argument after execution
        for (int i = 0; i < minNbArgs; i++)
        {
          switch (fct_item->args_type[i])
          {
            case STRING:
              free(a[i]);
              break;

            default:
              break;
          }
        }
      }
      break;

    case FCT_FORMAT:
      {
        int nbArgs = 0;
        parameter_value a[NB_MAX_ARGS];
        parameter_type t[NB_MAX_ARGS];

        if (f->argList != NULL)
        {
          nbArgs = f->argList->nb_args;
        }
        if (nbArgs > 10)
        {
          nbArgs = 10;
        }

        for (int i = 0; i < nbArgs; i++)
        {
          parameter_value v;
          parameter_type type;
          v = JObject_getValue(f->argList->listArgs[i], &type);
          a[i] = v;
          t[i] = type;
        }

        for (int i = nbArgs; i < NB_MAX_ARGS; i++)
        {
          a[i].type_string = NULL;
          t[i] = TYPE_STRING;
        }

        s = JObject_toString(pCurrentObject);
        if (s != NULL)
        {
          s = format(s, nbArgs, a, t);
          resultObject = JStringConstante_new(s);
        }

        //dessallocate the allocated string argument after execution
        for (int i = 0; i < nbArgs; i++)
        {
          switch (t[i])
          {
            case TYPE_STRING:
              free(a[i].type_string);
              break;

            default:
              break;
          }
        }
      }
      break;

    case FCT_RANGE:
      //give number of argument
      {
        int nbArgs = f->argList->nb_args;
        switch (nbArgs)
        {
          case 0: // not possible error...
            break;

          case 1: //only range(stop)
            resultObject = JRange_new(NULL, 0, JObject_toIntValue(f->argList->listArgs[0]), 1);
            break;

          case 2: //only range(start, stop)
            resultObject = JRange_new(NULL, JObject_toIntValue(f->argList->listArgs[0]),
                                      JObject_toIntValue(f->argList->listArgs[1]), 1);
            break;

          default:
          case 3: // range (start, stop, step) ... extras args are ignored
            resultObject = JRange_new(NULL, JObject_toIntValue(f->argList->listArgs[0]),
                                      JObject_toIntValue(f->argList->listArgs[1]), JObject_toIntValue(f->argList->listArgs[2]));
            break;
        }
      }
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  //return s;
  return resultObject;
}

int JArgs_insert_args(JArgs* obj, JObject* argToInsert)
{
  ASSERT(obj != NULL);
  ASSERT(argToInsert != NULL);
  int rc;

  if (obj->nb_args > NB_MAX_ARGS)
  {
    rc = -1;
  }
  else
  {
    obj->listArgs[obj->nb_args] = argToInsert;
    obj->nb_args++;
    rc = 0;
  }

  return rc;
}

BOOL isTypeOkForCalcul(parameter_type type)
{
  BOOL bOk;
  bOk = TRUE;

  if ((type == TYPE_STRING) ||
      (type == TYPE_UNKOWN))
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

static const op_decision operation_array_decision[] =
{
  { TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE}, //, calcul_ddd },
  { TYPE_DOUBLE, TYPE_INT, TYPE_DOUBLE}, //, calcul_did },
  { TYPE_INT, TYPE_DOUBLE, TYPE_DOUBLE}, //, calcul_idd },
  { TYPE_INT, TYPE_INT, TYPE_INT} //, calcul_iii},
};

static int select_operation(parameter_type t1, parameter_type t2)
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
  BOOL bOk;

  //object for mathematical can'not be string.
  parameter_value valueOp1;
  parameter_type  typeOp1;
  parameter_value valueOp2;
  parameter_type  typeOp2;

  valueOp1 = JObject_getValue(op1, &typeOp1);
  valueOp2 = JObject_getValue(op2, &typeOp2);

  bOk = isTypeOkForCalcul(typeOp1);
  if (bOk)
  {
    bOk = isTypeOkForCalcul(typeOp2);
  }
  else
  {
    if (valueOp1.type_string != NULL) //NOTE: TYPE_UNKOWN return a string NULL which can't be freed
    {
      free(valueOp1.type_string);
    }
  }

  if (!bOk)
  {
    if (valueOp2.type_string != NULL) //NOTE: TYPE_UNKOWN return a string NULL which can't be freed
    {
      free(valueOp2.type_string);
    }

    ast_setInError("Can't do calcul wrong operande type");
  }
  else
  {
    int s = select_operation(typeOp1, typeOp2);
    switch (s)
    {
      case OPERATION_DDD:
        {
          double r = calcul_ddd(valueOp1.type_double, valueOp2.type_double, mathOperation);
          pObjResult = JDouble_new(r);
        }
        break;

      case OPERATION_DID:
        {
          double r = calcul_did(valueOp1.type_double, valueOp2.type_int, mathOperation);
          pObjResult = JDouble_new(r);
        }
        break;

      case OPERATION_IDD:
        {
          double r = calcul_idd(valueOp1.type_int, valueOp2.type_double, mathOperation);
          pObjResult = JDouble_new(r);
        }
        break;

      case OPERATION_III:
        {
          int r = calcul_iii(valueOp1.type_int, valueOp2.type_int, mathOperation);
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


