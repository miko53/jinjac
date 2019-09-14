#ifndef _BUILDIN_H
#define _BUILDIN_H

#include "common.h"
#include "parameter.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char* upper(char* s);
extern char* lower(char* s);
extern char* capitalize(char* s);
extern char* title(char* s);
extern char* trim(char* s);
extern char* truncate(char* origin, unsigned int truncSize, BOOL killwords, char* endSentence,
                      unsigned int tolerateMargin);
extern char* center(char* origin, unsigned int width);
extern char* format(char* origin, int nbParameters, parameter* param);

#ifdef __cplusplus
}
#endif

#endif /* _BUILDIN_H */

