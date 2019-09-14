#ifndef _CONVERT_H
#define _CONVERT_H

#ifdef __cplusplus
extern "C" {
#endif

extern char* intToStr(int value);
extern char* doubleToStr(double value);

extern double calcul_ddd(double d1, double d2, char operation);
extern double calcul_did(double d1, int i2, char operation);
extern double calcul_idd(int i1, double d2, char operation);
extern int calcul_iii(int i1, int i2, char operation);

#ifdef __cplusplus
}
#endif

#endif /* _CONVERT_H */
