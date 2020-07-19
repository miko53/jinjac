#ifndef ASTSTATEMENT_H
#define ASTSTATEMENT_H

#include "astnode.h"
#include <vector>

class AstStatement : public AstNode
{
public:
    AstStatement();
    virtual ~AstStatement();
    virtual void print() const override;
    virtual void push(const AstNode * node) const override;
    virtual void setStripBeginBlock(bool b) const {m_bstripBeginBlock = b; }
    virtual void setStripEndBlock(bool b) const {m_bStripEndBlock = b; }
    virtual bool getStripBeginBlock(void) const { return m_bstripBeginBlock; }
    virtual bool getStripEndBlock(void) const { return m_bStripEndBlock; }
    virtual void printAsm(JinjacCompiler & compiler) const override;

    int32_t count() const { return m_stmtList.size(); }
    
protected:
    mutable std::vector<const AstNode*> m_stmtList;
    mutable bool m_bstripBeginBlock;
    mutable bool m_bStripEndBlock;
};

class AstFor : public AstStatement
{
public:
  AstFor(std::string& name, const AstNode* iteratorStatement);
  virtual ~AstFor();
  
  virtual void print() const override;
  virtual void push(const AstNode * node) const override;
  virtual void printAsm(JinjacCompiler & compiler) const override;

protected:
  const AstNode* m_iteratorStatement;
  std::string m_identifer;
};

class AstIf : public AstStatement
{
public:
  AstIf(const AstNode* expr);
  virtual ~AstIf();
  
  virtual void print() const override;
  virtual void push(const AstNode * node) const override;
  virtual void setStripBeginBlock(bool b) const override;
  virtual void setStripEndBlock(bool b) const override;
  virtual void printAsm(JinjacCompiler & compiler) const override;

  void pushToElseStmt() const { m_bInElse = true; }
  
protected:
  const AstNode* m_expr;
  AstStatement m_else;
  mutable bool m_bInElse;
};

#endif // ASTSTATEMENT_H
