#include "astnumber.h"
#include "assert.h"
#include "jinjac_compiler.h"
#include <iostream>

AstNumber::AstNumber(double number)
{
  m_type = DOUBLE;
  m_value.dlb = number;
}

AstNumber::AstNumber(int32_t number)
{
  m_type = INTEGER;
  m_value.integer = number;
}

AstNumber::AstNumber()
{
}


AstNumber::~AstNumber()
{

}

void AstNumber::print() const
{
  if (m_type == INTEGER)
  {
    std::cout << "<INT: " << m_value.integer << ">";
  }
  else if (m_type == DOUBLE)
  {
    std::cout << "<DOUBLE: " << m_value.dlb << ">";
  }
  else
    assert(false);
  
}

void AstNumber::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}

// 1 INT INT
// 2 INT DOUBLE
// 3 DOUBLE INT
// 4 DOUBLE DOUBLE
int32_t AstNumber::getOperationType(const AstNumber& r) const
{
  int32_t t;
  t = 0;
  
    if (m_type == INTEGER)
    {
      if (r.m_type == INTEGER)
        t = 1;
      else
        t = 2;
    }
    else if (m_type == DOUBLE)
    {
      if (r.m_type == INTEGER)
        t = 3;
      else
        t = 4;
    }
    return t;
}


AstNumber AstNumber::operator+(const AstNumber& r) const
{
  AstNumber result;
  int32_t t = getOperationType(r);
  switch (t)
  {
    case 1:
      result.m_type = INTEGER;
      result.m_value.integer = m_value.integer + r.m_value.integer;
      break;
      
    case 2:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.integer + r.m_value.dlb;
      break;
      
    case 3:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb + r.m_value.integer;
      break;
      
    case 4:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb + r.m_value.dlb;
      break;
      
    default:
      assert(false);
      break;
  }
  
  return result;
}

AstNumber AstNumber::operator-(const AstNumber& r) const
{
  AstNumber result;
  int32_t t = getOperationType(r);
  switch (t)
  {
    case 1:
      result.m_type = INTEGER;
      result.m_value.integer = m_value.integer - r.m_value.integer;
      break;
      
    case 2:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.integer - r.m_value.dlb;
      break;
      
    case 3:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb - r.m_value.integer;
      break;
      
    case 4:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb - r.m_value.dlb;
      break;
      
    default:
      assert(false);
      break;
  }
  
  return result;
}

AstNumber AstNumber::operator*(const AstNumber& r) const
{
  AstNumber result;
  int32_t t = getOperationType(r);
  switch (t)
  {
    case 1:
      result.m_type = INTEGER;
      result.m_value.integer = m_value.integer * r.m_value.integer;
      break;
      
    case 2:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.integer * r.m_value.dlb;
      break;
      
    case 3:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb * r.m_value.integer;
      break;
      
    case 4:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb * r.m_value.dlb;
      break;
      
    default:
      assert(false);
      break;
  }
  
  return result;
}

AstNumber AstNumber::operator/(const AstNumber& r) const
{
  AstNumber result;
  int32_t t = getOperationType(r);
  switch (t)
  {
    case 1:
      result.m_type = INTEGER;
      result.m_value.integer = m_value.integer / r.m_value.integer;
      break;
      
    case 2:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.integer / r.m_value.dlb;
      break;
      
    case 3:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb / r.m_value.integer;
      break;
      
    case 4:
      result.m_type = DOUBLE;
      result.m_value.dlb = m_value.dlb / r.m_value.dlb;
      break;
      
    default:
      assert(false);
      break;
  }
  
  return result;
}

void AstNumber::printAsm(JinjacCompiler& compiler) const
{
  if (m_type == INTEGER)
  {
    compiler.getCodeContainer()->insertStatement(OP_LOAD_INTEGER, m_value.integer);
  }
  else
  {
    compiler.getCodeContainer()->insertStatement(OP_LOAD_DOUBLE, m_value.dlb);
  }
}
