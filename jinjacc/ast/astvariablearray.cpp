#include "astvariablearray.h"
#include "jinjac_compiler.h"
#include <iostream>
#include <assert.h>

AstVariableArray::AstVariableArray(std::string& name, const AstNode* offsetExprs)
{
  m_name = name;
  m_offsetExprs = offsetExprs;
}

AstVariableArray::~AstVariableArray()
{
//   delete m_offsetExprs;
}

void AstVariableArray::print() const
{
  std::cout << "<AstVariableArray: '" << m_name << "' : ";
  std::cout << "..";
  m_offsetExprs->print();
  std::cout << ">" << std::endl;
}

void AstVariableArray::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}


void AstVariableArray::printAsm(JinjacCompiler& compiler) const
{
  int32_t offsetIdentifier;
  StaticData* pDataContainer = compiler.getDataContainer();
  Code* pCodeContainer = compiler.getCodeContainer();
  m_offsetExprs->printAsm(compiler);
  
  offsetIdentifier = pDataContainer->insertString(m_name);
  
  pCodeContainer->insertStatement(OP_LOAD_SYMBOL, offsetIdentifier);
  pCodeContainer->insertStatement(OP_GET_AT);
}
