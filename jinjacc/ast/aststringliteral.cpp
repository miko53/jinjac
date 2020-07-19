#include "aststringliteral.h"
#include <iostream>
#include <assert.h>
#include "jinjac_compiler.h"

AstStringLiteral::AstStringLiteral(std::string& s)
{
  m_name = s;
}


AstStringLiteral::~AstStringLiteral()
{

}

void AstStringLiteral::print() const
{
  std::cout << "<AstStringLiteral: '"<< m_name << "'>";
}

void AstStringLiteral::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}

void AstStringLiteral::printAsm(JinjacCompiler& compiler) const
{
  StaticData* pDataContainer = compiler.getDataContainer();
  Code* pCodeContainer = compiler.getCodeContainer();
  
  int32_t offset;
  offset = pDataContainer->insertString(m_name);
  
  pCodeContainer->insertStatement(OP_PUSH_STRING, offset);
}
