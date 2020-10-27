#include "astarray.h"
#include <iostream>
#include "assert.h"
#include "jinjac_compiler.h"

AstArray::AstArray(const AstNode* firstItems)
{
  m_items.push_back(firstItems);
  m_isTuple = false;
}

AstArray::~AstArray()
{
}

void AstArray::print() const
{
  if (m_isTuple)
    std::cout << "<TUPLE: ";
  else
    std::cout << "<ARRAY: ";
  
  for(uint32_t i = 0; i < m_items.size(); i++)
  {
      m_items[i]->print();
  }
  
  if (m_isTuple)
    std::cout << "TUPLE>" << std::endl;
  else
    std::cout << "ARRAY>" << std::endl;
}

void AstArray::push(const AstNode* node) const
{
  m_items.push_back(node);
}

void AstArray::printAsm(JinjacCompiler& compiler) const
{
  Code* pCodeContainer = compiler.getCodeContainer();
  

  for(int32_t i = m_items.size() - 1; i >= 0; i--)
  {
    m_items[i]->printAsm(compiler);
  }
  
  pCodeContainer->insertStatement(OP_LOAD_INTEGER, m_items.size());
  
  if (m_isTuple)
    pCodeContainer->insertStatement(OP_CREATE_TUPLE);
  else
    pCodeContainer->insertStatement(OP_CREATE_ARRAY);
  
}
