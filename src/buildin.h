#ifndef BUILDIN_H
#define BUILDIN_H

#include "common.h"
#include "parameter.h"

extern char* upper(char* s);
extern char* lower(char* s);
extern char* capitalize(char* s);
extern char* title(char* s);
extern char* trim(char* s);
extern char* truncate(char* origin, unsigned int truncSize, BOOL killwords, char* endSentence,
                      unsigned int tolerateMargin);
extern char* center(char* origin, unsigned int width);
extern char* format(char* origin, int nbParameters, parameter_value* param, parameter_type* type);


#endif /* BUILDIN_H */

