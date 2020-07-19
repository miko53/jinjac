#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
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
      //ASSERT(FALSE);
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
      //ASSERT(FALSE);
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
      //ASSERT(FALSE);
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
      //ASSERT(FALSE);
      break;
  }

  return r;
}

BOOL compare_ddd(double d1, double d2, j_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case COND_EQUAL:
      r = (d1 == d2);
      break;

    case COND_DIFFERENT:
      r = (d1 != d2);
      break;

    case COND_HIGH_AND_EQUAL_THAN:
      r = (d1 >= d2);
      break;

    case COND_HIGH_THAN:
      r = (d1 > d2);
      break;

    case COND_LOWER_AND_EQUAL_THAN:
      r = (d1 <= d2);
      break;

    case COND_LOWER_THAN:
      r = (d1 < d2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}


BOOL compare_did(double d1, int32_t i2, j_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case COND_EQUAL:
      r = (d1 == i2);
      break;

    case COND_DIFFERENT:
      r = (d1 != i2);
      break;

    case COND_HIGH_AND_EQUAL_THAN:
      r = (d1 >= i2);
      break;

    case COND_HIGH_THAN:
      r = (d1 > i2);
      break;

    case COND_LOWER_AND_EQUAL_THAN:
      r = (d1 <= i2);
      break;

    case COND_LOWER_THAN:
      r = (d1 < i2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}

BOOL compare_idd(int32_t i1, double d2, j_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case COND_EQUAL:
      r = (i1 == d2);
      break;

    case COND_DIFFERENT:
      r = (i1 != d2);
      break;

    case COND_HIGH_AND_EQUAL_THAN:
      r = (i1 >= d2);
      break;

    case COND_HIGH_THAN:
      r = (i1 > d2);
      break;

    case COND_LOWER_AND_EQUAL_THAN:
      r = (i1 <= d2);
      break;

    case COND_LOWER_THAN:
      r = (i1 < d2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}

BOOL compare_iii(int32_t i1, int32_t i2, j_condition condition)
{
  BOOL r;
  r = FALSE;

  switch (condition)
  {
    case COND_EQUAL:
      r = (i1 == i2);
      break;

    case COND_DIFFERENT:
      r = (i1 != i2);
      break;

    case COND_HIGH_AND_EQUAL_THAN:
      r = (i1 >= i2);
      break;

    case COND_HIGH_THAN:
      r = (i1 > i2);
      break;

    case COND_LOWER_AND_EQUAL_THAN:
      r = (i1 <= i2);
      break;

    case COND_LOWER_THAN:
      r = (i1 < i2);
      break;

    default:
      ASSERT(FALSE);
      break;
  }

  return r;
}
