#ifndef _AST_H
#define _AST_H

#include "common.h"

typedef struct
{
  BOOL inError;
  char* currentStringValue;
} ast;

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
