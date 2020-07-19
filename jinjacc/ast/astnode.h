#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>

class JinjacCompiler;

class AstNode
{
public:
    AstNode();
    virtual ~AstNode();

    virtual void push(const AstNode* node) const = 0;
    virtual void print(void) const = 0;
    virtual void printAsm(JinjacCompiler& compiler) const = 0;
    
protected:
};


#endif // ASTNODE_H
