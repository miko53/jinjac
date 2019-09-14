#include "buildin.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "str_obj.h"

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

char* truncate(char* origin, unsigned int truncSize, BOOL killwords, char* endSentence, unsigned int tolerateMargin)
{
  unsigned int lenString;
  unsigned int lenEndSentence;
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

char* center(char* origin, unsigned int width)
{
  unsigned int lenString;
  unsigned int offset;
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


BOOL findModifier(char* src, parameter_type* type, char** pModifierEnd, BOOL* hasNoArg)
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
  int size = pModifierEnd + 1 - pModifierBegin;
  trace("modifier (%d) ==> %.*s\n", size, size, pModifierBegin);

  char* modifierString = malloc(size + 1);
  strncpy(modifierString, pModifierBegin, size);
  modifierString[size] = '\0';

  fprintf(stdout, "modifier (%s)\n", modifierString);

  return modifierString;
}


BOOL setFormatConsistency(parameter_type* typeToInsert, parameter currentParameter, char* pModifierString,
                          parameter_value* paramDataToInsert)
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
    error("error in input, a float is required instead of a string\n");
    bOk = FALSE;
  }
  else if ((*typeToInsert == TYPE_INT) && (currentParameter.type == TYPE_DOUBLE))
  {
    (*paramDataToInsert).type_int = (int) currentParameter.value.type_double;
  }
  else if ((*typeToInsert == TYPE_INT) && (currentParameter.type == TYPE_STRING))
  {
    error("error in input, a int is required instead of a string\n");
    bOk = FALSE;
  }
  return bOk;
}

BOOL appendParameterToString(char* pModifierString, parameter_type typeToInsert,
                             str_obj* strDestination, int currentParameterIndex,
                             int nbParameters, parameter* param, BOOL hasNoArg
                            )
{
  ASSERT(pModifierString != NULL);
  ASSERT(strDestination != NULL);
  ASSERT(param != NULL);

  int addedSize = 0;
  char* extraStringToAdd;
  BOOL bOk;
  extraStringToAdd = NULL;
  bOk = TRUE;
  parameter_value paramDataToInsert;

  if (currentParameterIndex < nbParameters)
  {
    if (param[currentParameterIndex].type != typeToInsert)
    {
      error("warning: parameter type inconsistency %d versus %d\n", param[currentParameterIndex].type,
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
        addedSize = snprintf(NULL, addedSize, pModifierString);
        addedSize++;
        extraStringToAdd = malloc(addedSize);
        snprintf(extraStringToAdd, addedSize, pModifierString);
      }
      else
      {
        switch (typeToInsert)
        {
          case TYPE_INT:
            addedSize = snprintf(NULL, addedSize, pModifierString, paramDataToInsert.type_int);
            addedSize++;
            extraStringToAdd = malloc(addedSize);
            snprintf(extraStringToAdd, addedSize, pModifierString, paramDataToInsert.type_int);
            break;

          case TYPE_DOUBLE:
            addedSize = snprintf(NULL, addedSize, pModifierString, paramDataToInsert.type_double);
            addedSize++;
            extraStringToAdd = malloc(addedSize);
            snprintf(extraStringToAdd, addedSize, pModifierString, paramDataToInsert.type_double);
            break;

          case TYPE_STRING:
            addedSize = snprintf(NULL, addedSize, pModifierString, paramDataToInsert.type_string);
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
    error("error: inconsistency number of parameter for format function (%d versus max %d)\n",
          currentParameterIndex, nbParameters);
    bOk = FALSE;
  }

  return bOk;
}

char* format(char* origin, int nbParameters, parameter* param)
{
  int currentParameterIndex = 0;
  char* src;
  char* pModifierBegin;
  char* pModifierEnd;
  str_obj dst;

  str_obj_create(&dst);
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
        parameter_type typeToInsert;
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
