
#ifndef _AST_H
#define _AST_H

#include "common.h"
#include "parameter.h"

typedef char* (*filter_fct)(char*, ...);

typedef enum
{
  J_STR_CONSTANTE,
  J_INTEGER,
  J_DOUBLE,
  J_BOOLEAN,
  J_IDENTIFIER,
  J_ARRAY,
  J_FUNCTION_ARGS,
  J_FUNCTION,
} ast_type;

typedef struct
{
  BOOL inError;
  char* currentStringValue;
} ast;

typedef struct
{
  ast_type type;
} JObject;

typedef struct
{
  JObject base;
  char* str_constant;
} JStringConstante;

typedef struct
{
  JObject base;
  int value;
} JInteger;

typedef struct
{
  JObject base;
  double value;
} JDouble;

typedef struct
{
  JObject base;
  BOOL value;
} JBoolean;

typedef struct
{
  JObject base;
  char* identifier;
} JIdentifier;

typedef struct
{
  JObject base;
  char* identifier;
  int offset;
} JArray;

#define NB_MAX_ARGS   (10)

typedef struct
{
  JObject base;
  JObject* listArgs[NB_MAX_ARGS];
  int nb_args;
} JArgs;

typedef enum
{
    FCT_CAPITALIZE,
    FCT_CENTER,
    FCT_LOWER,
    FCT_UPPER,
    FCT_TITLE,
    FCT_TRIM,
    FCT_TRUNCATE
} fct_id;

typedef struct
{
  JObject base;
  fct_id functionID;
  JArgs* argList;
} JFunction;

extern ast* getAstRoot(void);
extern void ast_clean(void);
//extern filter_fct getFunction(char* fctName);

extern int ast_insert_constante(char* name);
extern int ast_insert_identifier(char* name);
extern int ast_insert_integer(int i);
extern int ast_insert_double(double d);
extern int ast_insert_boolean(BOOL b);
extern int ast_insert_function(char* fct);
extern int ast_create_array_on_top(char* name);
extern int ast_create_function_args_from_top(void);
extern int ast_insert_function_args(void);

extern int ast_dump_stack(void);

extern char* ast_convert_to_string(void);
extern char* ast_apply_filtering(void);

#endif /* _AST_H */
