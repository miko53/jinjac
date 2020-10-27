#ifndef ASTTEXT_H
#define ASTTEXT_H

#include "astnode.h"
#include <string>

class AstText : public AstNode
{
public:
    AstText(std::string& s);
    virtual ~AstText();
    void print() const override;
    void push(const AstNode * node) const override;
    void printAsm(JinjacCompiler & compiler) const override;
    
    std::string ltrim(void) const;
    std::string rtrim(void) const;
    
protected:
    mutable std::string m_text;
};

#endif // ASTTEXT_H
