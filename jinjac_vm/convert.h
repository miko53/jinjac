#ifndef _CONVERT_H
#define _CONVERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "j_object.h"

extern char* intToStr(int32_t value);
extern char* doubleToStr(double value);

extern double calcul_ddd(double d1, double d2, char operation);
extern double calcul_did(double d1, int32_t i2, char operation);
extern double calcul_idd(int32_t i1, double d2, char operation);
extern int32_t calcul_iii(int32_t i1, int32_t i2, char operation);

extern BOOL compare_ddd(double d1, double d2, j_condition condition);
extern BOOL compare_did(double d1, int32_t i2, j_condition condition);
extern BOOL compare_idd(int32_t i1, double d2, j_condition condition);
extern BOOL compare_iii(int32_t i1, int32_t i2, j_condition condition);

#ifdef __cplusplus
}
#endif

#endif /* _CONVERT_H */
