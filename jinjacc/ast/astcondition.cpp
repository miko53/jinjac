#include "astcondition.h"
#include <iostream>
#include "assert.h"
#include "jinjac_compiler.h"

AstCondition::AstCondition(const AstNode* left, const AstNode* right, ConditionType operation)
{
  m_left = left;
  m_right = right;
  m_operation = operation;
}

AstCondition::~AstCondition()
{

}


void AstCondition::print() const
{
  std::string condition;
  std::cout << "<AST_CONDITION: ";
  m_left->print();

  switch (m_operation)
  {
    case AST_EQUAL:
      condition = "==";
      break;
    case AST_DIFFERENT:
      condition = "!=";
      break;
    case AST_HIGH_AND_EQUAL_THAN:
      condition = ">=";
      break;
    case AST_HIGH_THAN:
      condition = ">";
      break;
    case AST_LOWER_AND_EQUAL_THAN:
      condition = "<=";
      break;
    case AST_LOWER_THAN:
      condition = "<";
      break;
    case AST_OR:
      condition = "||";
      break;
    case AST_AND:
      condition = "&&";
      break;
    case AST_NOT:
      condition = "!";
      break;
    case AST_IS:
      condition = "IS";
      break;
    default:
      assert(false);
      break;
  }
  std::cout << " '" << condition << "' ";
  
  if (m_right != nullptr)
  {
    m_right->print();
  }
  std::cout << " AST_CONDITION>";
}

void AstCondition::printAsm(JinjacCompiler& compiler) const
{
  Code* pCodeContainer = compiler.getCodeContainer();
  
  m_left->printAsm(compiler);
  
  if (m_right != nullptr)
  {
    m_right->printAsm(compiler);
  }
  
  switch(m_operation)
  {
    case AST_EQUAL:
      pCodeContainer->insertStatement(OP_TEST_EQ);
      break;
      
    case AST_DIFFERENT:
      pCodeContainer->insertStatement(OP_TEST_NEQ);
      break;
      
    case AST_HIGH_AND_EQUAL_THAN:
      pCodeContainer->insertStatement(OP_TEST_GE);
      break;
      
    case AST_HIGH_THAN:
      pCodeContainer->insertStatement(OP_TEST_GT);
      break;
      
    case AST_LOWER_AND_EQUAL_THAN:
      pCodeContainer->insertStatement(OP_TEST_LE);
      break;
      
    case AST_LOWER_THAN:
      pCodeContainer->insertStatement(OP_TEST_LT);
      break;
      
    case AST_OR:
      pCodeContainer->insertStatement(OP_TEST_OR);
      break;
      
    case AST_AND:
      pCodeContainer->insertStatement(OP_TEST_AND);
      break;
      
    case AST_NOT:
      pCodeContainer->insertStatement(OP_TEST_NOT);
      break;
      
    case AST_IS:
      pCodeContainer->insertStatement(OP_TEST_IS);
      break;
      
    default:
      assert(false);
      break;
  }
}


void AstCondition::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}
