#include "aststatement.h"
#include <iostream>
#include "jinjac_compiler.h"
#include "asttext.h"

AstStatement::AstStatement()
{
  m_bstripBeginBlock = false;
  m_bStripEndBlock = false;
}

AstStatement::~AstStatement()
{

}


void AstStatement::print() const
{
  if (m_bstripBeginBlock == true)
  {
    std::cout << " 'Begin of block has to be striped' " << std::endl;
  }
  
  for(uint32_t i = 0; i < m_stmtList.size(); i++)
  {
    m_stmtList[i]->print();
  }
  
  if (m_bStripEndBlock == true)
  {
    std::cout << " 'End of block has to be striped' " << std::endl;
  }
}

void AstStatement::printAsm(JinjacCompiler& compiler) const
{
  if (m_bstripBeginBlock == true)
  {
//     bool bStopTrim = false;;
//     for(uint32_t i = 0; ((i < m_stmtList.size()) && (bStopTrim==false)); i++)
//     {
      const AstText* pNodeText;
      pNodeText = dynamic_cast<const AstText*>(m_stmtList[/*i*/0]);
      if (pNodeText != nullptr)
      {
        /*std::string result = */pNodeText->ltrim();
//         if (result != "")
//         {
//           bStopTrim = true;
//         }
      }
//     }  
  }
  
  if ((m_bStripEndBlock == true) && (m_stmtList.size() > 0))
  {
//     bool bStopTrim = false;;
//     for(uint32_t i = m_stmtList.size(); ((i > 0) && (bStopTrim==false)); i++)
//     {
      const AstText* pNodeText;
      pNodeText = dynamic_cast<const AstText*>(m_stmtList[/*i*/m_stmtList.size()-1]);
      if (pNodeText != nullptr)
      {
        /*std::string result = */pNodeText->ltrim();
//         if (result != "")
//         {
//           bStopTrim = true;
//         }
      }
//     }  
  }
  
  
  for(uint32_t i = 0; i < m_stmtList.size(); i++)
  {
    m_stmtList[i]->printAsm(compiler);
  }  
  
  
}


void AstStatement::push(const AstNode* node) const
{
  m_stmtList.push_back(node);
}


AstFor::AstFor(std::string& name, const AstNode* iteratorStatement)
{
  m_identifer = name;
  m_iteratorStatement = iteratorStatement;
}


AstFor::~AstFor()
{
}

void AstFor::print() const
{
  std::cout << "<FOR '" << m_identifer << "'"<< std::endl;
  AstStatement::print();
  std::cout << "END_FOR>" << std::endl;
}

void AstFor::push(const AstNode* node) const
{
  AstStatement::push(node);
}

void AstFor::printAsm(JinjacCompiler& compiler) const
{
  Code* pCodeContainer = compiler.getCodeContainer();
  StaticData* pDataContainer = compiler.getDataContainer();
  uint32_t labelTest;
  uint32_t labelEndLoop;
  uint32_t offsetBranchOfEndLoop;
  
  int32_t offsetIdentifierName;
  offsetIdentifierName = pDataContainer->insertString(m_identifer);
  
  pCodeContainer->insertStatement(OP_CREATE_SYMBOL, offsetIdentifierName);

  //setter le symbol avec la valeur de l'iterateur
  
  pCodeContainer->insertStatement(OP_LOAD_SYMBOL, offsetIdentifierName);

  m_iteratorStatement->printAsm(compiler);
  
  pCodeContainer->insertStatement(OP_CREATE_ITERATOR);
  pCodeContainer->insertStatement(OP_ITERATOR_LOAD_FIRST);
  pCodeContainer->insertStatement(OP_SET_SYMBOL, offsetIdentifierName);
  pCodeContainer->insertStatement(OP_POP);
  labelTest = pCodeContainer->insertStatement(OP_ITERATOR_TEST_END_REACHED);
  offsetBranchOfEndLoop = pCodeContainer->insertStatement(OP_B_TRUE, -1);
  
  //loop statement
  AstStatement::printAsm(compiler);
  
  //incrementer le symbol
  pCodeContainer->insertStatement(OP_ITERATOR_INCR);
  pCodeContainer->insertStatement(OP_ITERATOR_LOAD_NEXT);
  pCodeContainer->insertStatement(OP_SET_SYMBOL, offsetIdentifierName);
  pCodeContainer->insertStatement(OP_POP);
   
  //branch au debut
  pCodeContainer->insertStatement(OP_JUMP, labelTest);
  labelEndLoop  = pCodeContainer->insertStatement(OP_DELETE_SYMBOL, offsetIdentifierName);
  pCodeContainer->insertStatement(OP_POP); //identifier
  pCodeContainer->insertStatement(OP_POP); //iterator statement
  pCodeContainer->insertStatement(OP_POP); //iterator object
  
  pCodeContainer->replaceStatement(offsetBranchOfEndLoop, OP_B_TRUE, labelEndLoop);
}



AstIf::AstIf(const AstNode* expr)
{
  m_expr = expr;
  m_bInElse = false;
}

AstIf::~AstIf()
{
}

void AstIf::print() const
{
  std::cout << "<IF: ";
  m_expr->print();
  std::cout << "<THEN: " << std::endl;
  AstStatement::print();
  std::cout << "<END THEN>" << std::endl;
  
  std::cout << "<ELSE: " << std::endl;
  m_else.print();
  std::cout << "<END ELSE> " << std::endl;
  std::cout << "END IF> " << std::endl;
}

void AstIf::push(const AstNode* node) const
{
  if (m_bInElse)
  {
    m_else.push(node);
  }
  else
  {
    AstStatement::push(node);
  }
}
#include "common.h"
#include "asttext.h"

void AstIf::setStripBeginBlock(bool b) const
{
  dbg_print("AstIf::setStripBeginBlock(%d) const\n", b);
  dbg_print("m_bInElse = %d\n", m_bInElse);
  
  if (b == true)
  {
    dbg_print("go\n");
  }
  
  if (m_bInElse)
    m_else.setStripBeginBlock(b);
  else
   AstStatement::setStripBeginBlock(b);
}

void AstIf::setStripEndBlock(bool b) const
{
  dbg_print("AstIf::setStripEndBlock(%d) const\n", b);
  dbg_print("m_bInElse = %d\n", m_bInElse);
  if (m_bInElse)
    m_else.setStripEndBlock(b);
  else
    AstStatement::setStripEndBlock(b);
}


void AstIf::printAsm(JinjacCompiler& compiler) const
{
  Code* pCodeContainer = compiler.getCodeContainer();
//   StaticData* pDataContainer = compiler.getDataContainer();
  uint32_t offsetElse = -1;
  uint32_t offsetEnd = -1;
  uint32_t branchElse;
  uint32_t branchEnd;
  
  
  m_expr->printAsm(compiler);
  //then part
  branchElse = pCodeContainer->insertStatement(OP_B_FALSE, offsetElse);
  
  AstStatement::printAsm(compiler);
  branchEnd = pCodeContainer->insertStatement(OP_JUMP, offsetEnd);
  offsetElse = pCodeContainer->getCurrent(); //else part
  m_else.printAsm(compiler);
  offsetEnd= pCodeContainer->getCurrent(); //else part

  pCodeContainer->replaceStatement(branchElse, OP_B_FALSE, offsetElse);
  pCodeContainer->replaceStatement(branchEnd, OP_JUMP, offsetEnd);
}
