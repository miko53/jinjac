#ifndef ASTFUNCTION_H
#define ASTFUNCTION_H

#include "astnode.h"
#include <vector>
#include "vm_opcode.h"

class AstFunction : public AstNode
{
public:
    AstFunction(std::string& name, const AstNode* pObj, const AstNode* param);
    virtual ~AstFunction();
    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;

protected:
    buildin_fct_id getBuildinFctId(void) const;
  
protected:
    std::string m_name;
    //std::vector<const AstNode*> m_obj;
    mutable const AstNode* m_obj;
    const AstNode* m_paramList;
};

class AstFunctionArgument : public AstNode
{
public:
  AstFunctionArgument(const AstNode* first = nullptr);
  virtual ~AstFunctionArgument();
  
  void print() const override;
  void push(const AstNode * node) const override;
  void printAsm(JinjacCompiler & compiler) const override;
  
  uint32_t nbArgs(void) const { return m_argumentList.size(); }
  
protected:
  mutable std::vector<const AstNode*> m_argumentList;
  
};

#endif // ASTFUNCTION_H
