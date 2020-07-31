
#include "buildin.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "j_value.h"
#include "stack.h"
#include "common.h"
#include "str_obj.h"
#include "trace.h"

static char* upper(char* s);
static char* lower(char* s);
static char* capitalize(char* s);
static char* title(char* s);
static char* trim(char* s);
static char* truncate(char* origin, uint32_t truncSize, BOOL killwords, char* endSentence, uint32_t tolerateMargin);
static char* center(char* origin, uint32_t width);
static char* format(char* origin, int32_t nbParameters, j_value* param);

static j_object* vm_exe_1arg(vm_desc* pVM, buildin_fct_id fctId, j_object* args);
static j_object* vm_exe_truncate(vm_desc* pVM, j_object* args[]);
static j_object* vm_exe_center(vm_desc* pVM, j_object* args[]);
static j_object* vm_exe_join(vm_desc* pVM, j_object* args[]);
static j_object* vm_exe_format(vm_desc* pVM, uint32_t nbArgs, j_object* args[]);
static j_object* vm_exe_range(vm_desc* pVM, uint32_t nbArgs, j_object* args[]);

int32_t buildin_execute(vm_desc* pVM, buildin_fct_id fctId, uint32_t nbArgs)
{
  J_STATUS rc;
  rc = J_ERROR;
  j_object* args[nbArgs];
  j_object* pResult;
  pResult = NULL;

  for (uint32_t i = 0; i < nbArgs; i++)
  {
    args[i] = stack_top_n(&pVM->stack, 0 - i);
    if (args[i] == NULL)
    {
      rc = J_ERROR;
      for (uint32_t j = 0; j < i; j++)
      {
        j_object_delete(args[j]);
      }
      return rc;
    }
  }

  //do execution here
  switch (fctId)
  {
    case FCT_CAPITALIZE:
    case FCT_LOWER:
    case FCT_UPPER:
    case FCT_TITLE:
    case FCT_TRIM:
      pResult = vm_exe_1arg(pVM, fctId, args[0]);
      break;

    case FCT_CENTER:
      pResult = vm_exe_center(pVM, args);
      break;

    case FCT_JOIN:
      pResult = vm_exe_join(pVM, args);
      break;

    case FCT_FORMAT:
      pResult = vm_exe_format(pVM, nbArgs, args);
      break;

    case FCT_RANGE:
      pResult = vm_exe_range(pVM, nbArgs, args);
      break;

    case FCT_TRUNCATE:
      if (nbArgs >= 5)
      {
        pResult = vm_exe_truncate(pVM, args);
      }
      break;

    default:
      break;
  }

  for (uint32_t i = 0; i < nbArgs; i++)
  {
    j_object_delete(args[i]);
    stack_pop(&pVM->stack);
  }

  if (pResult != NULL)
  {
    rc = J_OK;
    stack_push(&pVM->stack, pResult);
  }
  return rc;
}

j_object* vm_exe_1arg(vm_desc* pVM, buildin_fct_id fctId, j_object* args)
{
  char* pIn;
  char* pOut;
  j_object* pResult;
  (void) pVM;

  switch (fctId)
  {
    case FCT_CAPITALIZE:
      pIn = j_object_toString(args);
      pOut = capitalize(pIn);
      pResult = (j_object*) j_object_string_new(pOut);
      break;

    case FCT_LOWER:
      pIn = j_object_toString(args);
      pOut = lower(pIn);
      pResult = (j_object*) j_object_string_new(pOut);
      break;

    case FCT_UPPER:
      pIn = j_object_toString(args);
      pOut = upper(pIn);
      pResult = (j_object*) j_object_string_new(pOut);
      break;

    case FCT_TITLE:
      pIn = j_object_toString(args);
      pOut = title(pIn);
      pResult = (j_object*) j_object_string_new(pOut);
      break;

    case FCT_TRIM:
      pIn = j_object_toString(args);
      pOut = trim(pIn);
      pResult = (j_object*) j_object_string_new(pOut);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return pResult;
}

j_object* vm_exe_truncate(vm_desc* pVM, j_object* args[])
{
  uint32_t truncSize;
  BOOL killwords;
  char* endSentence;
  uint32_t tolerateMargin;

  UNUSED(pVM);

  j_object* pObject = args[0];
  j_object* pObjectTruncSize = args[1];
  j_object* pObjectKillWords = args[2];
  j_object* pObjectendSentence = args[3];
  j_object* pObjectTolerateMargin = args[4];

  j_object_string* pResult = NULL;
  char* s;
  char* o;

  if ((pObject != NULL) &&
      (pObjectTruncSize != NULL) &&
      (pObjectKillWords != NULL) &&
      (pObjectendSentence != NULL) &&
      (pObjectTolerateMargin != NULL))
  {
    o = j_object_toString(pObject);
    truncSize = j_object_toInteger(pObjectTruncSize);
    killwords = j_object_toInteger(pObjectKillWords);
    endSentence = j_object_toString(pObjectendSentence);
    tolerateMargin = j_object_toInteger(pObjectTolerateMargin);

    s = truncate(o, truncSize, killwords, endSentence, tolerateMargin);
    free(o);
    free(endSentence);
    pResult = j_object_string_new(s);
  }

  return (j_object*) pResult;
}


j_object* vm_exe_center(vm_desc* pVM, j_object* args[])
{
  uint32_t width;
  j_object* pObject = args[0];
  j_object* pObjectWidth = args[1];
  j_object_string* pResult = NULL;
  char* s;
  char* o;

  UNUSED(pVM);

  if ((pObject != NULL) &&
      (pObjectWidth != NULL))
  {
    o = j_object_toString(pObject);
    width = j_object_toInteger(pObjectWidth);
    s = center(o, width);
    free(o);
    pResult = j_object_string_new(s);
  }

  return (j_object*) pResult;
}

j_object* vm_exe_format(vm_desc* pVM, uint32_t nbArgs, j_object* args[])
{
  j_value value[nbArgs];
  //BOOL b;
  char* pS;
  char* o;
  j_object_string* pResult = NULL;
  j_object* pObject = args[0];

  UNUSED(pVM);

  for (uint32_t i = 1; i < nbArgs; i++)
  {
    /*b = */j_object_getValue(args[i], &value[i - 1]);
  }

  o = j_object_toString(pObject);

  pS = format(o, nbArgs - 1, value);
  pResult = j_object_string_new(pS);

  for (uint32_t i = 0; i < nbArgs - 1; i++)
  {
    j_value_destroy(&value[i]);
  }
  return (j_object*) pResult;
}


j_object* vm_exe_join(vm_desc* pVM, j_object* args[])
{
  str_obj strResult;
  j_object* pObject = args[0];
  j_object* pObjectJoin = args[1];
  j_object* pItem;

  j_object_string* pResult = NULL;

  UNUSED(pVM);

  char* pTemp;
  char* separator;
  str_obj_create(&strResult, 0);

  uint32_t index;
  uint32_t count;

  separator = j_object_toString(pObjectJoin);
  count = j_object_getCount(pObject);
  trace("count = %d\n", count);
  for (index = 0; index < count; index++)
  {
    pItem = j_object_getAtIndex(pObject, index);
    pTemp = j_object_toString(pItem);
    str_obj_insert(&strResult, pTemp);
    free(pTemp);
    j_object_delete(pItem);

    if (index != (count - 1))
    {
      str_obj_insert(&strResult, separator);
    }
  }

  free(separator);

  pResult = j_object_string_new(strResult.s);
  return (j_object*) pResult;
}

j_object* vm_exe_range(vm_desc* pVM, uint32_t nbArgs, j_object* args[])
{
  int32_t value[3];
  j_object_range* pResult = NULL;

  UNUSED(pVM);
  //first argument if impacted object it is null in case of range
  //it can be changed to set iterator with it ? (directly into the compiler)
  if (nbArgs == 4)
  {
    value[0] = j_object_toInteger(args[1]);
    value[1] = j_object_toInteger(args[2]);
    value[2] = j_object_toInteger(args[3]);
    pResult = j_object_range_new(value[0],  value[1], value[2]);
  }

  return (j_object*) pResult;
}


/**************************************************************/

char* upper(char* s)
{
  char* current;
  current = s;

  while (*current != '\0')
  {
    *current = toupper(*current);
    current++;
  }
  return s;
}

char* lower(char* s)
{
  char* current;
  current = s;

  while (*current != '\0')
  {
    *current = tolower(*current);
    current++;
  }
  return s;
}

char* capitalize(char* s)
{
  char* current;
  current = s;

  while ((*current != '\0') && (isspace(*current)))
  {
    current++;
  }

  if (*current != '\0')
  {
    *current = toupper(*current);
    current++;
  }

  while (*current != '\0')
  {
    *current = tolower(*current);
    current++;
  }

  return s;
}

char* title(char* s)
{
  char* current;
  current = s;
  BOOL bFirstChar;

  bFirstChar = TRUE;

  while (*current != '\0')
  {
    if (isspace(*current))
    {
      bFirstChar = TRUE;
    }
    else if (bFirstChar)
    {
      bFirstChar = FALSE;
      *current = toupper(*current);
    }
    else
    {
      *current = tolower(*current);
    }

    current++;
  }

  return s;
}

char* trim(char* s)
{
  char* begin;
  char* end;
  char* n;

  n = s; //by default return the first string
  begin = s;
  while ((*begin != '\0') && (isspace(*begin)))
  {
    begin++;
  }

  if (*begin != '\0')
  {
    end = begin + strlen(begin) - 1;
    while ((end > begin) && (isspace(*end)))
    {
      end--;
    }

    *(end + 1) = '\0';

    n = strdup(begin);
    free(s);
  }

  return n;
}

char* truncate(char* origin, uint32_t truncSize, BOOL killwords, char* endSentence, uint32_t tolerateMargin)
{
  uint32_t lenString;
  uint32_t lenEndSentence;
  char* r;

  r = NULL;
  lenString = strlen(origin);
  lenEndSentence = strlen(endSentence);

  if (lenString > (truncSize + tolerateMargin))
  {
    if (killwords == TRUE)
    {
      r = calloc(truncSize + 1, 1);
      ASSERT(r != NULL);
      strncpy(r, origin, truncSize - lenEndSentence);
      strncat(r, endSentence, lenEndSentence);
    }
    else
    {
      r = calloc(truncSize + 1, 1);
      ASSERT(r != NULL);
      strncpy(r, origin, truncSize - lenEndSentence);
      char* sp = rindex(r, ' ');
      if (sp != NULL)
      {
        *sp = '\0';
      }

      strncat(r, endSentence, lenEndSentence);
    }
  }
  else
  {
    r = strdup(origin);
  }

  return r;
}

char* center(char* origin, uint32_t width)
{
  uint32_t lenString;
  uint32_t offset;
  char* r;
  lenString = strlen(origin);
  r = NULL;

  if (width > lenString)
  {
    offset = (width - lenString) / 2;
    r = malloc(width + 1);
    ASSERT(r != NULL);

    memset(r, ' ', width);
    r[width] = '\0';

    memcpy(r + offset, origin, lenString);
  }
  else
  {
    r = strdup(origin);
  }

  return r;
}


BOOL findModifier(char* src, j_value_type* type, char** pModifierEnd, BOOL* hasNoArg)
{
  ASSERT(type != NULL);
  ASSERT(src != NULL);
  ASSERT(hasNoArg != NULL);
  ASSERT(pModifierEnd != NULL);
  *hasNoArg = FALSE;

  //find end modifier
  BOOL bEndFound = FALSE;
  while ((*src != '\0') && (bEndFound == FALSE))
  {
    switch (*src)
    {
      //int
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
      case 'X':
      case 'c':
      case 'p':
        *pModifierEnd = src;
        bEndFound = TRUE;
        *type = J_INT;
        break;

      //double
      case 'e':
      case 'E':
      case 'f':
      case 'F':
      case 'g':
      case 'G':
      case 'a':
      case 'A':
        *pModifierEnd = src;
        bEndFound = TRUE;
        *type = J_DOUBLE;
        break;

      //string
      case 's':
        *pModifierEnd = src;
        bEndFound = TRUE;
        *type = J_STRING;
        break;

      //no arg
      case 'm':
        *pModifierEnd = src;
        bEndFound = TRUE;
        *hasNoArg = TRUE;
        break;

      case 'n':
        ASSERT(FALSE);
        break;

      default:
        src++;
        break;
    }
  }
  return bEndFound;
}


char* getModifierString(char* pModifierBegin, char* pModifierEnd)
{
  int32_t size = pModifierEnd + 1 - pModifierBegin;
  //trace("modifier (%d) ==> %.*s\n", size, size, pModifierBegin);

  char* modifierString = malloc(size + 1);
  strncpy(modifierString, pModifierBegin, size);
  modifierString[size] = '\0';

  //fprintf(stdout, "modifier (%s)\n", modifierString);

  return modifierString;
}

#define error(a, ...)
#define ERROR_LEVEL 0
#define WARNING_LEVEL 1

BOOL setFormatConsistency(j_value_type* typeToInsert, j_value currentParameter, char* pModifierString,
                          j_value_base* paramDataToInsert)
{
  BOOL bOk;
  bOk = TRUE;

  ASSERT(paramDataToInsert != NULL);
  ASSERT(pModifierString != NULL);
  ASSERT(typeToInsert != NULL);

  if ((*typeToInsert == J_STRING) && (currentParameter.type == J_INT))
  {
    *typeToInsert = J_INT;
    pModifierString[strlen(pModifierString) - 1] = 'd';
    *paramDataToInsert = currentParameter.value;
  }
  else if ((*typeToInsert == J_STRING) && (currentParameter.type == J_DOUBLE))
  {
    *typeToInsert = J_DOUBLE;
    pModifierString[strlen(pModifierString) - 1] = 'f';
    *paramDataToInsert = currentParameter.value;
  }
  else if ((*typeToInsert == J_DOUBLE) && (currentParameter.type == J_INT))
  {
    (*paramDataToInsert).type_double = (double) currentParameter.value.type_int;
  }
  else if ((*typeToInsert == J_DOUBLE) && (currentParameter.type == J_STRING))
  {
    error(ERROR_LEVEL, "a float is required instead of a string\n");
    bOk = FALSE;
  }
  else if ((*typeToInsert == J_INT) && (currentParameter.type == J_DOUBLE))
  {
    (*paramDataToInsert).type_int = (int32_t) currentParameter.value.type_double;
  }
  else if ((*typeToInsert == J_INT) && (currentParameter.type == J_STRING))
  {
    error(ERROR_LEVEL, "a integer is required instead of a string\n");
    bOk = FALSE;
  }
  return bOk;
}

BOOL appendParameterToString(char* pModifierString, j_value_type typeToInsert,
                             str_obj* strDestination, int32_t currentParameterIndex,
                             int32_t nbParameters, j_value* param, BOOL hasNoArg
                            )
{
  ASSERT(pModifierString != NULL);
  ASSERT(strDestination != NULL);
  ASSERT(param != NULL);

  int32_t addedSize = 0;
  char* extraStringToAdd;
  BOOL bOk;
  extraStringToAdd = NULL;
  bOk = TRUE;
  j_value_base paramDataToInsert;

  if (currentParameterIndex < nbParameters)
  {
    if (param[currentParameterIndex].type != typeToInsert)
    {
      error(WARNING_LEVEL, "parameter type inconsistency %d versus %d\n", param[currentParameterIndex].type,
            typeToInsert);
      bOk = setFormatConsistency(&typeToInsert, param[currentParameterIndex], pModifierString, &paramDataToInsert);
    }
    else
    {
      paramDataToInsert = param[currentParameterIndex].value;
    }

    if (bOk)
    {
      if (hasNoArg)
      {
        addedSize = snprintf(NULL, 0, pModifierString);
        addedSize++;
        extraStringToAdd = malloc(addedSize);
        snprintf(extraStringToAdd, addedSize, pModifierString);
      }
      else
      {
        switch (typeToInsert)
        {
          case J_INT:
            addedSize = snprintf(NULL, 0, pModifierString, paramDataToInsert.type_int);
            addedSize++;
            extraStringToAdd = malloc(addedSize);
            snprintf(extraStringToAdd, addedSize, pModifierString, paramDataToInsert.type_int);
            break;

          case J_DOUBLE:
            addedSize = snprintf(NULL, 0, pModifierString, paramDataToInsert.type_double);
            addedSize++;
            extraStringToAdd = malloc(addedSize);
            snprintf(extraStringToAdd, addedSize, pModifierString, paramDataToInsert.type_double);
            break;

          case J_STRING:
            addedSize = snprintf(NULL, 0, pModifierString, paramDataToInsert.type_string);
            addedSize++;
            extraStringToAdd = malloc(addedSize);
            snprintf(extraStringToAdd, addedSize, pModifierString, paramDataToInsert.type_string);
            break;

          default:
            ASSERT(FALSE);
            break;
        }
      }

      str_obj_insert(strDestination, extraStringToAdd);
      free(extraStringToAdd);
    }
  }
  else
  {
    error(ERROR_LEVEL, "inconsistency number of parameter for format function (%d versus max %d)\n",
          currentParameterIndex, nbParameters);
    bOk = FALSE;
  }

  return bOk;
}

#if 0
STATIC char* join_withList(JList* pList, char* separator);
STATIC char* join_withArray(int64_t privKey, jinjac_parameter_type type, int32_t nbItems, char* separator);
STATIC char* join_withString(char* s, char* separator);

char* join(JObject* pObject, char* separator)
{
  char* r;
  r = NULL;

  switch (pObject->type)
  {
    case J_LIST:
      r = join_withList((JList*) pObject, separator);
      break;

    case J_IDENTIFIER:
      {
        jinjac_parameter param;
        BOOL bOk;
        BOOL isArray;
        int64_t privKey;
        JIdentifier* pIdent = (JIdentifier*) pObject;
        bOk = parameter_search(pIdent->identifier, &privKey, &isArray);
        if (bOk)
        {
          if (isArray)
          {
            //iterable on array
            int32_t nbItems;
            parameter_array_getProperties(privKey, &param.type, &nbItems);
            r = join_withArray(privKey, param.type, nbItems, separator);
          }
          else
          {
            //depend of object type
            parameter_get(privKey, &param);
            switch (param.type)
            {
              case TYPE_DOUBLE:
              case TYPE_INT:
                error(WARNING_LEVEL, "object is not iterable\n");
                r = NULL;
                break;

              case TYPE_STRING:
                r = join_withString(param.value.type_string, separator);
                break;

              default:
                ASSERT(FALSE);
                break;
            }
          }
        }
        else
        {
          error(WARNING_LEVEL, "unknown identifier\n");
          r = NULL;
        }
      }
      break;

    case J_ARRAY:
    case J_STR_CONSTANTE:
      {
        jinjac_parameter param;
        JObject_getValue(pObject, &param);
        switch (param.type)
        {
          case TYPE_STRING:
            r = join_withString(param.value.type_string, separator);
            free(param.value.type_string);
            break;

          default:
            error(ERROR_LEVEL, "object is not iterable\n");
            break;
        }
      }
      break;

    default:
      error(ERROR_LEVEL, "object is not iterable\n");
      break;
  }

  return r;
}

char* join_withList(JList* pList, char* separator)
{
  str_obj strResult;
  char* pTemp;
  str_obj_create(&strResult, 0);

  JListItem* item;
  item = pList->list;

  while (item != NULL)
  {
    pTemp = JObject_toString(item->object);
    str_obj_insert(&strResult, pTemp);
    free(pTemp);
    if (item->next != NULL)
    {
      str_obj_insert(&strResult, separator);
    }
    item = item->next;
  }

  return strResult.s;
}

char* join_withArray(int64_t privKey, jinjac_parameter_type type, int32_t nbItems, char* separator)
{
  str_obj strResult;
  J_STATUS status;
  jinjac_parameter_value temp;
  str_obj_create(&strResult, 0);
  char* strTemp;

  for (int32_t i = 0; i < nbItems; i++)
  {
    status = parameter_array_getValue(privKey, i, &temp);
    ASSERT(status == J_OK);
    switch (type)
    {
      case TYPE_DOUBLE:
        strTemp = doubleToStr(temp.type_double);
        str_obj_insert(&strResult, strTemp);
        free(strTemp);
        break;

      case TYPE_STRING:
        str_obj_insert(&strResult, temp.type_string);
        break;

      case TYPE_INT:
        strTemp = intToStr(temp.type_int);
        str_obj_insert(&strResult, strTemp);
        free(strTemp);
        break;

      default:
        ASSERT(FALSE);
        break;
    }
    if (i != (nbItems - 1))
    {
      str_obj_insert(&strResult, separator);
    }
  }

  return strResult.s;
}


char* join_withString(char* s, char* separator)
{
  str_obj strResult;
  int32_t len;

  len = strlen(s);
  str_obj_create(&strResult, 0);
  for (int32_t i = 0; i < len; i++)
  {
    str_obj_insertChar(&strResult, s[i]);
    if (i != (len - 1))
    {
      str_obj_insert(&strResult, separator);
    }
  }

  return strResult.s;
}
#endif

char* format(char* origin, int32_t nbParameters, j_value* param)
{
  int32_t currentParameterIndex = 0;
  char* src;
  char* pModifierBegin = NULL;
  char* pModifierEnd = NULL;
  str_obj dst;

  str_obj_create(&dst, 10);
  src = origin;
  BOOL isModifier;
  isModifier = FALSE;

  BOOL isInError;
  isInError = FALSE;

  while ((*src != '\0') && (isInError == FALSE))
  {
    if (isModifier == FALSE)
    {
      if (*src != '%')
      {
        str_obj_insertChar(&dst, *src);
      }
      else
      {
        isModifier = TRUE;
        pModifierBegin = src;
      }
      src++;
    }
    else
    {
      if (*src == '%')
      {
        str_obj_insertChar(&dst, *src);
        src++;
      }
      else
      {
        BOOL bEndFound;
        BOOL hasNoArg;
        j_value_type typeToInsert;
        //find end modifier
        bEndFound = findModifier(src, &typeToInsert, &pModifierEnd, &hasNoArg);
        if (bEndFound)
        {
          char* modifierString;
          modifierString = getModifierString(pModifierBegin, pModifierEnd);

          appendParameterToString(modifierString, typeToInsert, &dst, currentParameterIndex,
                                  nbParameters, param, hasNoArg);
          free(modifierString);
        }
        else
        {
          //error
          isInError = TRUE;
        }

        src = pModifierEnd + 1;
        isModifier = FALSE;
        currentParameterIndex++;
      }
    }
  }

  if (isInError)
  {
    str_obj_free(&dst);
    return origin;
  }

  free(origin);
  return dst.s;
}

