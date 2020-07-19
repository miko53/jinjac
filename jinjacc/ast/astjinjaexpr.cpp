#include "astjinjaexpr.h"
#include <iostream>
#include "assert.h"
#include "jinjac_compiler.h"

AstJinjaExpr::AstJinjaExpr(const AstNode* expr)
{
  m_node = expr;
}

AstJinjaExpr::~AstJinjaExpr()
{
  
}

void AstJinjaExpr::print() const
{
  std::cout << "<JINJA_EXPRESSION: " ;
  m_node->print();
  std::cout << " JINJA_EXPRESSION>" << std::endl;
}

void AstJinjaExpr::printAsm(JinjacCompiler& compiler) const
{
  m_node->printAsm(compiler);
  compiler.getCodeContainer()->insertStatement(OP_EVAL);
}


void AstJinjaExpr::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}
