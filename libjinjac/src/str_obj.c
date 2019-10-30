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
#include "str_obj.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

void str_obj_create(str_obj* obj, int32_t minToAllocate)
{
  if (minToAllocate < 10)
  {
    obj->allocatedSize = 10;
  }
  else
  {
    obj->allocatedSize = minToAllocate;
  }

  obj->s = malloc(obj->allocatedSize);
  ASSERT(obj->s != NULL);
  obj->s[0] = '\0';
  obj->size = 0;
}

void str_obj_clear(str_obj* obj)
{
  ASSERT(obj != NULL);
  ASSERT(obj->s != NULL);
  obj->s[0] = '\0';
  obj->size = 0;
}

int32_t str_obj_len(str_obj* obj)
{
  ASSERT(obj != NULL);
  return obj->size;
}

void str_obj_free(str_obj* obj)
{
  if (obj->s)
  {
    free(obj->s);
  }

  obj->s = NULL;
  obj->size = 0;
  obj->allocatedSize = 0;
}

STATIC void str_obj_realloc(str_obj* obj, int32_t newSize)
{
  ASSERT(obj != NULL);
  char* n = realloc(obj->s, newSize);
  ASSERT(n != NULL);
  obj->s = n;
  obj->allocatedSize = newSize;
}


void str_obj_insertChar(str_obj* obj, char src)
{
  ASSERT(obj != NULL);
  ASSERT(obj->s != NULL);

  if ((obj->size + 1) >= obj->allocatedSize)
  {
    str_obj_realloc(obj, obj->allocatedSize * 2);
  }

  int32_t size = obj->size;
  obj->s[size] = src;
  size++;
  obj->s[size] = '\0';
  obj->size = size;
}

void str_obj_insert(str_obj* obj, char* src)
{
  int32_t s = strlen(src);
  if (obj->size + s + 1 >= obj->allocatedSize)
  {
    str_obj_realloc(obj, obj->allocatedSize * 2);
  }

  strncpy(&obj->s[obj->size], src, s + 1);
  obj->size += s;
  obj->s[obj->size] = '\0';
}
