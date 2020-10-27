#include "ast.h"
#include "aststatement.h"
#include "jinjac_compiler.h"

Ast::Ast()
{

}

Ast::~Ast()
{

}

void Ast::print()
{
  for(uint32_t i = 0; i < m_listItem.size(); i++)
  {
    m_listItem[i]->print();
  }
}

void Ast::printAsm(JinjacCompiler& compiler)
{
  for(uint32_t i = 0; i < m_listItem.size(); i++)
  {
    m_listItem[i]->printAsm(compiler);
  }
  compiler.getCodeContainer()->insertStatement(OP_HALT);
}


void Ast::insert(const AstNode* stmt)
{
  if (m_statementStack.size() > 0)
  {
    const AstNode* n = top();
    n->push(stmt);
  }
  else
    m_listItem.push_back(stmt);
}

void Ast::push(const AstNode* stmt)
{
  m_statementStack.push_back(stmt);
}

void Ast::pop(bool stripEndBlock)
{
  if (m_statementStack.size() > 0)
  {
    const AstNode* n = top();
    const AstStatement* s =  dynamic_cast < const AstStatement* > (n);
    s->setStripEndBlock(stripEndBlock);
  }
  
  m_statementStack.pop_back();
}

const AstNode * Ast::top()
{
  return m_statementStack.back();
}
