#include "astvariable.h"
#include <iostream>
#include <assert.h>
#include "jinjac_compiler.h"

AstVariable::AstVariable(std::string& m)
{
  m_name = m;
}

AstVariable::~AstVariable()
{

}

void AstVariable::print() const
{
  std::cout << "<AstVariable : '" << m_name << "'>";
}

void AstVariable::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}

void AstVariable::printAsm(JinjacCompiler& compiler) const
{
  StaticData* pDataContainer = compiler.getDataContainer();
  Code* pCodeContainer = compiler.getCodeContainer();
  
  int32_t offset;
  offset = pDataContainer->insertString(m_name);
  
  pCodeContainer->insertStatement(OP_LOAD_SYMBOL, offset);
}
