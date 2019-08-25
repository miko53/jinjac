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


BOOL findModifier(char* src, parameter_type* type, char** pModifierEnd)
{
  ASSERT(type != NULL);
  ASSERT(src != NULL);
  ASSERT(pModifierEnd != NULL);

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
        *type = TYPE_UNKOWN;
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
  fprintf(stdout, "modifier (%d) ==> %.*s\n", size, size, pModifierBegin);

  char* modifierString = malloc(size + 1);
  strncpy(modifierString, pModifierBegin, size);
  modifierString[size] = '\0';

  fprintf(stdout, "modifier (%s)\n", modifierString);

  return modifierString;
}

BOOL appendParameterToString(char* pModifierString, parameter_type typeToInsert,
                             str_obj* strDestination, int currentParameterIndex,
                             int nbParameter, parameter_value* param, parameter_type* type
                            )
{
  ASSERT(pModifierString != NULL);
  ASSERT(strDestination != NULL);
  ASSERT(param != NULL);
  ASSERT(type != NULL);

  int addedSize = 0;
  char* extraStringToAdd;
  BOOL bOk;
  bOk = TRUE;

  switch (typeToInsert)
  {
    case TYPE_INT:
      addedSize = snprintf(NULL, addedSize, pModifierString, param[currentParameterIndex].type_int);
      addedSize++;
      extraStringToAdd = malloc(addedSize);
      snprintf(extraStringToAdd, addedSize, pModifierString, param[currentParameterIndex].type_int);
      break;

    case TYPE_DOUBLE:
      addedSize = snprintf(NULL, addedSize, pModifierString, param[currentParameterIndex].type_double);
      addedSize++;
      extraStringToAdd = malloc(addedSize);
      snprintf(extraStringToAdd, addedSize, pModifierString, param[currentParameterIndex].type_double);
      break;

    case TYPE_STRING:
      addedSize = snprintf(NULL, addedSize, pModifierString, param[currentParameterIndex].type_string);
      addedSize++;
      extraStringToAdd = malloc(addedSize);
      snprintf(extraStringToAdd, addedSize, pModifierString, param[currentParameterIndex].type_string);
      break;

    case TYPE_UNKOWN:
      addedSize = snprintf(NULL, addedSize, pModifierString);
      addedSize++;
      extraStringToAdd = malloc(addedSize);
      snprintf(extraStringToAdd, addedSize, pModifierString);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  str_obj_insert(strDestination, extraStringToAdd);
  free(extraStringToAdd);
  extraStringToAdd = NULL;

  return bOk;
}

char* format(char* origin, int nbParameter, parameter_value* param, parameter_type* type)
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

  while (*src != '\0')
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
        parameter_type typeToInsert;
        //find end modifier
        bEndFound = findModifier(src, &typeToInsert, &pModifierEnd);
        if (bEndFound)
        {
          char* modifierString;
          modifierString = getModifierString(pModifierBegin, pModifierEnd);

          appendParameterToString(modifierString, typeToInsert, &dst, currentParameterIndex,
                                  nbParameter, param, type);
          free(modifierString);
        }
        else
        {
          //error
          goto error;
        }

        src = pModifierEnd + 1;
        isModifier = FALSE;
        currentParameterIndex++;
      }
    }
  }

  free(origin);
  return dst.s;

error:
  str_obj_free(&dst);
  return origin;
}
