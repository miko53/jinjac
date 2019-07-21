#ifndef _PARAMETER_H
#define _PARAMETER_H

extern void parameter_init(void);
extern int insert_parameter(char* key, char* value);
extern char* param_getValue(char* key);


#endif /* _PARAMETER_H */

