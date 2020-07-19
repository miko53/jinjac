#include "astcompute.h"
#include <iostream>
#include <assert.h>
#include "jinjac_compiler.h"
#include "astnumber.h"

AstCompute::AstCompute(const AstNode* left, const AstNode* right, char operation)
{
  m_left = left;
  m_right = right;
  m_operation = operation;
}

AstCompute::~AstCompute()
{
  
}


void AstCompute::print() const
{
  std::cout << "<COMPUTE: ";
  m_left->print();
  std::cout << "'" << m_operation << "'";
  m_right->print();
  std::cout << " COMPUTE>" << std::endl;
}

void AstCompute::push(const AstNode* node) const
{
  (void) node;
  assert(false);
}

void AstCompute::printAsm(JinjacCompiler& compiler) const
{
  const AstNumber* pLeft;
  const AstNumber* pRight;
  
  pLeft = dynamic_cast<const AstNumber*>(m_left);
  pRight = dynamic_cast<const AstNumber*>(m_right);
  
  if ((pLeft != NULL) && (pRight != NULL))
  {
    AstNumber result;
    //do the computation and load value
    //std::cout << " can compute directly !" << std::endl;    
    
    switch (m_operation)
    {
      case '+':
        result = *pLeft + *pRight;
        break;
        
      case '-':
        result = *pLeft - *pRight;
        break;
        
      case '*':
        result = *pLeft * (*pRight);
        break;
        
      case '/':
        result = *pLeft / *pRight;
        break;
        
      default:
        std::cerr << "unknown operation: " << m_operation << std::endl;
        exit(1);
        break;
    }
    result.printAsm(compiler);
  }
  else
  {
    m_left->printAsm(compiler);
    m_right->printAsm(compiler);
    
    switch (m_operation)
    {
      case '+':
        compiler.getCodeContainer()->insertStatement(OP_ADD);
        break;
        
      case '-':
        compiler.getCodeContainer()->insertStatement(OP_SUB);
        break;
        
      case '*':
        compiler.getCodeContainer()->insertStatement(OP_MUL);
        break;
        
      case '/':
        compiler.getCodeContainer()->insertStatement(OP_DIV);
        break;
        
      default:
        std::cerr << "unknown operation: " << m_operation << std::endl;
        exit(1);
        break;
    }
  }
}
