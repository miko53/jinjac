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

#include <stdio.h>
#include <stdlib.h>
#include "convert.h"

char* intToStr(int32_t value)
{
  int32_t size = 0;
  char* p = NULL;
  size = snprintf(p, size, "%d", value);
  if (size < 0)
  {
    p = NULL;
  }
  else
  {
    size++;
    p = malloc(size);
    ASSERT(p != NULL);
    snprintf(p, size, "%d", value);
  }

  return p;
}

char* doubleToStr(double value)
{
  int32_t size = 0;
  char* p = NULL;
  size = snprintf(p, size, "%f", value);
  if (size < 0)
  {
    p = NULL;
  }
  else
  {
    size++;
    p = malloc(size);
    ASSERT(p != NULL);
    snprintf(p, size, "%f", value);
  }

  return p;
}

double calcul_ddd(double d1, double d2, char operation)
{
  double r;

  switch (operation)
  {
    case '+':
      r = d1 + d2;
      break;

    case '-':
      r = d1 - d2;
      break;

    case '*':
      r = d1 * d2;
      break;

    case '/':
      r = d1 / d2;
      break;

    default:
      r = 0.;
      ASSERT(FALSE);
      break;
  }

  return r;
}

double calcul_did(double d1, int32_t i2, char operation)
{
  double r;

  switch (operation)
  {
    case '+':
      r = d1 + i2;
      break;

    case '-':
      r = d1 - i2;
      break;

    case '*':
      r = d1 * i2;
      break;

    case '/':
      r = d1 / i2;
      break;

    default:
      r = 0.;
      ASSERT(FALSE);
      break;
  }

  return r;
}

double calcul_idd(int32_t i1, double d2, char operation)
{
  double r;

  switch (operation)
  {
    case '+':
      r = i1 + d2;
      break;

    case '-':
      r = i1 - d2;
      break;

    case '*':
      r = i1 * d2;
      break;

    case '/':
      r = i1 / d2;
      break;

    default:
      r = 0.;
      ASSERT(FALSE);
      break;
  }

  return r;
}

int32_t calcul_iii(int32_t i1, int32_t i2, char operation)
{
  int32_t r;

  switch (operation)
  {
    case '+':
      r = i1 + i2;
      break;

    case '-':
      r = i1 - i2;
      break;

    case '*':
      r = i1 * i2;
      break;

    case '/':
      r = i1 / i2;
      break;

    default:
      r = 0;
      ASSERT(FALSE);
      break;
  }

  return r;
}

BOOL compare_ddd(double d1, double d2, jobject_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case AST_EQUAL:
      r = (d1 == d2);
      break;

    case AST_DIFFERENT:
      r = (d1 != d2);
      break;

    case AST_HIGH_AND_EQUAL_THAN:
      r = (d1 >= d2);
      break;

    case AST_HIGH_THAN:
      r = (d1 > d2);
      break;

    case AST_LOWER_AND_EQUAL_THAN:
      r = (d1 <= d2);
      break;

    case AST_LOWER_THAN:
      r = (d1 < d2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}

BOOL compare_did(double d1, int32_t i2, jobject_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case AST_EQUAL:
      r = (d1 == i2);
      break;

    case AST_DIFFERENT:
      r = (d1 != i2);
      break;

    case AST_HIGH_AND_EQUAL_THAN:
      r = (d1 >= i2);
      break;

    case AST_HIGH_THAN:
      r = (d1 > i2);
      break;

    case AST_LOWER_AND_EQUAL_THAN:
      r = (d1 <= i2);
      break;

    case AST_LOWER_THAN:
      r = (d1 < i2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}

BOOL compare_idd(int32_t i1, double d2, jobject_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case AST_EQUAL:
      r = (i1 == d2);
      break;

    case AST_DIFFERENT:
      r = (i1 != d2);
      break;

    case AST_HIGH_AND_EQUAL_THAN:
      r = (i1 >= d2);
      break;

    case AST_HIGH_THAN:
      r = (i1 > d2);
      break;

    case AST_LOWER_AND_EQUAL_THAN:
      r = (i1 <= d2);
      break;

    case AST_LOWER_THAN:
      r = (i1 < d2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}

BOOL compare_iii(int32_t i1, int32_t i2, jobject_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case AST_EQUAL:
      r = (i1 == i2);
      break;

    case AST_DIFFERENT:
      r = (i1 != i2);
      break;

    case AST_HIGH_AND_EQUAL_THAN:
      r = (i1 >= i2);
      break;

    case AST_HIGH_THAN:
      r = (i1 > i2);
      break;

    case AST_LOWER_AND_EQUAL_THAN:
      r = (i1 <= i2);
      break;

    case AST_LOWER_THAN:
      r = (i1 < i2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}

