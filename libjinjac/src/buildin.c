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

#include "buildin.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "str_obj.h"
#include "convert.h"

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

  r = origin;
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
      free(origin);
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
      free(origin);
    }
  }

  return r;
}

char* center(char* origin, uint32_t width)
{
  uint32_t lenString;
  uint32_t offset;
  char* r;
  lenString = strlen(origin);
  r = origin;

  if (width > lenString)
  {
    offset = (width - lenString) / 2;
    r = malloc(width + 1);
    ASSERT(r != NULL);

    memset(r, ' ', width);
    r[width] = '\0';

    memcpy(r + offset, origin, lenString);
    free(origin);
  }

  return r;
}


BOOL findModifier(char* src, jinjac_parameter_type* type, char** pModifierEnd, BOOL* hasNoArg)
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
        *type = TYPE_INT;
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
        *type = TYPE_DOUBLE;
        break;

      //string
      case 's':
        *pModifierEnd = src;
        bEndFound = TRUE;
        *type = TYPE_STRING;
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
  trace("modifier (%d) ==> %.*s\n", size, size, pModifierBegin);

  char* modifierString = malloc(size + 1);
  strncpy(modifierString, pModifierBegin, size);
  modifierString[size] = '\0';

  fprintf(stdout, "modifier (%s)\n", modifierString);

  return modifierString;
}


BOOL setFormatConsistency(jinjac_parameter_type* typeToInsert, jinjac_parameter currentParameter, char* pModifierString,
                          jinjac_parameter_value* paramDataToInsert)
{
  BOOL bOk;
  bOk = TRUE;

  ASSERT(paramDataToInsert != NULL);
  ASSERT(pModifierString != NULL);
  ASSERT(typeToInsert != NULL);

  if ((*typeToInsert == TYPE_STRING) && (currentParameter.type == TYPE_INT))
  {
    *typeToInsert = TYPE_INT;
    pModifierString[strlen(pModifierString) - 1] = 'd';
    *paramDataToInsert = currentParameter.value;
  }
  else if ((*typeToInsert == TYPE_STRING) && (currentParameter.type == TYPE_DOUBLE))
  {
    *typeToInsert = TYPE_DOUBLE;
    pModifierString[strlen(pModifierString) - 1] = 'f';
    *paramDataToInsert = currentParameter.value;
  }
  else if ((*typeToInsert == TYPE_DOUBLE) && (currentParameter.type == TYPE_INT))
  {
    (*paramDataToInsert).type_double = (double) currentParameter.value.type_int;
  }
  else if ((*typeToInsert == TYPE_DOUBLE) && (currentParameter.type == TYPE_STRING))
  {
    error(ERROR_LEVEL, "a float is required instead of a string\n");
    bOk = FALSE;
  }
  else if ((*typeToInsert == TYPE_INT) && (currentParameter.type == TYPE_DOUBLE))
  {
    (*paramDataToInsert).type_int = (int32_t) currentParameter.value.type_double;
  }
  else if ((*typeToInsert == TYPE_INT) && (currentParameter.type == TYPE_STRING))
  {
    error(ERROR_LEVEL, "a integer is required instead of a string\n");
    bOk = FALSE;
  }
  return bOk;
}

BOOL appendParameterToString(char* pModifierString, jinjac_parameter_type typeToInsert,
                             str_obj* strDestination, int32_t currentParameterIndex,
                             int32_t nbParameters, jinjac_parameter* param, BOOL hasNoArg
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
  jinjac_parameter_value paramDataToInsert;

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
          case TYPE_INT:
            addedSize = snprintf(NULL, 0, pModifierString, paramDataToInsert.type_int);
            addedSize++;
            extraStringToAdd = malloc(addedSize);
            snprintf(extraStringToAdd, addedSize, pModifierString, paramDataToInsert.type_int);
            break;

          case TYPE_DOUBLE:
            addedSize = snprintf(NULL, 0, pModifierString, paramDataToInsert.type_double);
            addedSize++;
            extraStringToAdd = malloc(addedSize);
            snprintf(extraStringToAdd, addedSize, pModifierString, paramDataToInsert.type_double);
            break;

          case TYPE_STRING:
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


char* format(char* origin, int32_t nbParameters, jinjac_parameter* param)
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
        jinjac_parameter_type typeToInsert;
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
