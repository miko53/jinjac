#ifndef AST_H
#define AST_H

#include <vector>
#include "astnode.h"

class AstStatement;
class JinjacCompiler;

class Ast
{
public:
    Ast();
    virtual ~Ast();

    void print(void);
    void printAsm(JinjacCompiler& compiler);
    void insert(const AstNode* stmt);
    void push(const AstNode* stmt);
    void pop(bool stripEndBlock = false);
    
    const AstNode* top();
    
protected:
    std::vector<const AstNode* > m_listItem;
    std::vector<const AstNode* > m_statementStack;
};

#endif // AST_H
