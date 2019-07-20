#include "ast.h"

ast ast_root;

void ast_clean()
{
  ast_root.inError = FALSE;
  ast_root.type = 0;
}

ast* getAstRoot(void)
{
  return &ast_root;
}
