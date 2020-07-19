#include "astfunction.h"
#include <iostream>
#include <assert.h>
#include "jinjac_compiler.h"

AstFunction::AstFunction(std::string& name, const AstNode* pObj, const AstNode* param)
{
  m_name = name;
  m_obj = pObj;
  m_paramList = param;
}

AstFunction::~AstFunction()
{
//   if (m_paramList)
//     delete m_paramList;
}


void AstFunction::print() const
{
  std::cout << "<FUNCTION: '" << m_name << "' ";
  if (m_obj)
  {
    std::cout << "IMPACTED OBJ: ";
    m_obj->print();
  }
  if (m_paramList)
  {
    std::cout << "<ARGS: ";
    m_paramList->print();
    std::cout << " ARGS>";
  }
  std::cout << " FUNCTION>" << std::endl;
}


void AstFunction::push(const AstNode* node) const
{
  if (m_obj == nullptr)
  {
    m_obj = node;
  }
  else
    assert(false);
}

const char* vm_buildin_fct[]
{
  "capitalize", 
  "center",
  "format", 
  "join", 
  "lower", 
  "upper", 
  "range", 
  "title",
  "trim", 
  "truncate",
};

buildin_fct_id AstFunction::getBuildinFctId(void) const
{
  int32_t r = -1;
  
  for(uint32_t i=0; i < sizeof(vm_buildin_fct)/sizeof(const char*); i++)
  {
    if (m_name == vm_buildin_fct[i])
    {
      r = i;
      break;
    }
  }
  
  return (buildin_fct_id) r;
}


void AstFunction::printAsm(JinjacCompiler& compiler) const
{
  Code* pCodeContainer = compiler.getCodeContainer();
  StaticData* pDataContainer = compiler.getDataContainer();
  int32_t offset;
  int32_t nbArgs;
  const AstFunctionArgument* pArgs = dynamic_cast<const AstFunctionArgument*>(m_paramList);
  nbArgs = 0;
  
//   if (m_paramList)
//   {
//     m_paramList->printAsm(compiler);
//   }
  
  buildin_fct_id fctIndex;
  fctIndex = getBuildinFctId();
  if (fctIndex == (buildin_fct_id) -1)
  {
    std::cerr << "Unknown function " << m_name << std::endl;
    exit(EXIT_FAILURE);
  }
  else
  {
    //check if all default argument are correctly pushed before to continue
    switch(fctIndex)
    {
      case FCT_CAPITALIZE:
      case FCT_LOWER:
      case FCT_UPPER:
      case FCT_TITLE:
      case FCT_TRIM:
        if (pArgs->nbArgs() != 0)
        {
          std::cerr << "error: no argument is expected for " << m_name << std::endl;
          exit(EXIT_FAILURE);
        }
        break;
        
      case FCT_TRUNCATE:
        //insert default argument if not present
        switch (pArgs->nbArgs())
        {
          case 0:
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0); //tolerateMargin
            offset = pDataContainer->insertString("...");
            pCodeContainer->insertStatement(OP_PUSH_STRING, offset); //end with "..."
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0); //killWord = FALSE
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 255); //lenght = 255
            break;
            
          case 1:
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0); //tolerateMargin
            offset = pDataContainer->insertString("...");
            pCodeContainer->insertStatement(OP_PUSH_STRING, offset); //end with "..."
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0); //killWord = FALSE
            break;
            
          case 2:
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0); //tolerateMargin
            offset = pDataContainer->insertString("...");
            pCodeContainer->insertStatement(OP_PUSH_STRING, offset); //end with "..."
            break;
            
          case 3:
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0); //tolerateMargin
            break;
            
          case 4:
            break;
            
          default:
            std::cerr << "error: no argument that 4 args is expected for truncate function" << m_name << std::endl;
            exit(EXIT_FAILURE);
            break;
        }
        nbArgs = 4;
        m_paramList->printAsm(compiler);
        break;
        
      case FCT_CENTER:
        switch (pArgs->nbArgs())
        {
          case 0:
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 80); //width = 80
            break;
          case 1:
            break;
            
          default:
            std::cerr << "error: no argument that 1 arg is expected for center function" << m_name << std::endl;
            exit(EXIT_FAILURE);
            break;
        }
        nbArgs = 1;
        m_paramList->printAsm(compiler);
        break;
        
      case FCT_FORMAT:
        nbArgs = pArgs->nbArgs();
        m_paramList->printAsm(compiler);
      break;
      
      case FCT_RANGE:
        switch (pArgs->nbArgs())
        {
          case 0:
            std::cerr << "error: range can't have no argument" << std::endl;
            exit(EXIT_FAILURE);
            break;
          case 1:
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 1); // step            
            m_paramList->printAsm(compiler); 
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 0); // start
            break;
            
          case 2:
            pCodeContainer->insertStatement(OP_LOAD_INTEGER, 1); // step            
            m_paramList->printAsm(compiler);
            break;
            
          default:
            m_paramList->printAsm(compiler);
            break;
        }
        nbArgs = 3;
        break;
        
      case FCT_JOIN:
        if (pArgs->nbArgs() == 0)
        {
          offset = pDataContainer->insertString("");
          pCodeContainer->insertStatement(OP_PUSH_STRING, offset); //string empty by default for join function
        }
        else if (pArgs->nbArgs() == 1)
        {
          m_paramList->printAsm(compiler);
        }
        else
        {
          std::cerr << "error: no argument that 1 arg is expected for join() function" << m_name << std::endl;
          exit(EXIT_FAILURE);
        }
        nbArgs=1;
        break;

      default:
        assert(false);
        break;
    }
    
    if (m_obj)
    {
      m_obj->printAsm(compiler);
    }
    else
    {
      pCodeContainer->insertStatement(OP_PUSH_NIL); //???
    }
    
    nbArgs++;
    pCodeContainer->insertStatement(OP_CALL_BUILDIN_FCT, fctIndex, nbArgs);
  }  
}


AstFunctionArgument::AstFunctionArgument(const AstNode* first)
{
  if (first != nullptr)
    m_argumentList.push_back(first);
}


AstFunctionArgument::~AstFunctionArgument()
{
  
}


void AstFunctionArgument::print() const
{
  for(uint32_t i = 0 ;i < m_argumentList.size(); i++)
  {
    m_argumentList[i]->print();
  }
}

void AstFunctionArgument::push(const AstNode* node) const
{
  m_argumentList.push_back(node);
}

void AstFunctionArgument::printAsm(JinjacCompiler& compiler) const
{
  if (m_argumentList.size() != 0)
  {
    for(int32_t i = m_argumentList.size()-1 ;i >= 0; i--)
    {
      m_argumentList[i]->printAsm(compiler);
    }
  }
}
