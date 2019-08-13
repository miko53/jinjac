#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"
#include "common.h"
#include "convert.h"

#define MAX_OBJECT (50)
#define NEW(obj)  malloc(sizeof(obj))

static ast ast_root;

static JObject* ast_list[MAX_OBJECT];
static unsigned int ast_nb_object;

void JObject_delete(JObject* pObject);

void ast_clean()
{
  ast_root.inError = FALSE;
  if (ast_root.currentStringValue != NULL)
  {
    free(ast_root.currentStringValue );
    ast_root.currentStringValue = NULL;
  }

  for (unsigned int i = 0; i < ast_nb_object; i++)
  {
    JObject_delete(ast_list[i]);
    ast_list[i] = NULL;
  }
  ast_nb_object = 0;
}

ast* getAstRoot(void)
{
  return &ast_root;
}

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

typedef struct
{
  filter_fct fct;
  const char* name;
} fct_converter;

fct_converter tab_fct_converter[] =
{
  { .fct = upper, .name = "upper" }
};


filter_fct getFunction(char* fctName)
{
  int sizeMax = sizeof(tab_fct_converter) / sizeof(fct_converter);
  int i;

  for (i = 0; i < sizeMax; i++)
  {
    if (strcmp(tab_fct_converter[i].name, fctName) == 0)
    {
      return tab_fct_converter[i].fct;
    }
  }


  return NULL;
}

int ast_insert(JObject* o)
{
  int rc;
  rc = -1;

  if (ast_nb_object < MAX_OBJECT)
  {
    ast_list[ast_nb_object] = o;
    ast_nb_object++;
    rc = 0;
  }

  return rc;
}

JObject* JStringConstante_new(char* name)
{
  JStringConstante* o = NEW(JStringConstante);
  o->base.type = J_STR_CONSTANTE;
  o->str_constant = name;
  return (JObject*) o;
}

JObject* JIdentifier_new(char* name)
{
  JIdentifier* o = NEW(JIdentifier);
  o->base.type = J_IDENTIFIER;
  o->identifier = name;
  return (JObject*) o;
}


JObject* JInteger_new(int i)
{
  JInteger* o = NEW(JInteger);
  o->base.type = J_INTEGER;
  o->value = i;
  return (JObject*) o;
}

JObject* JDouble_new(double d)
{
  JDouble* o = NEW(JDouble);
  o->base.type = J_DOUBLE;
  o->value = d;
  return (JObject*) o;
}

JObject* JFunction_new(char* fct)
{
  filter_fct function = getFunction(fct);
  if (function != NULL)
  {
    JFunction* o = NEW(JFunction);
    o->base.type = J_FUNCTION;
    o->argList = NULL;
    o->function = getFunction(fct);
    return (JObject*) o;
  }
  else
  {
    fprintf(stdout, "filtered fct not found !\n");
  }

  return NULL;
}

JObject* JArray_new(char* name, int offset)
{
  JArray* o = NEW(JArray);
  o->base.type = J_ARRAY;
  o->identifier = name;
  o->offset = offset;
  return (JObject*) o;
}

int ast_insert_constante(char* name)
{
  JObject* o = JStringConstante_new(name);
  return ast_insert(o);
}

int ast_insert_identifier(char* name)
{
  JObject* o = JIdentifier_new(name);
  return ast_insert(o);
}

int ast_insert_integer(int i)
{
  JObject* o = JInteger_new(i);
  return ast_insert(o);
}

int ast_insert_double(double d)
{
  JObject* o = JDouble_new(d);
  return ast_insert(o);
}

int ast_insert_function(char* fct)
{
  JObject* o = JFunction_new(fct);
  if (o != NULL)
  {
    return ast_insert(o);
  }

  return -1;
}

int ast_insert_array(char* name, int offset)
{
  JObject* o = JArray_new(name, offset);
  if (o != NULL)
  {
    return ast_insert(o);
  }

  return -1;
}

char* JObject_toString(JObject* pObject)
{
  ASSERT(pObject != NULL);
  char* s;
  s = NULL;

  switch (pObject->type)
  {
    case J_STR_CONSTANTE:
      s = strdup(((JStringConstante*) pObject)->str_constant);
      break;

    case J_INTEGER:
      s = intToStr(((JInteger*) pObject)->value);
      break;

    case J_DOUBLE:
      s = doubleToStr(((JDouble*) pObject)->value);
      break;

    case J_IDENTIFIER:
      {
        JIdentifier* pIdent;
        pIdent = (JIdentifier*) pObject;
        parameter_type type = param_getType(pIdent->identifier);
        switch (type)
        {
          case TYPE_STRING:
            s = (char*) param_getValue(pIdent->identifier).type_string;
            break;

          case TYPE_INT:
            s = intToStr(param_getValue(pIdent->identifier).type_int);
            if (s == NULL)
            {
              fprintf(stdout, "ID error\n");
              getAstRoot()->inError = TRUE;
            }
            break;

          case TYPE_DOUBLE:
            s = doubleToStr(param_getValue(pIdent->identifier).type_double);
            if (s == NULL)
            {
              fprintf(stdout, "ID error\n");
              getAstRoot()->inError = TRUE;
            }
            break;

          default:
            getAstRoot()->inError = TRUE;
            fprintf(stdout, "unknown '%s' identifier\n", pIdent->identifier);
            ASSERT(FALSE);
            break;
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
              s = strdup((char*) v.type_string);
              break;

            case TYPE_DOUBLE:
              s = doubleToStr(v.type_double);
              if (s == NULL)
              {
                fprintf(stdout, "array conversion error\n");
                getAstRoot()->inError = TRUE;
              }
              break;

            case TYPE_INT:
              s = intToStr(v.type_int);
              if (s == NULL)
              {
                fprintf(stdout, "array conversion error\n");
                getAstRoot()->inError = TRUE;
              }
              break;

            default:
              getAstRoot()->inError = TRUE;
              fprintf(stdout, "unknown '%s' array\n", pArray->identifier);
              ASSERT(FALSE);
              break;
          }
        }

      }
      break;

    default:
      fprintf(stdout, "not yet implemented...(type = %d)\n", pObject->type);
      break;
  }

  return s;
}

void JObject_delete(JObject* pObject)
{
  switch (pObject->type)
  {
    case J_STR_CONSTANTE:
      free (((JStringConstante*) pObject)->str_constant);
      break;

    case J_IDENTIFIER:
      free (((JIdentifier*) pObject)->identifier);
      break;

    case J_INTEGER:
    case J_DOUBLE:
      break;

    case J_FUNCTION:
      {
        JFunction* fct = ((JFunction*) pObject);
        if (fct->argList != NULL)
        {
          JObject_delete((JObject*) fct->argList);
        }
      }
      break;

    case J_ARRAY:
      free (((JArray*) pObject)->identifier);
      break;

    default:
      fprintf(stdout, "type = %d\n", pObject->type);
      ASSERT(FALSE);
      break;
  }

  free(pObject);
}


void ast_remove_last()
{
  JObject_delete(ast_list[ast_nb_object - 1]);
  ast_list[ast_nb_object - 1] = NULL;
  ast_nb_object--;
}


char* ast_convert_to_string()
{
  char* s;
  s = NULL;
  if (ast_nb_object != 0)
  {
    s = JObject_toString(ast_list[ast_nb_object - 1]);
    ast_remove_last();
  }

  return s;
}


char* ast_apply_filtering()
{
  char* s;
  s = NULL;
  if (ast_nb_object != 0)
  {
    ASSERT(ast_list[ast_nb_object - 1]->type == J_FUNCTION);

    JFunction* f = (JFunction*) ast_list[ast_nb_object - 1];
    s = f->function(getAstRoot()->currentStringValue); //TODO add argument
    ast_remove_last();
  }

  return s;
}

BOOL ast_get_offset(JObject* pObject, int* pOffset)
{
  ASSERT(pObject != NULL);
  ASSERT(pOffset != NULL);
  BOOL  b;
  b = FALSE;

  switch (pObject->type)
  {
    case J_IDENTIFIER:
      {
        JIdentifier* pIdent;
        pIdent = (JIdentifier*) pObject;
        parameter_type type = param_getType(pIdent->identifier);
        switch (type)
        {
          case TYPE_INT:
            *pOffset = param_getValue(pIdent->identifier).type_int;
            b = TRUE;
            break;

          case TYPE_STRING:
          case TYPE_DOUBLE:
          default:
            fprintf(stdout, "error: type '%s' can't be convert to integer\n", pIdent->identifier);
            break;
        }
      }

      break;

    case J_INTEGER:
      *pOffset = ((JInteger*) pObject)->value;
      b = TRUE;
      break;

    default:
      fprintf(stdout, "this object can give offset array\n");
      break;
  }

  if (b)
  {
    fprintf(stdout, "offset of array is %d\n", *pOffset);
  }

  return b;
}


int ast_create_array_on_top(char* name)
{
  if (ast_nb_object != 0)
  {
    int offset;
    BOOL b;
    b = ast_get_offset(ast_list[ast_nb_object - 1], &offset);
    if (b)
    {
      ast_remove_last();
      return ast_insert_array(name, offset);
    }
  }

  return -1;
}

