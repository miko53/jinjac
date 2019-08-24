#include "buildin.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

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

typedef struct
{
  char* s;
  int size;
  int allocatedSize;
} str_obj;

void str_obj_create(str_obj* obj)
{
  obj->s = malloc(10);
  ASSERT(obj->s != NULL);
  obj->s[0] = '\0';
  obj->allocatedSize = 10;
  obj->size = 0;
}

void str_obj_free(str_obj* obj)
{
  if (obj->s)
  {
    free(obj->s);
  }

  obj->s = NULL;
  obj->size = 0;
  obj->allocatedSize = 0;
}

void str_obj_realloc(str_obj* obj, int newSize)
{
  ASSERT(obj != NULL);
  char* n = realloc(obj->s, newSize);
  ASSERT(n != NULL);
  obj->s = n;
  obj->allocatedSize = newSize;
}


void str_obj_insertChar(str_obj* obj, char src)
{
  ASSERT(obj != NULL);
  ASSERT(obj->s != NULL);

  if ((obj->size + 1) >= obj->allocatedSize)
  {
    str_obj_realloc(obj, obj->allocatedSize * 2);
  }

  int size = obj->size;
  obj->s[size] = src;
  size++;
  obj->s[size] = '\0';
  obj->size = size;
}

void str_obj_insert(str_obj* obj, char* src)
{
  int s = strlen(src);
  if (obj->size + s + 1 >= obj->allocatedSize)
  {
    str_obj_realloc(obj, obj->allocatedSize * 2);
  }

  strncat(obj->s, src, s);
  obj->size += s;
  obj->s[obj->size] = '\0';
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
        //find end modifier
        BOOL bEndFound = FALSE;
        parameter_type type;
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
              pModifierEnd = src;
              bEndFound = TRUE;
              type = TYPE_INT;
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
              pModifierEnd = src;
              bEndFound = TRUE;
              type = TYPE_DOUBLE;
              break;

            //string
            case 's':
              pModifierEnd = src;
              bEndFound = TRUE;
              type = TYPE_STRING;
              break;

            //no arg
            case 'm':
              pModifierEnd = src;
              bEndFound = TRUE;
              type = TYPE_UNKOWN;
              break;

            case 'n':
              ASSERT(FALSE);
              break;

            default:
              src++;
              break;
          }
        }

        if (bEndFound)
        {
          int size = pModifierEnd + 1 - pModifierBegin;
          fprintf(stdout, "modifier (%d) ==> %.*s\n", size, size, pModifierBegin);
          char* modifierString = malloc(size + 1);
          strncpy(modifierString, pModifierBegin, size);
          modifierString[size] = '\0';
          fprintf(stdout, "modifier (%s)\n", modifierString);

          int addedSize = 0;
          char* extraStringToAdd;
          switch (type)
          {
            case TYPE_INT:
              addedSize = snprintf(NULL, addedSize, modifierString, param[currentParameterIndex].type_int);
              addedSize++;
              extraStringToAdd = malloc(addedSize);
              snprintf(extraStringToAdd, addedSize, modifierString, param[currentParameterIndex].type_int);
              break;

            case TYPE_DOUBLE:
              addedSize = snprintf(NULL, addedSize, modifierString, param[currentParameterIndex].type_double);
              addedSize++;
              extraStringToAdd = malloc(addedSize);
              snprintf(extraStringToAdd, addedSize, modifierString, param[currentParameterIndex].type_double);
              break;

            case TYPE_STRING:
              addedSize = snprintf(NULL, addedSize, modifierString, param[currentParameterIndex].type_string);
              addedSize++;
              extraStringToAdd = malloc(addedSize);
              snprintf(extraStringToAdd, addedSize, modifierString, param[currentParameterIndex].type_string);
              break;

            case TYPE_UNKOWN:
              addedSize = snprintf(NULL, addedSize, modifierString);
              addedSize++;
              extraStringToAdd = malloc(addedSize);
              snprintf(extraStringToAdd, addedSize, modifierString);
              break;

            default:
              ASSERT(FALSE);
              break;
          }

          str_obj_insert(&dst, extraStringToAdd);
          free(extraStringToAdd);
          free(modifierString);
          extraStringToAdd = NULL;
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

  //   size = snprintf(r, size, origin);
  //
  //   fprintf(stdout, "size = %d\n", size);
  //sprintf(r, origin, a1.type_double, a2.type_double);
  /*va_list args;

  va_start (args, origin);
  nbChar = vsnprintf(NULL, 0, origin, args);
  va_end(args);

  r = malloc(nbChar + 1);
  r[nbChar] = '\0';

  va_start (args, origin);
  nbChar = vsnprintf(r, nbChar, origin, args);
  va_end (args);
  */
  free(origin);
  return dst.s;

error:
  str_obj_free(&dst);
  return origin;
}
