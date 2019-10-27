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
#ifndef _PARAMETER_H
#define _PARAMETER_H

#include "common.h"
#include "jinjac.h"

#ifdef __cplusplus
extern "C" {
#endif


extern void parameter_init(void);

extern void jinjac_parameter_delete_all();

extern J_STATUS parameter_delete(char* key);
extern J_STATUS parameter_insert2(char* key, jinjac_parameter_type type, jinjac_parameter_value value);
extern BOOL parameter_search(char* key, int64_t* privKey, BOOL* isArray);
extern J_STATUS parameter_get(int64_t privKey, jinjac_parameter* param);
extern J_STATUS parameter_update(char* key, jinjac_parameter_value newValue);

extern BOOL parameter_array_getProperties(int64_t privKey, jinjac_parameter_type* type, int32_t* nbItem);
extern J_STATUS parameter_array_getValue(int64_t privKey, int32_t offset, jinjac_parameter_value* v);
extern char* parameter_convertArrayToString(int64_t privKey);

extern void param_delete(jinjac_parameter* param);



#ifdef __cplusplus
}
#endif

#endif /* _PARAMETER_H */

