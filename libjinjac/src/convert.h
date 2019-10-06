/*
 * Copyright (c) 2019 miko53
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _CONVERT_H
#define _CONVERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "jobject.h"

extern char* intToStr(int32_t value);
extern char* doubleToStr(double value);

extern double calcul_ddd(double d1, double d2, char operation);
extern double calcul_did(double d1, int32_t i2, char operation);
extern double calcul_idd(int32_t i1, double d2, char operation);
extern int32_t calcul_iii(int32_t i1, int32_t i2, char operation);

extern BOOL compare_ddd(double d1, double d2, jobject_condition condition);
extern BOOL compare_did(double d1, int32_t i2, jobject_condition condition);
extern BOOL compare_idd(int32_t i1, double d2, jobject_condition condition);
extern BOOL compare_iii(int32_t i1, int32_t i2, jobject_condition condition);


#ifdef __cplusplus
}
#endif

#endif /* _CONVERT_H */
