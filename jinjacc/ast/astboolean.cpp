#include "astboolean.h"
#include <assert.h>
#include <iostream>
#include "jinjac_compiler.h"

AstBoolean::AstBoolean(bool b)
{
  m_value = b;
}

AstBoolean::~AstBoolean()
{

}

void AstBoolean::print() const
{
  if (m_value)
    std::cout << "<BOOLEAN: TRUE>"; 
  else
    std::cout << "<BOOLEAN: FALSE>"; 
}

void AstBoolean::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}

void AstBoolean::printAsm(JinjacCompiler& compiler) const
{
  Code* pCodeContainer = compiler.getCodeContainer();
  
  if (m_value)
  {
    pCodeContainer->insertStatement(OP_LOAD_INTEGER, 1);
  }
  else
    pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0);
}
