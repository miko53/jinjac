
#include <stdio.h>
#include <stdlib.h>
#include "convert.h"
#include "common.h"

char* intToStr(int value)
{
  int size = 0;
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
  int size = 0;
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

double calcul_did(double d1, int i2, char operation)
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

double calcul_idd(int i1, double d2, char operation)
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

int calcul_iii(int i1, int i2, char operation)
{
  int r;

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


