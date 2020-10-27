#ifndef ASTJINJAEXPR_H
#define ASTJINJAEXPR_H

#include "astnode.h"

class AstJinjaExpr : public AstNode
{
public:
    AstJinjaExpr(const AstNode* expr);
    virtual ~AstJinjaExpr();

    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;
    
protected:
    const AstNode* m_node;
};

#endif // ASTJINJAEXPR_H
