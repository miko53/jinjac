#ifndef _FLEX_DECL_H
#define _FLEX_DECL_H

#include <stdio.h>

// Declare stuff from Flex that Bison needs to know about:
extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string( const char* str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern int yylex_destroy(void);

#endif /* _FLEX_DECL_H */
